#pragma once

#include <cstddef>
#include <cassert>

template<typename T>
class Handle {
public:
	using IndexType = size_t;

	Handle() : m_index(InvalidIndex()) {}
	explicit Handle(IndexType index) : m_index(index) {}

	bool IsValid() const { return m_index != InvalidIndex(); }
	IndexType Index() const { return m_index; }

	bool operator==(const Handle& other) const { return m_index == other.m_index; }
	bool operator!=(const Handle& other) const { return !(*this == other); }

	static constexpr IndexType InvalidIndex() { return static_cast<IndexType>(-1); }

private:
	IndexType m_index;
};

// Type aliases for clarity
class Texture;
class VertexBuffer;
class RenderTarget;
class DepthBuffer;
class CommandList;
class Mesh;

using TextureHandle = Handle<Texture>;
using VertexBufferHandle = Handle<VertexBuffer>;
using HRenderTarget = Handle<RenderTarget>;
using HDepthBuffer = Handle<DepthBuffer>;
using HCommandList = Handle<CommandList>;
using HMesh = Handle<Mesh>;
