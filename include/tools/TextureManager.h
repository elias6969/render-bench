#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <glad/glad.h>
#include <string>
#include <vector>

namespace TextureLoader {
//simple texture loading
GLuint loadTexture(const std::string &path, bool flip = true);

//Advanced 2d texture that includes wrapping and filtering 
GLuint loadTextureAdvanced(const std::string &path, GLint wrapS, GLint wrapT,
                           GLint minFilter, GLint magFilter, bool flip = true);
//Load a cubemap
GLuint loadCubemap(const std::vector<std::string> &faces, bool flip = false);
}
#endif
