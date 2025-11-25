#pragma once
#include <GL/glew.h>
enum class BufferUsage {
	STATIC_DRAW,
	DYNAMIC_DRAW,
	STREAM_DRAW
};
inline GLenum ConvertUsage(BufferUsage usage) {
    switch (usage) {
    case BufferUsage::STATIC_DRAW: return GL_STATIC_DRAW;
    case BufferUsage::DYNAMIC_DRAW: return GL_DYNAMIC_DRAW;
    case BufferUsage::STREAM_DRAW: return GL_STREAM_DRAW;
    }
    return GL_STATIC_DRAW; // fallback
}