#!/usr/bin/env python3
"""Import an external OBJ model (or a zip containing one) as a SuperEZ scene.

Produces bin/Editor/assets/<name>/ with:
  <name>.obj   - normalized mesh library (triangulated, absolute indices,
                 one 'g <Object>_Mesh' group per object, v/vt/vn guaranteed)
  <name>.xml   - scene descriptor (auto-framed camera, sunlight, one
                 GameObject per mesh group)
  *.bmp        - one 24-bit BMP per material (converted map_Kd, or a small
                 solid-color texture baked from Kd)

Stdlib only. See docs/asset-import-pipeline.md for the requirements.

Usage:
  python tools/import_scene.py C:/Users/me/Downloads/bmw.zip --name bmw
"""

import argparse
import math
import re
import shutil
import struct
import sys
import tempfile
import zipfile
import zlib
from pathlib import Path

FOV_DEGREES = 36.0      # Camera.h: fov = 36 deg vertical
FAR_PLANE = 1000.0      # Camera.h: farPlane
# Mesh::name and Material::name are char[32] filled with strcpy_s, which
# ABORTS on overflow. The material name is "TEX_" + meshName
# (RenderContext.cpp:953 passes it unbounded), so mesh names (including the
# _Mesh suffix) must never exceed 31 - len("TEX_") = 27 chars.
MESH_NAME_LIMIT = 31 - len("TEX_")
BASE_NAME_LIMIT = MESH_NAME_LIMIT - len("_Mesh")


# --------------------------------------------------------------------------
# Small helpers
# --------------------------------------------------------------------------

def sanitize(name):
    out = re.sub(r"[^A-Za-z0-9_\-]", "_", name).strip("_")
    return out if out else "unnamed"


def unique_name(base, taken, max_len=None):
    if max_len is not None:
        base = base[:max_len]
    name = base
    counter = 2
    while name in taken:
        suffix = f"_{counter}"
        trimmed = base[:max_len - len(suffix)] if max_len is not None else base
        name = trimmed + suffix
        counter += 1
    taken.add(name)
    return name


class Report:
    def __init__(self):
        self.warnings = []
        self.notes = []

    def warn(self, message):
        self.warnings.append(message)

    def note(self, message):
        self.notes.append(message)


# --------------------------------------------------------------------------
# MTL parsing
# --------------------------------------------------------------------------

def parse_mtl(path, report):
    """Return {material_name: {'kd': (r,g,b), 'map_kd': str|None}}."""
    materials = {}
    current = None
    try:
        lines = path.read_text(errors="replace").splitlines()
    except OSError as error:
        report.warn(f"Could not read MTL {path.name}: {error}")
        return materials

    for line in lines:
        parts = line.split()
        if not parts:
            continue
        key = parts[0].lower()
        if key == "newmtl" and len(parts) >= 2:
            current = " ".join(parts[1:])
            materials[current] = {"kd": (0.8, 0.8, 0.8), "map_kd": None}
        elif current is None:
            continue
        elif key == "kd" and len(parts) >= 4:
            try:
                materials[current]["kd"] = tuple(float(v) for v in parts[1:4])
            except ValueError:
                report.warn(f"Bad Kd in material '{current}', using default")
        elif key == "map_kd":
            materials[current]["map_kd"] = parts[-1]
    return materials


# --------------------------------------------------------------------------
# OBJ parsing
# --------------------------------------------------------------------------

class ObjObject:
    def __init__(self, name):
        self.name = name
        # material name -> list of triangles; each triangle is a tuple of
        # three (vi, ti, ni) corner tuples (1-based absolute, ti/ni may be None)
        self.faces_by_material = {}

    def add_triangle(self, material, triangle):
        self.faces_by_material.setdefault(material, []).append(triangle)

    @property
    def triangle_count(self):
        return sum(len(t) for t in self.faces_by_material.values())


def resolve_index(raw, count):
    value = int(raw)
    return value if value > 0 else count + value + 1


