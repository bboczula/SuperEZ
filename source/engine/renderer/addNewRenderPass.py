import os
import sys
import textwrap
import re

# --- CONFIGURATION ---
TARGET_DIRECTORY = "./passes" 
SHADER_DIRECTORY = "./shaders"
MAX_LINE_LENGTH  = 100 
# ---------------------

def to_snake_case(name):
    # Inserts an underscore before capital letters (unless it's the start)
    # AlphaBetaGamma -> alpha_beta_gamma
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def get_compute_shader_content():
    return """[numthreads(8, 8, 1)]
void CSMain(uint3 dtid : SV_DispatchThreadID)
{
}
"""

def get_graphics_shader_content():
    return """struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(float4 position : POSITION)
{
    PSInput result;
    result.position = position;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}
"""

def format_description(description):
    if not description:
        return "// (No description provided)"
    wrapper = textwrap.TextWrapper(width=MAX_LINE_LENGTH, initial_indent="// ", subsequent_indent="// ")
    return wrapper.fill(description)

def create_header(path, class_name, pass_type_str, description):
    formatted_desc = format_description(description)
    
    content = f"""#pragma once

#include "../RenderPass.h"

// Type: {pass_type_str}
{formatted_desc}

class Orbit;

class {class_name} : public RenderPass
{{
public:
    {class_name}();
    ~{class_name}();

    void ConfigurePipelineState() override;
    void Initialize() override;
    void Update() override;
    void Execute() override;
    void Allocate(DeviceContext* deviceContext) override;
}};
"""
    with open(path, 'w') as f:
        f.write(content)
    print(f"  -> Created Header: {path}")

def create_source(path, class_name, base_name, shader_name, pass_type_enum, pass_type_str):
    # 1. Determine Includes
    includes = f'#include "{class_name}.h"\n#include "../RenderContext.h"\n#include "../../bind/RootSignatureBuilder.h"'
    
    # Graphics needs InputLayout, Compute does not.
    if pass_type_str == "Graphics":
        includes += '\n#include "../../core/InputLayout.h"'
        
    # Add the extern declaration
    includes += "\n\nextern RenderContext renderContext;"
    
    # 2. Determine ConfigurePipelineState Body
    config_body = ""
    
    if pass_type_str == "Compute":
        config_body = """    // Now we can create the root signature
    RootSignatureBuilder builder;
    rootSignature = renderContext.CreateRootSignature(builder);"""

    else: # Graphics or Drawless
        # Construct RT name: RT_NamePass
        rt_name = f"RT_{class_name}"
        
        # Start of body
        lines = []
        
        # Only add InputLayout logic for pure Graphics
        if pass_type_str == "Graphics":
            lines.append("    // Pre-AutomaticInitialize Procedure")
            lines.append("    inputLayout = renderContext.CreateInputLayout();")
            lines.append("    renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);")
            lines.append("")

        # Common for Graphics/Drawless
        lines.append("    // Now we can create the root signature")
        lines.append("    RootSignatureBuilder builder;")
        lines.append("    rootSignature = renderContext.CreateRootSignature(builder);")
        lines.append("")
        lines.append(f'    renderTarget = renderContext.CreateRenderTarget("{rt_name}", RenderTargetFormat::RGB8_UNORM, 64, 64);')
        
        config_body = "\n".join(lines)

    # 3. Assemble File
    content = f"""{includes}

{class_name}::{class_name}() : RenderPass(L"{base_name}", L"{shader_name}", {pass_type_enum})
{{
}}

{class_name}::~{class_name}()
{{
}}

void {class_name}::ConfigurePipelineState()
{{
{config_body}
}}

void {class_name}::Initialize()
{{
}}

void {class_name}::Update()
{{
}}

void {class_name}::Execute()
{{
}}

void {class_name}::Allocate(DeviceContext* deviceContext)
{{
}}
"""
    with open(path, 'w') as f:
        f.write(content)
    print(f"  -> Created Source: {path}")

def create_shader(path, is_compute):
    if os.path.exists(path):
        print(f"  -> Shader '{path}' already exists. Skipping write (Safety first).")
        return

    content = get_compute_shader_content() if is_compute else get_graphics_shader_content()
    
    with open(path, 'w') as f:
        f.write(content)
    print(f"  -> Created Shader: {path}")

def run_generator():
    print("--- NEW RENDER PASS GENERATOR ---")
    
    # 1. Inputs
    raw_name = input("1. Enter the Render Pass name (e.g. 'Shadow' or 'AlphaBetaGamma'): ").strip()
    if not raw_name:
        print("You didn't type shit. Aborting.")
        return
    
    base_name = raw_name[0].upper() + raw_name[1:]
    final_name = f"{base_name}Pass"

    print("\nSelect Pass Type:")
    print("  [1] Graphics")
    print("  [2] Drawless")
    print("  [3] Compute")
    type_choice = input("Enter number (default 1): ").strip()
    
    # Defaults
    pass_type_str = "Graphics"
    pass_type_enum = "Type::Graphics"
    is_compute = False
    
    if type_choice == '2': 
        pass_type_str = "Drawless"
        pass_type_enum = "Type::Drawless"
    elif type_choice == '3': 
        pass_type_str = "Compute"
        pass_type_enum = "Type::Compute"
        is_compute = True

    print(f"\nCreating {pass_type_str} Pass: {final_name}")
    description = input("3. Enter a description for the header comment: ").strip()

    # Smart Shader Naming
    default_shader_name = f"{to_snake_case(base_name)}.hlsl"
    shader_input = input(f"4. Enter HLSL file name (for default press ENTER: '{default_shader_name}'): ").strip()
    
    if not shader_input:
        shader_name = default_shader_name
    else:
        shader_name = shader_input

    # 2. Paths
    header_path = os.path.join(TARGET_DIRECTORY, f"{final_name}.h")
    cpp_path = os.path.join(TARGET_DIRECTORY, f"{final_name}.cpp")
    shader_path = os.path.join(SHADER_DIRECTORY, shader_name)

    # 3. Checks
    if not os.path.exists(TARGET_DIRECTORY):
        print(f"Error: C++ Directory '{TARGET_DIRECTORY}' missing.")
        return
    if not os.path.exists(SHADER_DIRECTORY):
        print(f"Error: Shader Directory '{SHADER_DIRECTORY}' missing.")
        return

    # 4. Overwrite Prompt
    if os.path.exists(header_path) or os.path.exists(cpp_path):
        print(f"\nWARNING: Files for '{final_name}' already exist.")
        choice = input("Do you want to OVERWRITE them? (y/n): ").strip().lower()
        if choice != 'y':
            print("Aborting.")
            return

    # 5. Execution
    print("-" * 30)
    try:
        create_header(header_path, final_name, pass_type_str, description)
        create_source(cpp_path, final_name, base_name, shader_name, pass_type_enum, pass_type_str)
        create_shader(shader_path, is_compute)
        
        print("-" * 30)
        print(f"Success! {final_name} generated.")
        print("Now run the fucking Premake script.")
        
    except Exception as e:
        print(f"Something went wrong: {e}")

if __name__ == "__main__":
    run_generator()