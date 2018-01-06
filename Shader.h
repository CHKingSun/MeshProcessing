//
// Created by KingSun on 2017/12/17.
//

#ifndef ASSIMP_SHADER_H
#define ASSIMP_SHADER_H

#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vec3.hpp>
#include <mat4x4.hpp>

//TODO Light wrong!

struct Light{
    Light(){
		position = glm::vec4(0.0, 0.0, 0.0, 0.0);
		ambient = glm::vec3(0.1, 0.1, 0.1);
		diffuse = glm::vec3(1.0, 1.0, 1.0);
		specular = glm::vec3(1.0, 1.0, 1.0);
    }
    glm::vec4 position;
    glm::vec3 ambient, diffuse, specular;
};

class Shader{
public:
    GLuint program;

    Shader(): n_light(0){}
    Shader(std::string vs_filename, std::string fs_filename): n_light(0)
    {
        compile_shader(vs_filename, fs_filename);
    }

    bool compile_shader(std::string vs_filename, std::string fs_filename)
    {
        program = glCreateProgram();
        if(!program)
        {
            std::cerr<<"Error creating shader program"<<std::endl;
            glDeleteProgram(program);
            return false;
        }

        std::string vs, fs;
        if(!read_file(vs_filename, vs) || !read_file(fs_filename, fs))
        {
            glDeleteProgram(program);
            return false;
        }

        add_shader(program, vs, GL_VERTEX_SHADER);
        add_shader(program, fs, GL_FRAGMENT_SHADER);

        glLinkProgram(program);
        GLint flag;
        GLchar error_log[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &flag);
        if(!flag)
        {
            glGetProgramInfoLog(program, sizeof(error_log), NULL, error_log);
            std::cerr<<"Error linking program: "<<error_log<<std::endl;
            glDeleteProgram(program);
            return false;
        }

        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &flag);
        if(!flag)
        {
            glGetProgramInfoLog(program, sizeof(error_log), NULL, error_log);
            std::cerr<<"Program valid: "<<error_log<<std::endl;
            glDeleteProgram(program);
            return false;
        }

        glUseProgram(program);
        return true;
    }

    bool add_light(Light light)
    {
        if(n_light == MAX_LIGHTS)
        {
            std::cerr<<"Lights number is out of range."<<std::endl;
            return false;
        }

        lights[n_light++] = light;
        return true;
    }

    void init_light()
    {
		Light light;
		light.position = glm::vec4(1.0, 1.0, 0.0, 0.0);
		add_light(light);
		light.position = glm::vec4(0.0, 1.0, 1.0, 0.0);
		add_light(light);
		light.position = glm::vec4(-1.0, 1.0, 0.0, 0.0);
		add_light(light);
		light.position = glm::vec4(0.0, 1.0, -1.0, 0.0);
		add_light(light);
    }

    void load_lights()
    {
        for(int i = 0; i < n_light; ++i)
        {
            std::string u_light = "u_lights[" + std::to_string(i) + "]";
            glUniform4fv(glGetUniformLocation(program, (u_light+".position").data()), 1, &lights[i].position[0]);
            glUniform3fv(glGetUniformLocation(program, (u_light+".ambient").data()), 1, &lights[i].ambient[0]);
            glUniform3fv(glGetUniformLocation(program, (u_light+".diffuse").data()), 1, &lights[i].diffuse[0]);
            glUniform3fv(glGetUniformLocation(program, (u_light+".specular").data()), 1, &lights[i].specular[0]);
        }
    }

    void enable_light()
    {
        for(int i = 0; i < n_light; ++i)
        {
            glUniform1i(glGetUniformLocation(program, ("u_lights[" + std::to_string(i) + "].enable").data()), true);
        }
    }

    void disable_light()
    {
        for(int i = 0; i < n_light; ++i)
        {
            glUniform1i(glGetUniformLocation(program, ("u_lights[" + std::to_string(i) + "].enable").data()), false);
        }
    }

protected:
    void add_shader(GLuint program, const std::string text, GLenum type)
    {
        GLuint shader = glCreateShader(type);
        if(!shader)
        {
            std::cerr<<"Error creating shader type: "<<type<<std::endl;
            exit(1);
        }

        const char *p = text.c_str();
        glShaderSource(shader, 1, &p, NULL);
        glCompileShader(shader);

        GLint flag;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &flag);
        if(!flag)
        {
            GLchar  error_log[1024];
            glGetShaderInfoLog(shader, sizeof(error_log), NULL, error_log);
            std::cerr<<"Error compiling shader: "<<error_log<<std::endl;
            exit(1);
        }

        glAttachShader(program, shader);
    }

    bool read_file(const std::string filename, std::string &out)
    {
        std::ifstream file(filename, std::ios::in);
        if(!file.is_open())
        {
            std::cerr<<"File "<<filename<<" read failed!"<<std::endl;
            return false;
        }

        out = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return true;
    }

private:
    static const int MAX_LIGHTS = 4;
    Light lights[MAX_LIGHTS];
    int n_light;
};

#endif //ASSIMP_SHADER_H
