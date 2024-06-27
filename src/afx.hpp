#pragma once

#include <cstdint>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace afx 
{
    typedef unsigned int VBO;

    glm::mat4 camera_view_matrix(const Camera* my_camera)
    {
        return glm::translate(glm::mat4(1.0f), glm::vec3(-my_camera->pos.x, -my_camera->pos.y, -100.0f));
    }

    void setConstant(unsigned int program, const char* location, glm::mat4 data)
    {
        unsigned int uniform_location = glGetUniformLocation(program, location);
        glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(data));
    }

    void drawRectangle(unsigned int program, glm::mat4 view, glm::mat4 projection, float x, float y)
    {
        glUseProgram(program);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(10, 10, 1.0));

        afx::setConstant(program, "model", model);
        afx::setConstant(program, "view", view);
        afx::setConstant(program, "projection", projection);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }
    
    void drawUI(unsigned int program, glm::mat4 projection, float x, float y)
    {
        glUseProgram(program);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);

        view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -100.0f));

        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(10, 10, 1.0));

        afx::setConstant(program, "model", model);
        afx::setConstant(program, "view", view);
        afx::setConstant(program, "projection", projection);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
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