def parse_obj(path, report):
    """Stream-parse an OBJ. Returns (positions, texcoords, normals, objects, mtl_files).

    positions: list of (x, y, z) floats
    texcoords: list of raw 'u v' strings
    normals:   list of (x, y, z) floats
    objects:   list of ObjObject in file order
    """
    positions = []
    texcoords = []
    normals = []
    objects = []
    mtl_files = []
    current_object = None
    current_material = None
    bad_faces = 0

    def get_object():
        nonlocal current_object
        if current_object is None:
            current_object = ObjObject(path.stem)
            objects.append(current_object)
        return current_object

    with path.open("r", errors="replace") as handle:
        for line in handle:
            if line.startswith("v "):
                parts = line.split()
                positions.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif line.startswith("vt "):
                parts = line.split()
                texcoords.append(f"{parts[1]} {parts[2] if len(parts) > 2 else '0'}")
            elif line.startswith("vn "):
                parts = line.split()
                normals.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif line.startswith("f "):
                corners = []
                ok = True
                for token in line.split()[1:]:
                    fields = token.split("/")
                    try:
                        vi = resolve_index(fields[0], len(positions))
                        ti = (resolve_index(fields[1], len(texcoords))
                              if len(fields) > 1 and fields[1] else None)
                        ni = (resolve_index(fields[2], len(normals))
                              if len(fields) > 2 and fields[2] else None)
                    except (ValueError, IndexError):
                        ok = False
                        break
                    if not (1 <= vi <= len(positions)):
                        ok = False
                        break
                    corners.append((vi, ti, ni))
                if not ok or len(corners) < 3:
                    bad_faces += 1
                    continue
                obj = get_object()
                # fan-triangulate quads / n-gons
                for i in range(1, len(corners) - 1):
                    obj.add_triangle(
                        current_material, (corners[0], corners[i], corners[i + 1]))
            elif line.startswith(("o ", "g ")):
                name = line[2:].strip()
                if name and name.lower() not in ("off", "default"):
                    # 'o X' immediately followed by 'g X' (3ds Max style) is one object
                    if current_object is None or current_object.name != name \
                            or current_object.triangle_count > 0:
                        if current_object is not None \
                                and current_object.triangle_count == 0:
                            # replace the empty placeholder
                            current_object.name = name
                        else:
                            current_object = ObjObject(name)
                            objects.append(current_object)
            elif line.startswith("usemtl"):
                current_material = line[6:].strip() or None
            elif line.startswith("mtllib"):
                mtl_files.append(line[6:].strip())

    if bad_faces:
        report.warn(f"Skipped {bad_faces} malformed face line(s)")
    objects = [o for o in objects if o.triangle_count > 0]
    return positions, texcoords, normals, objects, mtl_files


# --------------------------------------------------------------------------
# Geometry
# --------------------------------------------------------------------------

AXIS_TRANSFORMS = {
    "y": lambda v: v,                            # already Y-up
    "z": lambda v: (v[0], v[2], -v[1]),          # Z-up source -> Y-up
    "-z": lambda v: (v[0], -v[2], v[1]),
}


def face_normal(a, b, c):
    ux, uy, uz = b[0] - a[0], b[1] - a[1], b[2] - a[2]
    vx, vy, vz = c[0] - a[0], c[1] - a[1], c[2] - a[2]
    nx, ny, nz = uy * vz - uz * vy, uz * vx - ux * vz, ux * vy - uy * vx
    length = math.sqrt(nx * nx + ny * ny + nz * nz)
    if length < 1e-12:
        return (0.0, 1.0, 0.0)
    return (nx / length, ny / length, nz / length)


def compute_bounds(positions, used_indices):
    xs = [positions[i - 1] for i in used_indices]
    mins = [min(p[axis] for p in xs) for axis in range(3)]
    maxs = [max(p[axis] for p in xs) for axis in range(3)]
    return mins, maxs


# --------------------------------------------------------------------------
# BMP / PNG
# --------------------------------------------------------------------------

