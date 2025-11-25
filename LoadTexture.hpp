#pragma once
#include <string>
#include <GL/glew.h>
class Texture {
public:
    GLuint id;
    int width, height, channels;

    Texture();
    ~Texture();

   static GLuint ReadTextureFromFile(const char* file_name);
   static GLuint ReadTextureFromFileRoughness(const char* file_name);
    bool load(const std::string& path, bool flip = true);
    void bind(GLuint unit) const;
};
class Material {
public:
    GLuint albedoID;
    GLuint roughnessID;

    bool load(const std::string& albedoPath, const std::string& roughPath) {
        albedoID = Texture::ReadTextureFromFile(albedoPath.c_str());
        if (albedoID == 0) return false;

        roughnessID = Texture::ReadTextureFromFile(roughPath.c_str());
        if (roughnessID == 0) return false;

        return true;
    }

    void bind(GLuint albedoUnit, GLuint roughUnit) const {
        glActiveTexture(GL_TEXTURE0 + albedoUnit);
        glBindTexture(GL_TEXTURE_2D, albedoID);

        glActiveTexture(GL_TEXTURE0 + roughUnit);
        glBindTexture(GL_TEXTURE_2D, roughnessID);
    }
};