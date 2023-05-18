#pragma once
#ifndef OPENGL_UTIL
#define OPENGL_UTIL
#include <GL/glew.h>
#include <vector>

namespace bf {
	namespace gl {
		template<typename T>
		void namedBufferData(GLuint buffer, const std::vector<T>& data, bool isDynamic) {
			GLenum usage = isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
            if(GLEW_ARB_direct_state_access)
			    glNamedBufferData(buffer, data.size() * sizeof(T), data.data(), usage);
            else
			    glNamedBufferDataEXT(buffer, data.size() * sizeof(T), data.data(), usage);
		}
		template<typename T>
		void namedBufferSubData(GLuint buffer, const std::vector<T>& data, unsigned beginIndex, unsigned size) {
            if(GLEW_ARB_direct_state_access)
			    glNamedBufferSubData(buffer, beginIndex * sizeof(T), size * sizeof(T), &(data[beginIndex]));
            else
			    glNamedBufferSubDataEXT(buffer, beginIndex * sizeof(T), size * sizeof(T), &(data[beginIndex]));
		}
		void namedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
	}
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void*);
#endif //OPENGL_UTIL