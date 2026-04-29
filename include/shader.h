#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    Shader(const char* vert_path, const char* frag_path);
    ~Shader();

    void use() const;
    void set_float(const char* name, float v) const;
    void set_int(const char* name, int v) const;
    void set_vec3(const char* name, const glm::vec3& v) const;
    void set_vec4(const char* name, const glm::vec4& v) const;
    void set_mat4(const char* name, const glm::mat4& m) const;

    GLuint id() const { return id_; }

private:
    GLuint id_;
    static GLuint compile(const char* path, GLenum type);
};
