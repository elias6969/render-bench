#ifndef MATERIAL_H
#define MATERIAL_H

#include "core/Camera.h"
#include "core/Shader.h"
#include <glad/glad.h>
#include <map>
#include <string>
#include <variant>
#include <vector>

struct TextureInfo {
  GLuint id = 0;
  std::string type;
  int unit = 0;
};

class Material {
public:
  Shader *shader = nullptr;
  std::vector<TextureInfo> textures;
  std::map<std::string, glm::vec3> vec3Uniforms;
  std::map<std::string, float> floatUniforms;
  std::map<std::string, int> intUniforms;
  std::map<std::string, std::variant<int, float, glm::vec3>> uniforms;

  void apply() {
    if (!shader)
      return;

    shader->use();

    // Bind all textures
    for (auto &tex : textures) {
      glActiveTexture(GL_TEXTURE0 + tex.unit);
      glBindTexture(GL_TEXTURE_2D, tex.id);
      shader->setUniform(("texture_" + tex.type).c_str(), tex.unit);
    }


for (auto &[name, val] : uniforms) {
    std::visit([&](auto&& v) {
        shader->setUniform(name, v);
    }, val);
}

  }
};

#endif // MATERIAL_H
