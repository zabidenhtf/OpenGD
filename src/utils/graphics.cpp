/* Copyright (C) 2025-2026 Mykyta Polishyk */
/* This project is licensed under the GNU General Public License v3.0 or later. */
/* See the LICENSE file for details. */
#include "utils/graphics.hpp"
#include "utils/data.hpp"
#include "utils/config.hpp"

GraphicsSystem *Graphics; // Global graphics object

GraphicsSystem::GraphicsSystem(){
	Root = nullptr;
	// Setting params
	Width = stoi(Config->LoadData("GFX", "screen_width", "800"));
	Height = stoi(Config->LoadData("GFX", "screen_height", "800"));
	string FullscreenBuffer = Config->LoadData("GFX", "fullscreen", "False");
	if (FullscreenBuffer == "True"){ // String -> Bool
   		Fullscreen = true;
    }
    else{
        Fullscreen = false;
    }

    CreateWindowAndContext();

    SetBlendNormal();
    // Creating models
    QuadModel = LoadQuadModel();
}

void GraphicsSystem::CreateWindowAndContext(){
    // GLFW initialization
	if (glfwInit()){
        Console.WriteDebug("GFX", "GLFW initialisated");
    }

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	// Creating window
	switch (Fullscreen){
		case true:
			Root = glfwCreateWindow(Width, Height, "OpenSourceGD", glfwGetPrimaryMonitor(), NULL);
			break;
		case false:
			Root = glfwCreateWindow(Width, Height, "OpenSourceGD", NULL, NULL);
			break;
	}
	if (!Root){
		Console.WriteDebug("GFX", "Failed to create window");
	}
    glfwMakeContextCurrent(Root);
    // GLAD initialization
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Console.WriteDebug("GFX", "GLAD initialisated");
    } 
    else {
        Console.WriteDebug("GFX", "Failed to initialize GLAD");
        return;
    }

    error = FT_Init_FreeType(&library);

    // Freetype initialization
    if (!error){
        Console.WriteDebug("GFX", "Freetype intialisated");
    }
    else{
        Console.WriteDebug("GFX", "Failed to initialisate Freetype");
    }

    // Creating shaders
    shader2D = glCreateProgram();

   	// 2D stuff
    // Reading raw shaders
    string Vertex2DShaderSource = ReadFile("shaders/shader2D.vert");
    string Fragment2DShaderSource = ReadFile("shaders/shader2D.frag");

    GLuint Vertex2DShader = glCreateShader(GL_VERTEX_SHADER);
    const char* Vertex2DShaderSource_cstring = Vertex2DShaderSource.c_str();
    glShaderSource(Vertex2DShader, 1, &Vertex2DShaderSource_cstring, nullptr);
    glCompileShader(Vertex2DShader); // Vertex 2D shader

    GLuint Fragment2DShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* Fragment2DShaderSource_cstring = Fragment2DShaderSource.c_str();
    glShaderSource(Fragment2DShader, 1, &Fragment2DShaderSource_cstring, nullptr);
    glCompileShader(Fragment2DShader); // Fragment 2D shader

    // Attaching shaders
    glAttachShader(shader2D, Vertex2DShader);
    glAttachShader(shader2D, Fragment2DShader);
    glLinkProgram(shader2D);

    glDisable(GL_DEPTH_TEST); // Disabling depth test because 2D gfx
}

bool GraphicsSystem::ShouldWindowClose(){
	return glfwWindowShouldClose(Root);
}

void GraphicsSystem::SetOrtho(int w, int h){
	// Setting in shader ortho
	glUseProgram(shader2D);
    mat4 proj2D = ortho<float>(0, w, h, 0);
    mat4 view2D = mat4(1.0f);
    // Setting projection mode and view in shaders
    glUniformMatrix4fv(glGetUniformLocation(shader2D, "projection"),
                   1, GL_FALSE, value_ptr(proj2D));
    glUniformMatrix4fv(glGetUniformLocation(shader2D, "view"),
                   1, GL_FALSE, value_ptr(view2D));
}

void GraphicsSystem::SetViewport(int w, int h){
	glViewport(0, 0, w, h);
}

void GraphicsSystem::LoadFont(string path, int id){
    // Creating face
    error = FT_New_Face(library, path.c_str(), 0, &face);
    
    // If error returning
    if (!error){
        Console.WriteDebug("GFX", "Loaded "+path+" font");
    }
    else{
        Console.WriteDebug("GFX", "Failed to load font");
        return;
    }
}

