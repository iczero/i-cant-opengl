#include "shaders.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>

Shader::Shader(std::string name): name(name), has_shader(false) {}

Shader::~Shader() {
    if (has_shader) glDeleteShader(id);
}

Shader Shader::from_file(std::string name, GLenum type, std::string file) {
    std::ifstream input(file);
    if (!input) throw std::runtime_error("failed to open shader");
    std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    // if (!input.eof()) throw std::runtime_error("error while reading shader");
    input.close();

    Shader shader(name);
    shader.id = glCreateShader(type);
    shader.has_shader = true;
    const char *source_cstr = source.data();
    // forgive me
    GLint lengths[1] = { static_cast<int>(source.length()) };
    glShaderSource(shader.id, 1, &source_cstr, lengths);
    return shader;
}

bool Shader::compile() {
    glCompileShader(id);
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success) return true;

    // error handling
    int info_log_length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_log_length);
    std::string shader_error(info_log_length, '\0');
    glGetShaderInfoLog(id, info_log_length, NULL, &shader_error[0]);
    std::cerr << "ERROR: shader " << name << " failed to compile" << std::endl;
    std::cerr << shader_error << std::endl;
    return false;
}

ShaderProgram::ShaderProgram(std::string name): name(name) {
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::attach_shader(Shader &shader) {
    if (!shader.has_shader) throw new std::logic_error("provided shader is invalid");
    glAttachShader(id, shader.id);
}

bool ShaderProgram::link() {
    glLinkProgram(id);
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (success) return true;

    int info_log_length;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &info_log_length);
    std::string link_error(info_log_length, '\0');
    glGetProgramInfoLog(id, info_log_length, NULL, &link_error[0]);
    std::cerr << "ERROR: program " << name << " failed to link" << std::endl;
    std::cerr << link_error << std::endl;
    throw std::runtime_error("program failed to link");
    return false;
}

void ShaderProgram::use() {
    glUseProgram(id);
}

int ShaderProgram::get_uniform_location(const char *name) {
    return glGetUniformLocation(id, name);
}

void ShaderProgram::set_uniform(int location, glm::vec4 &vec4) {
    glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
}

void ShaderProgram::set_uniform(int location, glm::mat4 &mat4) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat4[0][0]);
}
