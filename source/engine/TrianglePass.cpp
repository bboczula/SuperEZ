#include "TrianglePass.h"

void TrianglePass::Execute()
{
}

void TrianglePass::Allocate()
{
	// Create a Render Target and store reference to it
	// This could be problematic, since actually Render Pass keeps pointers to Render Targets
	// I was rather thinking about storing Render Targets in a separate container in RenderContext
	// and use some kind of ID to reference them
	// Why would I do that? Why is keepin index better than keeping a pointer?
	// Because I can easily iterate over all Render Targets and do some operations on them
	// I can also easily remove them from the container
	// I can also easily add them to the container
	// I can also easily check if a Render Target is in the container
	// I can also easily get a Render Target by its ID
	// I can also easily get a Render Target by its index
	// I can also easily get a Render Target by its name
	// Maybe I should use a map instead of a vector?
	// Maybe I should just leave all this stuff in Render Context? Let's just make this our policy for now
	// And let's just keep Render Passes as simple as possible
}
