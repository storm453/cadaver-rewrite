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
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(10, 10, 1.0));

        afx::setConstant(program, "model", model);
        afx::setConstant(program, "view", view);
        afx::setConstant(program, "projection", projection);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }
    
    void drawUI(unsigned int program, float x, float y)
    {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        view = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, -100));

        model = glm::scale(model, glm::vec3(50, 50, 1.0));
        
        projection = glm::ortho(-640.0, 640.0, -360.0, 360.0, 0.1, 100.0);
        //projection = glm::ortho(-128.0, 128.0, -90.0, 90.0);
        //projection = glm::perspective(glm::radians(90.0f), game.window.width / game.window.height, 0.1f, 100.0f);
        //projection = glm::scale(projection, glm::vec3(1.0f, -1.0f, 1.0f));

        afx::setConstant(program, "model", model);
        afx::setConstant(program, "view", view);
        afx::setConstant(program, "projection", projection);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }

    void drawEntity(unsigned int program, Entity* entity, V2* entity_scale, glm::mat4 view, glm::mat4 projection)
    {
        int scale = 1;

        if(entity->character.velocity.x < 0)
        {
            scale = -1;
        }
        
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(entity->position.x, entity->position.y, 0.0f));
        model = glm::scale(model, glm::vec3(entity_scale->x * scale, entity_scale->y, 1.0));

        afx::setConstant(program, "model", model);
        afx::setConstant(program, "view", view);
        afx::setConstant(program, "projection", projection);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        glBlendFunc(GL_ONE, GL_ZERO);
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