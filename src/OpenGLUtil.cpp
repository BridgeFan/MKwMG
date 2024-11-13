#include "OpenGLUtil.h"
#include <string>
#include <iostream>

void bf::gl::namedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height) {
if(GLEW_ARB_direct_state_access)
	glNamedRenderbufferStorage(renderbuffer, internalformat, width, height);
else
	glNamedRenderbufferStorageEXT(renderbuffer, internalformat, width, height);
}

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void*) {
	std::string sourceStr;
	switch (source) {
	case GL_DEBUG_SOURCE_API:				sourceStr = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		sourceStr = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:	sourceStr = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:		sourceStr = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:		sourceStr = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: default:	sourceStr = "Other"; break;
	}
	std::string typeStr;
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:				typeStr = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	typeStr = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	typeStr = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:			typeStr = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:			typeStr = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER:				typeStr = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:			typeStr = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:			typeStr = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: default:		typeStr = "Other"; break;
	}
	std::string severityStr;
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:		severityStr = "HIGH"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:		severityStr = "MEDIUM"; break;
	case GL_DEBUG_SEVERITY_LOW:			severityStr = "LOW"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:return; //should not be shown
	default:							severityStr = "?????";
	}
	std::cerr << "GL CALLBACK: id=" << id << ", source = "<<sourceStr << ", type = " << typeStr <<
        ", severity = " << severityStr << ", messsage = " << message << "\n";
}