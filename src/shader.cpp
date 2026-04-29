#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

static std::string read_file(const char* path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error(std::string("Cannot open shader: ") + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

GLuint Shader::compile(const char* path, GLenum type) {
    std::string src = read_file(path);
    const char* c   = src.c_str();
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &c, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("Shader compile error (") + path + "): " + log);
    }
    return s;
}

Shader::Shader(const char* vert_path, const char* frag_path) {
    GLuint v = compile(vert_path, GL_VERTEX_SHADER);
    GLuint f = compile(frag_path, GL_FRAGMENT_SHADER);
    id_ = glCreateProgram();
    glAttachShader(id_, v); glAttachShader(id_, f);
    glLinkProgram(id_);
    GLint ok; glGetProgramiv(id_, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetProgramInfoLog(id_, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("Shader link error: ") + log);
    }
    glDeleteShader(v); glDeleteShader(f);
}

Shader::~Shader() { glDeleteProgram(id_); }

void Shader::use()                                        const { glUseProgram(id_); }
void Shader::set_float(const char* n, float v)           const { glUniform1f(glGetUniformLocation(id_, n), v); }
void Shader::set_int(const char* n, int v)               const { glUniform1i(glGetUniformLocation(id_, n), v); }
void Shader::set_vec3(const char* n, const glm::vec3& v) const { glUniform3fv(glGetUniformLocation(id_, n), 1, glm::value_ptr(v)); }
void Shader::set_vec4(const char* n, const glm::vec4& v) const { glUniform4fv(glGetUniformLocation(id_, n), 1, glm::value_ptr(v)); }
void Shader::set_mat4(const char* n, const glm::mat4& m) const { glUniformMatrix4fv(glGetUniformLocation(id_, n), 1, GL_FALSE, glm::value_ptr(m)); }
