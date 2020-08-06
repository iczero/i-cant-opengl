#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    std::string name;
    unsigned int id;
    bool has_shader = false;
    GLenum type;

    Shader(std::string name);
    ~Shader();
    static Shader from_file(std::string name, GLenum type, std::string file);
    bool compile();
};

class ShaderProgram {
public:
    std::string name;
    unsigned int id;

    ShaderProgram(std::string name);
    ~ShaderProgram();
    void attach_shader(Shader &shader);
    bool link();
    void use();
    int get_uniform_location(const char *name);
    void set_uniform(int location, glm::vec4 &vec4);
    void set_uniform(int location, glm::mat4 &mat4);
};
