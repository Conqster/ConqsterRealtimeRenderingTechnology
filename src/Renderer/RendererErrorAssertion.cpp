#pragma once
#include "RendererErrorAssertion.h"
#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR)
	{
		GLenum ErrorCheckValue = glGetError();
		std::cout << "stuck in a while loop Renderer.cpp: " << glewGetErrorString(ErrorCheckValue) << "\n";
	}
}

bool GLLogCall(const char* func, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] -> " << error <<
			",  " << func << " " << file << ":" << line << "\n";
		return false;
	}
	return true;
}


