#pragma once

// Keep the comparison path available while the renderer's color handling is
// being developed. Set this to 0 for builds that should expose only the
// correct linear-light path.
#ifndef ENABLE_COLOR_PIPELINE_DEBUG
#define ENABLE_COLOR_PIPELINE_DEBUG 1
#endif