void GraphicsSystem::PollEvents(){
	glfwPollEvents();
    glfwSwapBuffers(Root);
}

void GraphicsSystem::ClearScreen(vec3 Color){
	// Just cleaning with color
	glClearColor(Color.x, Color.y, Color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsSystem::EnableTexture(GraphicsTexture texture){
    glUseProgram(shader2D);
    glActiveTexture(GL_TEXTURE0);
    // Binding texture from argument
    glBindTexture(GL_TEXTURE_2D, texture.Raw);
    glUniform1i(glGetUniformLocation(shader2D, "tex"), 0);
}
void GraphicsSystem::DisableTexture(){
	glUseProgram(shader2D);
    glActiveTexture(GL_TEXTURE0);
    // Binding NOTHING
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(shader2D, "tex"), 0);
}

float GraphicsSystem::GetScreenAspect(){
    return static_cast<float>(Width) / static_cast<float>(Height);
}

void GraphicsSystem::SetBlendNormal(){
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void GraphicsSystem::SetBlendAdditive(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

GraphicsModel GraphicsSystem::LoadQuadModel(){
	GraphicsModel Buffer; // Buffer to return

	// Vertices of plane
    vertex2D QuadVertices[4] = {
	    {{0.0f, 0.0f}, {0.0f, 0.0f}},
	    {{1.0f, 0.0f}, {1.0f, 0.0f}},
	    {{1.0f, 1.0f}, {1.0f, 1.0f}},
	    {{0.0f, 1.0f}, {0.0f, 1.0f}}
    };
    GLuint quadIndices[6] = {0, 1, 2, 2, 3, 0};

    // Binding buffers
    glGenVertexArrays(1, &Buffer.VAO);
    glGenBuffers(1, &Buffer.VBO);
    glGenBuffers(1, &Buffer.EBO);

    glBindVertexArray(Buffer.VAO);

    glBindVertexArray(Buffer.VAO);

    // Setting up data into VBO and EBO
    glBindBuffer(GL_ARRAY_BUFFER, Buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex2D), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex2D), (void*)offsetof(vertex2D, TexturePos));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbinding arrays and buffers

    return Buffer;
}

void GraphicsSystem::DrawQuad(vec2 Position, vec2 Size, vec4 Color){
    glUseProgram(shader2D);

    glUniform4f(glGetUniformLocation(shader2D, "uColor"),
                Color.x, Color.y, Color.z, Color.w);

    mat4 model = translate(mat4(1.0f), vec3(Position, 0.0f));
    // Scaling
    model = scale(model, vec3(Size, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader2D, "model"), 1, GL_FALSE, value_ptr(model));

    // Rendering
    glBindVertexArray(QuadModel.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GraphicsSystem::DrawText(vec2 Position, int TextSize, int TextResolution, string Text, vec4 Color){
	for (int i = 0; i<Text.length(); i++){ // Drawing each char of string
        FT_Set_Pixel_Sizes(face, 0, TextResolution);
        // rendering char
        FT_Load_Char(face, Text[i], FT_LOAD_RENDER);
        FT_GlyphSlot g = face->glyph;
        // Generating texture
        GraphicsTexture CharTexture;
        glGenTextures(1, &CharTexture.Raw);
        glBindTexture(GL_TEXTURE_2D, CharTexture.Raw);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            g->bitmap.width,
            g->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            g->bitmap.buffer
        );
        // Texture params 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Finally render (with size)
        FT_Set_Pixel_Sizes(face, 0, TextSize);
        FT_Load_Char(face, Text[i], FT_LOAD_RENDER);
        g = face->glyph; // Updating glyph to get correct sizes

        int x = g->bitmap_left;
        int y = g->bitmap_top;
        int w = g->bitmap.width;
        int h = g->bitmap.rows;

        EnableTexture(CharTexture);
        DrawQuad(Position + vec2(x+i*TextSize/1.5,-y+TextSize), vec2(w,h), Color);
        DisableTexture();
    }
}

int GraphicsSystem::GetTextWidth(int TextSize, string Text){
    return Text.length() * TextSize/1.5; // TODO: Remove magic numbers
}

void GraphicsSystem::Kill(){
	// For first destroying window, after terminating GLFW
	glfwDestroyWindow(Root);
	glfwTerminate();
}