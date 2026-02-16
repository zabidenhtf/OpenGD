/* Copyright (C) 2025-2026 Mykyta Polishyk */
/* This project is licensed under the GNU General Public License v3.0 or later. */
/* See the LICENSE file for details. */
#include "utils/system.hpp"
#include "utils/data.hpp"

#include "includes.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#pragma once

using namespace glm;

class GraphicsSystem{
public:
	GLFWwindow *GetWindow(){return Root;};
	float GetTime(){return glfwGetTime();};
	GraphicsSystem();
	void CreateWindowAndContext(); // Creating OpenGL context
	bool ShouldWindowClose(); // Interface for glfwShouldClose()
	void SetOrtho(int w, int h); // Set Ortho
	void SetViewport(int w, int h); // Set Viewport
	void LoadFont(string path, int id); // Loading freetype font
	void PollEvents(); // Updating window and polling events
	void ClearScreen(vec3 Color); // Clear screen with Color
	void EnableTexture(GraphicsTexture texture); // Enable texture
	void DisableTexture(); // Disable texture
	float GetScreenAspect(); // Get Screen aspect in float
	void SetBlendNormal(); // Set blending type on normal
	void SetBlendAdditive(); // Set blending type on additive
	GraphicsModel LoadQuadModel(); // Loading simple 2D quad model
	void DrawQuad(vec2 Position, vec2 Size, vec4 Color); // Draw 2D quad
	void DrawText(vec2 Position, int TextSize, int TextResolution, string Text, vec4 Color); // Draw freetype text
	int GetTextWidth(int TextSize, string Text); // Getting text width in pixels
	int GetWidth(){return Width;};
	int GetHeight(){return Height;};
	void Kill();
private:
	GLuint shader2D; // 2D stuff

	GLFWwindow *Root; // Main Window
	// Important models
	GraphicsModel QuadModel;
	int Width;
	int Height;
	bool Fullscreen;

	// FT stuff
	FT_Library    library;
	FT_Face       face;
	FT_GlyphSlot  slot;
	FT_Matrix     matrix;
	FT_Vector     pen;
	FT_Error      error;
};

extern GraphicsSystem *Graphics; // Global graphics object