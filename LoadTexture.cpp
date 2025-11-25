#include "stb_image.h"
#include "LoadTexture.hpp"
#include <iostream>

Texture::Texture() : id(0), width(0), height(0), channels(0) {}

Texture::~Texture() {
    if (id != 0) glDeleteTextures(1, &id);
}
GLuint Texture::ReadTextureFromFile(const char* file_name) {

    int x, y, n;
    int force_channels = 4;
    unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);

    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", file_name);
        return false;
    }
    // NPOT check
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf(
            stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
        );
    }

    int width_in_bytes = x * 4;
    unsigned char* top = NULL;
    unsigned char* bottom = NULL;
    unsigned char temp = 0;
    int half_height = y / 2;

    for (int row = 0; row < half_height; row++) {

        top = image_data + row * width_in_bytes;
        bottom = image_data + (y - row - 1) * width_in_bytes;

        for (int col = 0; col < width_in_bytes; col++) {

            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top++;
            bottom++;
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_SRGB, //GL_SRGB,//GL_RGBA,
        x,
        y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}
GLuint Texture::ReadTextureFromFileRoughness(const char* file_name) {

    stbi_set_flip_vertically_on_load(true);

    int x, y, n;
    unsigned char* image_data = stbi_load(file_name, &x, &y, &n, 0);

    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", file_name);
        return 0;
    }

    // Verificare NPOT (optional)
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf(stderr,
            "WARNING: texture %s is not power-of-2 dimensions\n",
            file_name
        );
    }

    // Roughness = 1 canal
    GLint internalFormat = GL_R8;  // un singur canal pe 8 biti
    GLenum format = GL_RED;        // image is R only

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        x,
        y,
        0,
        format,
        GL_UNSIGNED_BYTE,
        image_data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image_data);

    return textureID;
}


bool Texture::load(const std::string& path, bool flip)
{
    GLuint textureID = ReadTextureFromFile(path.c_str());
    if (textureID == 0) {
        std::cout << "Failed to load texture: " << path << "\n";
        return false;
    }
    id = textureID;
    return true;
}

void Texture::bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}