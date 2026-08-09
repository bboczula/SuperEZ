#pragma once

// Keep the decode, lighting, and output-encoding comparison available while
// the renderer's color handling is being developed. Mip generation is always
// performed in linear light. Set this to 0 for the shipping path.
#ifndef ENABLE_COLOR_PIPELINE_DEBUG
#define ENABLE_COLOR_PIPELINE_DEBUG 1
#endif
