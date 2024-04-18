#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>

//silence
#define GL_SILENCE_DEPRECATION

#if defined(__WIN32__)
#include <GL/glew.h>
#include <GL/glu.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#endif

#define GLEW_STATIC

#include <cstdint>

//additional utils
#include "window.h"
#include "entity.h"
#include "game.h"
#include "player.h"
#include "animation.h"
#include "chunk.h"
#include "entity.h"

#include "SimplexNoise.h"

Game game;

float lerp(float a, float b, float t)
{
    return a * (1 - t) + b * t;
}

Camera camera;

Chunk chunks_array[999];

static const GLchar* vertex_shader_source =
    "#version 120\n"
    "attribute vec2 coord2d;\n"
    "void main() {\n"
    "    gl_Position = vec4(coord2d, 0.0, 1.0);\n"
    "}\n";

static const GLchar* fragment_shader_source =
    "#version 120\n"
    "void main() {\n"
    "    gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);\n"
    "}\n";

GLuint common_get_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) 
{
    GLchar *log = NULL;
    GLint log_length, success;
    GLuint fragment_shader, program, vertex_shader;

    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);

    /* Link shaders */
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

    /* Cleanup. */
    free(log);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

int main()
{
    init_window(&game.window);

    //make a couple entities
    for(int i = 0; i < 3; i++)
    {
        Entity* entity = &game.entities[find_free_entity()];

        *entity = make_entity(entity_object, Vec2{ rand() / (float)RAND_MAX * 250, rand() / (float)RAND_MAX * 250, });
    }

    //make the player entity
    {
        game.player = make_player();
    }

    float last_time = SDL_GetTicks();

    glewInit();

    GLuint program = common_get_shader_program(vertex_shader_source, fragment_shader_source);

    glUseProgram(program);
    glViewport(0, 0, game.window.width, game.window.height);
    
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    while(game.window.running)
    {
        //set screen color
        glClearColor(0.23f, 0.23f, 0.38f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        V2i chunk_index = get_chunk_index(game.player->position.x, game.player->position.y);

        //chunk logic
        for(int i = -chunk_load; i <= chunk_load; i++)
        {
            for(int j = -chunk_load; j <= chunk_load; j++)
            {
                V2i loop_chunk_index = { chunk_index.x + i, chunk_index.y + j };

                Chunk* check_chunk = lookup_chunk(loop_chunk_index, array_size(chunks_array), chunks_array);

                if(check_chunk == NULL)
                {
                    //chunk doesn't exist, make it
                    Chunk* new_chunk = find_free_chunk_slot(array_size(chunks_array), chunks_array);
                    
                    if(new_chunk != NULL)
                    {
                        new_chunk->index = loop_chunk_index;
                        new_chunk->exists = true;
                    }
                }
                else
                {
                    //chunk does exist, so move it to the top of the array
                }
            }
        }

        unsigned int start_time = SDL_GetTicks();

        update_window(&game.window);

        //move camera
        float target_x = game.player->position.x; //- game.window.width / 2 //+ game.player->origin.x / 2;
        float target_y = game.player->position.y; //- game.window.height / 2;

        camera.x = lerp(camera.x, target_x, 0.1);
        camera.y = lerp(camera.y, target_y, 0.1);

        //render chunks
        for(int i = 0; i < array_size(chunks_array); i++)
        {
            Chunk* current_chunk = &chunks_array[i];

            V2i chunk_physical = { current_chunk->index.x * chunk_size, current_chunk->index.y * chunk_size };

            //render here
        }

        //entity loop
        for(int i = 0; i < max_entity_count; i++)
        {
            Entity* entity = &game.entities[i];

            entity_update(entity);

            if(entity->type != entity_none)
            {
                float entity_x = (entity->position.x - camera.x) / game.window.width;
                float entity_y = (-entity->position.y + camera.y) / game.window.height;

                float entity_space_size = 0.1;

                float entity_vertices[] =
                {
                    entity_x, float(entity_y + entity_space_size), 0.0,
                    float(entity_x - entity_space_size), float(entity_y - entity_space_size), 0.0,
                    float(entity_x + entity_space_size), float(entity_y - entity_space_size), 0.0
                };

                GLuint vbo;

                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(entity_vertices), entity_vertices, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(0);

                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }
        
        //render
        SDL_GL_SwapWindow(game.window.window);
    
        unsigned int end_time = SDL_GetTicks();

        game.delta_time = (end_time - start_time) / 1000.f;
    }
    
    clean_window(&game.window);
}