#include "tools/TextureManager.h"
#include "tools/EngineConfig.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static GLenum detectFormat(int nrChannels) {
  if (nrChannels == 1)
    return GL_RED;
  if (nrChannels == 3)
    return GL_RGB;
  if (nrChannels == 4)
    return GL_RGBA;
  return 0; // unsupported
}

GLuint TextureLoader::loadTexture(const std::string &path, bool flip) {
  return loadTextureAdvanced(path, GL_REPEAT, GL_REPEAT,
                             GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, flip);
}

GLuint TextureLoader::loadTextureAdvanced(const std::string &path, GLint wrapS,
                                          GLint wrapT, GLint minFilter,
                                          GLint magFilter, bool flip) {
  stbi_set_flip_vertically_on_load(flip);

  int width, height, nrChannels;
  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  if (!data) {
    std::cerr << "Failed to load texture: " << path << "\n";
    return 0;
  }

  GLenum format = detectFormat(nrChannels);
  if (format == 0) {
    std::cerr << "Unsupported channel count: " << nrChannels << "\n";
    stbi_image_free(data);
    return 0;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Custom parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

  stbi_image_free(data);
  return texture;
}

GLuint TextureLoader::loadCubemap(const std::vector<std::string> &faces,
                                  bool flip) {
  stbi_set_flip_vertically_on_load(flip);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data =
        stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      GLenum format = detectFormat(nrChannels);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height,
                   0, format, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cerr << "Cubemap failed to load: " << faces[i] << "\n";
      stbi_image_free(data);
    }
  }

  // Default parameters
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}