def write_bmp(path, width, height, rgb_rows):
    """rgb_rows: list of rows (top to bottom), each row a bytes of RGBRGB..."""
    row_size = (width * 3 + 3) & ~3
    padding = b"\x00" * (row_size - width * 3)
    pixel_data = bytearray()
    for row in reversed(rgb_rows):  # BMP is bottom-up
        bgr = bytearray()
        for x in range(width):
            r, g, b = row[x * 3], row[x * 3 + 1], row[x * 3 + 2]
            bgr += bytes((b, g, r))
        pixel_data += bgr + padding
    file_size = 54 + len(pixel_data)
    header = struct.pack("<2sIHHI", b"BM", file_size, 0, 0, 54)
    info = struct.pack("<IiiHHIIiiII", 40, width, height, 1, 24, 0,
                       len(pixel_data), 2835, 2835, 0, 0)
    path.write_bytes(header + info + pixel_data)


def bake_solid_bmp(path, kd, size=8):
    # gamma-encode: authored textures are sRGB, Kd is linear-ish
    rgb = bytes(min(255, max(0, round(255.0 * (c ** (1.0 / 2.2))))) for c in kd)
    row = rgb * size
    write_bmp(path, size, size, [row] * size)


def bake_checker_bmp(path, size=8):
    rows = []
    for y in range(size):
        row = bytearray()
        for x in range(size):
            row += b"\xff\x00\xff" if (x + y) % 2 == 0 else b"\x20\x20\x20"
        rows.append(bytes(row))
    write_bmp(path, size, size, rows)


def decode_png(data):
    """Minimal PNG decoder: 8-bit RGB/RGBA/palette, non-interlaced.
    Returns (width, height, rgb_rows) or raises ValueError."""
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("not a PNG")
    pos = 8
    width = height = None
    color_type = None
    palette = None
    idat = bytearray()
    while pos < len(data):
        length, ctype = struct.unpack(">I4s", data[pos:pos + 8])
        chunk = data[pos + 8:pos + 8 + length]
        pos += 12 + length
        if ctype == b"IHDR":
            width, height, depth, color_type, _, _, interlace = \
                struct.unpack(">IIBBBBB", chunk)
            if depth != 8 or interlace != 0 or color_type not in (2, 3, 6):
                raise ValueError(
                    f"unsupported PNG (depth={depth}, color={color_type}, "
                    f"interlace={interlace})")
        elif ctype == b"PLTE":
            palette = chunk
        elif ctype == b"IDAT":
            idat += chunk
        elif ctype == b"IEND":
            break
    raw = zlib.decompress(bytes(idat))
    channels = {2: 3, 3: 1, 6: 4}[color_type]
    stride = width * channels
    rows = []
    previous = bytearray(stride)
    offset = 0
    for _ in range(height):
        filter_type = raw[offset]
        line = bytearray(raw[offset + 1:offset + 1 + stride])
        offset += 1 + stride
        if filter_type == 1:  # Sub
            for i in range(channels, stride):
                line[i] = (line[i] + line[i - channels]) & 0xFF
        elif filter_type == 2:  # Up
            for i in range(stride):
                line[i] = (line[i] + previous[i]) & 0xFF
        elif filter_type == 3:  # Average
            for i in range(stride):
                left = line[i - channels] if i >= channels else 0
                line[i] = (line[i] + ((left + previous[i]) >> 1)) & 0xFF
        elif filter_type == 4:  # Paeth
            for i in range(stride):
                left = line[i - channels] if i >= channels else 0
                up = previous[i]
                up_left = previous[i - channels] if i >= channels else 0
                p = left + up - up_left
                pa, pb, pc = abs(p - left), abs(p - up), abs(p - up_left)
                if pa <= pb and pa <= pc:
                    predictor = left
                elif pb <= pc:
                    predictor = up
                else:
                    predictor = up_left
                line[i] = (line[i] + predictor) & 0xFF
        elif filter_type != 0:
            raise ValueError(f"bad PNG filter {filter_type}")
        previous = line
        if color_type == 2:
            rows.append(bytes(line))
        elif color_type == 6:
            rgb = bytearray()
            for x in range(width):
                rgb += line[x * 4:x * 4 + 3]
            rows.append(bytes(rgb))
        else:  # palette
            rgb = bytearray()
            for x in range(width):
                idx = line[x] * 3
                rgb += palette[idx:idx + 3]
            rows.append(bytes(rgb))
    return width, height, rows


