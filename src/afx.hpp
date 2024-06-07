#pragma once

#include <cstdint>
#include <stdio.h>

#include <glad/glad.h>

namespace afx 
{
    unsigned int shaderProgram(const char *vertex_shader_source, const char *fragment_shader_source) 
    {
        unsigned int program, vertex_shader, fragment_shader;

        //vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
        glCompileShader(vertex_shader);

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
        glCompileShader(fragment_shader);

        int success;

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            char error_message[1024];

            glGetShaderInfoLog(fragment_shader, sizeof(error_message), NULL, error_message);

            printf("FRAGMENT ERROR %s \n", error_message);
        }

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            char error_message[1024];

            glGetShaderInfoLog(vertex_shader, sizeof(error_message), NULL, error_message);

            printf("VERTEX ERROR %s \n", error_message);
        }

        program = glCreateProgram();

        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if(!success)
        {
            char error_message[1024];

            glGetProgramInfoLog(program, sizeof(error_message), NULL, error_message);

            printf("PROGRAM ERROR %s \n", error_message);
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);  

        return program;
    }
}