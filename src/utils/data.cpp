/* Copyright (C) 2025-2026 Mykyta Polishyk */
/* This project is licensed under the GNU General Public License v3.0 or later. */
/* See the LICENSE file for details. */
#include "utils/data.hpp"
#include <png.h>

DataSystem *Data; // Global object initialization

// Load functions
GraphicsTexture DataSystem::LoadTexture(const string &path)
{
    GraphicsTexture Texture = {};

    // Reading PNG
    ifstream file(path, ios::binary);
    if (!file) {
        Console.WriteDebug("GFX", "Failed to open " + path + " image");
        return Texture;
    }
    vector<unsigned char> file_data((istreambuf_iterator<char>(file)),
                                         istreambuf_iterator<char>());
    file.close();

    // Check PNG
    if (file_data.size() < 8 || png_sig_cmp(file_data.data(), 0, 8)) {
        Console.WriteDebug("GFX", "File " + path + " is not a PNG");
        return Texture;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) return Texture;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) { png_destroy_read_struct(&png_ptr, nullptr, nullptr); return Texture; }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        Console.WriteDebug("GFX", "Error reading PNG " + path);
        return Texture;
    }

    struct MemReader { const unsigned char* data; size_t size; size_t offset; };
    MemReader reader = { file_data.data() + 8, file_data.size() - 8, 0 }; // Signature check
    png_set_sig_bytes(png_ptr, 8);
    // Reading PNG
    auto png_read_from_memory = [](png_structp png_ptr, png_bytep out_bytes, png_size_t byte_count_to_read) {
        MemReader* r = (MemReader*)png_get_io_ptr(png_ptr);
        if (r->offset + byte_count_to_read > r->size)
            png_error(png_ptr, "Read beyond end of buffer");
        memcpy(out_bytes, r->data + r->offset, byte_count_to_read);
        r->offset += byte_count_to_read;
    };

    png_set_read_fn(png_ptr, &reader, png_read_from_memory);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    // Setting image paramgs
    Texture.Width  = png_get_image_width(png_ptr, info_ptr);
    Texture.Height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth  = png_get_bit_depth(png_ptr, info_ptr);

    if (bit_depth == 16) png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY) png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    vector<png_byte> image_data(Texture.Width * Texture.Height * 4);
    vector<png_bytep> row_pointers(Texture.Height);
    for (int y = 0; y < Texture.Height; y++)
        row_pointers[y] = image_data.data() + y * Texture.Width * 4;

    png_read_image(png_ptr, row_pointers.data());
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    // Creating GL texture
    glGenTextures(1, &Texture.Raw);
    glBindTexture(GL_TEXTURE_2D, Texture.Raw);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture.Width, Texture.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data.data());

    Console.WriteDebug("GFX", "Loaded " + path + " image");
    return Texture;
}

SoundData DataSystem::LoadSound(string Path){
    // SO BIG THANKS FOR codetechandtutorials github user!
    ALenum err, format;
    ALuint buffer;
    SoundData SoundRetBuffer;
    SNDFILE* sndfile;
    SF_INFO sfinfo;
    short* membuf;
    sf_count_t num_frames;
    ALsizei num_bytes;

    /* Open the audio file and check that it's usable. */
    sndfile = sf_open(Path.c_str(), SFM_READ, &sfinfo);
    if (!sndfile)
    {
        Console.WriteDebug("AUDIO", "Could not open audio in " + Path);
    }
    if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
    {
        Console.WriteDebug("AUDIO", "Bad sample count");
        sf_close(sndfile);
    }

    /* Get the sound format, and figure out the OpenAL format */
    format = AL_NONE;
    if (sfinfo.channels == 1)
        format = AL_FORMAT_MONO16;
    else if (sfinfo.channels == 2)
        format = AL_FORMAT_STEREO16;
    else if (sfinfo.channels == 3)
    {
        if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
            format = AL_FORMAT_BFORMAT2D_16;
    }
    else if (sfinfo.channels == 4)
    {
        if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
            format = AL_FORMAT_BFORMAT3D_16;
    }
    if (!format)
    {
        Console.WriteDebug("AUDIO", "Unsupported channel count");
        sf_close(sndfile);
    }

    /* Decode the whole audio file to a buffer. */
    membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

    num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
    if (num_frames < 1)
    {
        free(membuf);
        sf_close(sndfile);
        Console.WriteDebug("AUDIO", "Failed to read samples");
    }
    num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

    /* Buffer the audio data into a new buffer object, then free the data and
     * close the file.
     */
    buffer = 0;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

    free(membuf);
    sf_close(sndfile);

    /* Check if an error occured, and clean up if so. */
    err = alGetError();
    if (err != AL_NO_ERROR)
    {
        Console.WriteDebug("AUDIO", "OpenAL Error: " + string(alGetString(err)));
        if (buffer && alIsBuffer(buffer))
            alDeleteBuffers(1, &buffer);
    }

    SoundRetBuffer.Path = Path;
    SoundRetBuffer.Raw = buffer;  // add to the list of known buffers

    return SoundRetBuffer;
}

// Constructor
DataSystem::DataSystem(){
    // Cleaning vectors
    TexturesPaths.clear();
    Textures.clear();
    SoundPaths.clear();
    Sounds.clear();
}

// Get functions
GraphicsTexture DataSystem::GetTextureByID(int ID){
    return Textures[ID];
}
SoundData DataSystem::GetSoundByID(int ID){
    return Sounds[ID];
}

// Push functions
void DataSystem::PushTexturePath(string Path){
    TexturesPaths.push_back(Path);
}
void DataSystem::PushSoundPath(string Path){
    SoundPaths.push_back(Path);
}

void DataSystem::LoadEverything(){
    LoadTextures();
    LoadSounds();
}

// Load functions
void DataSystem::LoadTextures(){
    // Simple null texture
    // TODO: Add load_null_texture function
    GLuint NullTexRaw;
    glGenTextures(1, &NullTexRaw);
    glBindTexture(GL_TEXTURE_2D, NullTexRaw);

    // Drawing 1 white pixel
    unsigned char pixel[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GraphicsTexture NullTex;
    NullTex.Raw = NullTexRaw;
    NullTex.Width = 1;
    NullTex.Height = 1;
    Textures.push_back(NullTex); // Pushing to vector null texture
    for (int i = 0; i<TexturesPaths.size(); i++){
        Textures.push_back(LoadTexture("assets/" + TexturesPaths[i])); // Loading another stuff
    }
}
void DataSystem::LoadSounds(){
    for (int i = 0; i<SoundPaths.size(); i++){
        Sounds.push_back(LoadSound("sounds/" + SoundPaths[i])); // Pushing audio datas to vector
    }
}