def convert_texture(source, destination, report):
    """Convert map_Kd image to 24-bit BMP. Returns True on success."""
    suffix = source.suffix.lower()
    if suffix == ".bmp":
        shutil.copyfile(source, destination)
        return True
    if suffix == ".png":
        try:
            width, height, rows = decode_png(source.read_bytes())
        except (ValueError, zlib.error) as error:
            report.warn(f"PNG decode failed for {source.name}: {error}")
            return False
        write_bmp(destination, width, height, rows)
        return True
    report.warn(f"Unsupported texture format: {source.name}")
    return False


# --------------------------------------------------------------------------
# Output generation
# --------------------------------------------------------------------------

def build_groups(objects, report):
    """Split objects per material and assign unique sanitized group names.
    Returns list of (game_object_name, mesh_name, material, triangles)."""
    groups = []
    taken = set()
    for obj in objects:
        multi = len(obj.faces_by_material) > 1
        if multi:
            report.note(
                f"Object '{obj.name}' uses {len(obj.faces_by_material)} materials"
                " - split into one group per material")
        for material, triangles in obj.faces_by_material.items():
            base = sanitize(obj.name)
            if base.endswith("_Mesh"):  # avoid X_Mesh_Mesh on re-import
                base = base[:-5]
            if multi and material:
                # trim the object part but keep the material suffix readable
                mat = sanitize(material)
                room = BASE_NAME_LIMIT - len(mat) - 1
                if room >= 4:
                    base = f"{base[:room]}_{mat}"
                else:
                    base = f"{base}_{mat}"
            name = unique_name(base, taken, BASE_NAME_LIMIT)
            if name != base:
                report.note(f"Renamed '{base}' -> '{name}' (engine mesh"
                            f" names are limited to {MESH_NAME_LIMIT} chars)")
            groups.append((name, f"{name}_Mesh", material, triangles))
    return groups


def write_obj(path, scene_name, positions, texcoords, normals, groups):
    with path.open("w", newline="\n") as out:
        out.write(f"# Generated by tools/import_scene.py\n")
        out.write(f"mtllib {scene_name}.mtl\n")
        for p in positions:
            out.write(f"v {p[0]:.6f} {p[1]:.6f} {p[2]:.6f}\n")
        for t in texcoords:
            out.write(f"vt {t}\n")
        for n in normals:
            out.write(f"vn {n[0]:.4f} {n[1]:.4f} {n[2]:.4f}\n")
        for _, mesh_name, material, triangles in groups:
            out.write(f"g {mesh_name}\n")
            out.write("s 0\n")
            if material:
                out.write(f"usemtl {material}\n")
            for tri in triangles:
                refs = " ".join(f"{vi}/{ti}/{ni}" for vi, ti, ni in tri)
                out.write(f"f {refs}\n")


def write_mtl(path, materials_used, materials):
    with path.open("w", newline="\n") as out:
        out.write("# Generated by tools/import_scene.py\n")
        for name in materials_used:
            kd = materials.get(name, {}).get("kd", (0.8, 0.8, 0.8))
            out.write(f"newmtl {name}\n")
            out.write(f"Kd {kd[0]:.4f} {kd[1]:.4f} {kd[2]:.4f}\n")


def camera_from_bounds(mins, maxs):
    center = [(mins[i] + maxs[i]) * 0.5 for i in range(3)]
    radius = max(0.5 * math.dist(mins, maxs), 1e-3)
    distance = radius / math.tan(math.radians(FOV_DEGREES) * 0.5) * 1.1
    # stay inside the engine's fixed far plane, even if the model then
    # doesn't fully fit in the frustum
    distance = min(distance, max(FAR_PLANE * 0.95 - radius, radius * 1.2))
    direction = (0.55, 0.4, 0.66)  # pleasant three-quarter view
    norm = math.sqrt(sum(c * c for c in direction))
    position = [center[i] + direction[i] / norm * distance for i in range(3)]
    # Engine convention (Camera.cpp): forward =
    #   (cos(pitch)*sin(yaw), -sin(pitch), cos(pitch)*cos(yaw))
    d = [center[i] - position[i] for i in range(3)]
    length = math.sqrt(sum(c * c for c in d))
    d = [c / length for c in d]
    pitch = math.asin(-d[1])
    yaw = math.atan2(d[0], d[2]) % (2.0 * math.pi)
    return position, pitch, yaw, distance, radius


