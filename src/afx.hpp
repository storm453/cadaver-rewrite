#pragma once

#include <cstdint>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace afx 
{
    glm::mat4 camera_view_matrix(const Camera* my_camera)
    {  
        return glm::translate(glm::mat4(1.0f), glm::vec3(-my_camera->pos.x, -my_camera->pos.y, -100.0f));
    }

    void setConstant(unsigned int program, const char* location, glm::mat4 data)
    {
        unsigned int uniform_location = glGetUniformLocation(program, location);
        glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(data));
    }

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