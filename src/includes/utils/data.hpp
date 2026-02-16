/* Copyright (C) 2025-2026 Mykyta Polishyk */
/* This project is licensed under the GNU General Public License v3.0 or later. */
/* See the LICENSE file for details. */
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <inttypes.h>
#include <AL/alext.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "includes.hpp"
#include "utils/system.hpp"

#pragma once

using namespace glm;

struct vertex2D
{
    vec2 Position;
    vec2 TexturePos;
};

struct vertex3D
{
    vec3 Position;
    vec2 TexturePos;
    vec3 Normal;
};

struct GraphicsTexture{ // 2D texture structure
    GLuint Raw;
    int Width;
    int Height;
};

struct GraphicsModel{ // model structure with draw stuff
    GLuint VAO, VBO, EBO;
};

struct SoundData{ // Sound data with raw audio data and path
    string Path;
    ALuint Raw;
    int BPM; // Beats per min
};

class DataSystem{
public:
    // Load functions
    GraphicsModel LoadModel(const string path); // On some time public, TODO: Add LoadModels function 
    DataSystem();
    // Get functions
    GraphicsTexture GetTextureByID(int ID);
    SoundData GetSoundByID(int ID);
    // Push functions
    void PushTexturePath(string Path);
    void PushSoundPath(string Path);
    // Load functions
    void LoadEverything();
    void LoadTextures();
    void LoadSounds();
private:
    // Load functions
    SoundData LoadSound(string Path); // Loading raw sound using AL
    GraphicsTexture LoadTexture(const string &path); // Loading PNG image using libpng
    // Data
    vector<string> TexturesPaths;
    vector<GraphicsTexture> Textures;
    vector<string> SoundPaths;
    vector<SoundData> Sounds;
    // Assimp importer
    Assimp::Importer importer;
};

extern DataSystem *Data; // Global data pointer

enum{ // Textures
    NULL_TEX
};

// Abonded
/*enum{ // Fonts
    EUROSTILE_ROMAN
};

enum{ // Sounds
    INGAME_MELODY1
};
*/