def write_scene_xml(path, scene_name, groups, group_textures, camera):
    position, pitch, yaw, _, _ = camera
    lines = []
    lines.append("<Scene>")
    lines.append('\t<Camera name="MainCamera">')
    lines.append(f'\t\t<Position x="{position[0]:.3f}" y="{position[1]:.3f}"'
                 f' z="{position[2]:.3f}" />')
    lines.append(f'\t\t<Rotation pitch="{pitch:.3f}" yaw="{yaw:.3f}" roll="0.0" />')
    lines.append("\t</Camera>")
    lines.append('\t<Sunlight name="Sun">')
    lines.append('\t\t<Direction x="-0.5" y="-1.0" z="-0.3" />')
    lines.append('\t\t<Color r="1.0" g="1.0" b="1.0" />')
    lines.append('\t\t<Lighting ambient="0.35" diffuse="0.9" />')
    lines.append("\t</Sunlight>")
    lines.append(f'\t<MeshLibrary file="{scene_name}.obj" />')
    for name, mesh_name, material, _ in groups:
        texture = group_textures[material]
        lines.append(f'\t<GameObject name="{name}" mesh="{mesh_name}"'
                     f' texture="{texture}">')
        lines.append('\t\t<Position x="0.0" y="0.0" z="0.0" />')
        lines.append('\t\t<Rotation x="0.0" y="0.0" z="0.0" />')
        lines.append('\t\t<Scale x="1.0" y="1.0" z="1.0" />')
        lines.append("\t</GameObject>")
    lines.append("</Scene>")
    path.write_text("\n".join(lines) + "\n", newline="\n")


# --------------------------------------------------------------------------
# Validation
# --------------------------------------------------------------------------

def validate_output(scene_dir, scene_name, report):
    import xml.etree.ElementTree as ET
    xml_path = scene_dir / f"{scene_name}.xml"
    root = ET.parse(xml_path).getroot()
    assert root.tag == "Scene"
    library = root.find("MeshLibrary")
    obj_path = scene_dir / library.get("file")
    if not obj_path.exists():
        report.warn(f"VALIDATION: mesh library {obj_path.name} missing")
        return False

    group_names = set()
    counts = {"v": 0, "vt": 0, "vn": 0, "f": 0}
    with obj_path.open("r") as handle:
        for line in handle:
            if line.startswith("g "):
                group_names.add(line[2:].strip())
            elif line[:2] in ("v ", "f "):
                counts[line[0]] += 1
            elif line.startswith("vt "):
                counts["vt"] += 1
            elif line.startswith("vn "):
                counts["vn"] += 1

    ok = True
    for game_object in root.findall("GameObject"):
        mesh = game_object.get("mesh")
        texture = game_object.get("texture")
        if mesh not in group_names:
            report.warn(f"VALIDATION: mesh '{mesh}' not found in OBJ")
            ok = False
        if len(mesh) > MESH_NAME_LIMIT:
            report.warn(f"VALIDATION: mesh name '{mesh}' exceeds "
                        f"{MESH_NAME_LIMIT} chars - strcpy_s in Mesh.cpp"
                        " will abort")
            ok = False
        if not (scene_dir / texture).exists():
            report.warn(f"VALIDATION: texture '{texture}' missing")
            ok = False
        for tag in ("Position", "Rotation", "Scale"):
            if game_object.find(tag) is None:
                report.warn(f"VALIDATION: GameObject '{game_object.get('name')}'"
                            f" missing <{tag}>")
                ok = False
    camera = root.find("Camera")
    if camera is None or camera.find("Position") is None \
            or camera.find("Rotation") is None:
        report.warn("VALIDATION: camera incomplete")
        ok = False
    if counts["vt"] == 0 or counts["vn"] == 0:
        report.warn("VALIDATION: missing texcoords or normals in output OBJ")
        ok = False
    print(f"  validated: {len(root.findall('GameObject'))} game objects, "
          f"{len(group_names)} obj groups, {counts['v']} verts, "
          f"{counts['f']} tris")
    return ok


# --------------------------------------------------------------------------
# Main
# --------------------------------------------------------------------------

def find_input_obj(input_path, workdir, report):
    """Return (obj_path, source_dir). Extracts zips into workdir."""
    if input_path.suffix.lower() == ".zip":
        with zipfile.ZipFile(input_path) as archive:
            archive.extractall(workdir)
        candidates = sorted(workdir.rglob("*.obj"),
                            key=lambda p: p.stat().st_size, reverse=True)
        if not candidates:
            sys.exit(f"error: no .obj found inside {input_path}")
        if len(candidates) > 1:
            # prefer an OBJ named after the archive itself (e.g. mitsuba.zip
            # -> mitsuba.obj) over incidental variants/probes bundled
            # alongside it; only fall back to "largest" if none match.
            archive_stem = input_path.stem.lower()
            named_match = next(
                (c for c in candidates if c.stem.lower() == archive_stem),
                None)
            chosen = named_match or candidates[0]
            others = ", ".join(c.name for c in candidates if c != chosen)
            report.note(f"Multiple OBJ files in zip, using '{chosen.name}' "
                        f"(ignored: {others})")
        else:
            chosen = candidates[0]
        return chosen, chosen.parent
    if input_path.suffix.lower() == ".obj":
        return input_path, input_path.parent
    sys.exit(f"error: input must be a .zip or .obj, got {input_path.name}")


def main():
    parser = argparse.ArgumentParser(
        description="Import an OBJ model as a SuperEZ scene")
    parser.add_argument("input", type=Path, help=".zip or .obj file")
    parser.add_argument("--name", required=True,
                        help="scene name (folder + xml + obj name)")
    parser.add_argument("--out-root", type=Path,
                        default=Path(__file__).resolve().parent.parent
                        / "bin" / "Editor" / "assets",
                        help="assets root (default: bin/Editor/assets)")
    parser.add_argument("--up", choices=sorted(AXIS_TRANSFORMS), default="y",
                        help="source up-axis; converted to engine Y-up"
                             " (default: y = no change)")
    parser.add_argument("--scale", type=float, default=1.0,
                        help="uniform scale factor (default 1.0 = preserve)")
    parser.add_argument("--texture-size", type=int, default=8,
                        help="size of baked solid-color textures")
    parser.add_argument("--force", action="store_true",
                        help="overwrite an existing scene folder")
    args = parser.parse_args()

    scene_name = sanitize(args.name)
    scene_dir = args.out_root / scene_name
    if scene_dir.exists() and any(scene_dir.iterdir()) and not args.force:
        sys.exit(f"error: {scene_dir} already exists, use --force to overwrite")

    report = Report()
    with tempfile.TemporaryDirectory(prefix="import_scene_") as tmp:
        obj_path, source_dir = find_input_obj(
            args.input.resolve(), Path(tmp), report)

        print(f"parsing {obj_path.name} "
              f"({obj_path.stat().st_size / 1e6:.1f} MB)...")
        positions, texcoords, normals, objects, mtl_files = \
            parse_obj(obj_path, report)
        if not objects:
            sys.exit("error: no renderable geometry found")

        materials = {}
        for mtl_name in mtl_files:
            mtl_path = source_dir / mtl_name
            if mtl_path.exists():
                materials.update(parse_mtl(mtl_path, report))
            else:
                report.warn(f"MTL file not found: {mtl_name}")

        # axis conversion + scale
        transform = AXIS_TRANSFORMS[args.up]
        if args.up != "y" or args.scale != 1.0:
            s = args.scale
            positions = [tuple(c * s for c in transform(p)) for p in positions]
            normals = [transform(n) for n in normals]

        groups = build_groups(objects, report)

        # ensure every corner has vt and vn
        missing_uv_groups = 0
        default_uv_index = None
        for _, mesh_name, _, triangles in groups:
            group_had_missing_uv = False
            for f_index, tri in enumerate(triangles):
                fixed = list(tri)
                for c_index, (vi, ti, ni) in enumerate(fixed):
                    if ti is None:
                        if default_uv_index is None:
                            texcoords.append("0.0 0.0")
                            default_uv_index = len(texcoords)
                        ti = default_uv_index
                        group_had_missing_uv = True
                    if ni is None:
                        a = positions[fixed[0][0] - 1]
                        b = positions[fixed[1][0] - 1]
                        c = positions[fixed[2][0] - 1]
                        normals.append(face_normal(a, b, c))
                        ni = len(normals)
                    fixed[c_index] = (vi, ti, ni)
                triangles[f_index] = tuple(fixed)
            if group_had_missing_uv:
                missing_uv_groups += 1
        if missing_uv_groups:
            report.warn(f"{missing_uv_groups} group(s) had no UVs -"
                        " dummy (0,0) coordinates generated")

        # bounds and camera
        used = {vi for _, _, _, tris in groups for tri in tris for vi, _, _ in tri}
        mins, maxs = compute_bounds(positions, used)
        camera = camera_from_bounds(mins, maxs)
        _, _, _, distance, radius = camera
        ideal = radius / math.tan(math.radians(FOV_DEGREES) * 0.5) * 1.1
        if ideal + radius > FAR_PLANE:
            suggested = FAR_PLANE * 0.9 / (ideal + radius)
            report.warn(
                f"Scene is large ({radius:.0f} unit radius) for the engine's "
                f"farPlane={FAR_PLANE:.0f} - camera pulled in closer than "
                f"ideal; consider --scale {suggested:.3f}")

        # textures: one BMP per material actually used
        scene_dir.mkdir(parents=True, exist_ok=True)
        group_textures = {}
        baked = converted = 0
        for _, _, material, _ in groups:
            if material in group_textures:
                continue
            if material is None:
                bake_checker_bmp(scene_dir / "missing.bmp")
                group_textures[None] = "missing.bmp"
                report.warn("Some objects have no material - using missing.bmp")
                continue
            info = materials.get(material)
            filename = f"mat_{sanitize(material)}.bmp"
            if info is None:
                report.warn(f"Material '{material}' not in MTL - baked gray")
                bake_solid_bmp(scene_dir / filename, (0.8, 0.8, 0.8),
                               args.texture_size)
                baked += 1
            elif info["map_kd"]:
                source = source_dir / info["map_kd"]
                if source.exists() and convert_texture(
                        source, scene_dir / filename, report):
                    converted += 1
                else:
                    if not source.exists():
                        report.warn(f"map_Kd not found: {info['map_kd']} -"
                                    f" baked solid color instead")
                    bake_solid_bmp(scene_dir / filename, info["kd"],
                                   args.texture_size)
                    baked += 1
            else:
                bake_solid_bmp(scene_dir / filename, info["kd"],
                               args.texture_size)
                baked += 1
            group_textures[material] = filename

        # write outputs
        write_obj(scene_dir / f"{scene_name}.obj", scene_name,
                  positions, texcoords, normals, groups)
        write_mtl(scene_dir / f"{scene_name}.mtl",
                  [m for m in group_textures if m], materials)
        write_scene_xml(scene_dir / f"{scene_name}.xml", scene_name,
                        groups, group_textures, camera)

    # summary
    triangle_total = sum(len(t) for _, _, _, t in groups)
    print(f"\nscene '{scene_name}' written to {scene_dir}")
    print(f"  {len(groups)} mesh groups, {triangle_total} triangles, "
          f"{len(positions)} vertices")
    print(f"  {baked} solid-color textures baked, {converted} converted")
    print(f"  bounds: x[{mins[0]:.1f}, {maxs[0]:.1f}] "
          f"y[{mins[1]:.1f}, {maxs[1]:.1f}] z[{mins[2]:.1f}, {maxs[2]:.1f}]")
    position, pitch, yaw, _, _ = camera
    print(f"  camera: pos=({position[0]:.1f}, {position[1]:.1f}, "
          f"{position[2]:.1f}) pitch={pitch:.2f} yaw={yaw:.2f}")

    ok = validate_output(scene_dir, scene_name, report)
    for note in report.notes:
        print(f"  note: {note}")
    for warning in report.warnings:
        print(f"  WARNING: {warning}")
    if not ok:
        sys.exit("validation FAILED - scene may not load")
    print("validation OK")


if __name__ == "__main__":
    main()
