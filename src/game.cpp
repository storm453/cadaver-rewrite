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

#define STB_IMAGE_IMPLEMENTATION   

#include "stb_image.h"
#include "SimplexNoise.h"

Game game;

float lerp(float a, float b, float t)
{
    return a * (1 - t) + b * t;
}

Camera camera;

Chunk chunks_array[999];

static const char* vertex_shader_source =
    "#version 330 core\n"
    "out vec3 vertex_color;\n"
    "out vec2 TexCoord;\n"
    "uniform vec2 shift;\n"
    "uniform vec2 scale;\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4((aPos.x * scale.x) + shift.x, (aPos.y * scale.y) + shift.y, aPos.z, 1.0);\n"
    "    vertex_color = aColor;\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

static const char* fragment_shader_source =
    "#version 330 core\n"
    "in vec3 vertex_color;\n"
    "in vec2 TexCoord;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D ourTexture;\n"
    "uniform vec4 our_color;\n"
    "void main() {\n"
    "    finalColor = texture(ourTexture, TexCoord) * vec4(vertex_color.x, our_color.y, vertex_color.z, 1.0f);\n"
    "}\n";

unsigned int shader_program(const char *vertex_shader_source, const char *fragment_shader_source) 
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

Sprite make_sprite(const char* filename)
{
    Sprite temp;
    
    glGenTextures(1, &temp.texture);
    glBindTexture(GL_TEXTURE_2D, temp.texture);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

    temp.width = width;
    temp.height = height;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return temp;
}

int main()
{
    init_window(&game.window);

    #if defined(__WIN32__)
        glewInit();
    #endif

    stbi_set_flip_vertically_on_load(true);

    //make a couple entities
    for(int i = 0; i < 10; i++)
    {
        Entity* entity = &game.entities[find_free_entity()];

        *entity = make_entity(entity_object, Vec2{ (rand() / (float)RAND_MAX) * (game.window.width * 2), (rand() / (float)RAND_MAX) * (game.window.height * 2) }, "tree.png");
    }

    //make the player entity
    {
        game.player = make_player();
    }

    float last_time = SDL_GetTicks();

    unsigned int program = shader_program(vertex_shader_source, fragment_shader_source);

    glUseProgram(program);
    
    unsigned int vao;
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    float entity_space_size = 1;

    float entity_vertices[] =
    {
        //four vertices, forms a square
         entity_space_size,  entity_space_size, 0.0,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        -entity_space_size,  entity_space_size, 0.0,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        -entity_space_size, -entity_space_size, 0.0,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
         entity_space_size, -entity_space_size, 0.0,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
    };

    unsigned int entity_indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };

    float chunk_space_size_x = (chunk_size / game.window.width);
    float chunk_space_size_y = (chunk_size / game.window.height);

    float chunk_vertices[] =
    {
        -chunk_space_size_x,  chunk_space_size_y, 0.0,
         chunk_space_size_x,  chunk_space_size_y, 0.0,
         chunk_space_size_x, -chunk_space_size_y, 0.0,
        -chunk_space_size_x, -chunk_space_size_y, 0.0,
    };

    //entity data
    unsigned int entity_vbo;
    glGenBuffers(1, &entity_vbo);

    unsigned int entity_ebo;
    glGenBuffers(1, &entity_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, entity_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(entity_vertices), entity_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(entity_indices), entity_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //chunk buffer
    unsigned int chunk_vbo;

    // glGenBuffers(1, &chunk_vbo);

    // glBindBuffer(GL_ARRAY_BUFFER, chunk_vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(chunk_vertices), chunk_vertices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray(0);

    while(game.window.running)
    {
        //set screen color
        glClearColor(0.81f, 0.75f, 0.8f, 1.0f);
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

        //render chunks
        for(int i = 0; i < array_size(chunks_array); i++)
        {
            Chunk* current_chunk = &chunks_array[i];

            V2i chunk_physical = { current_chunk->index.x * chunk_size, current_chunk->index.y * chunk_size };

            
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

                int shift_location = glGetUniformLocation(program, "shift");
                glUniform2f(shift_location, entity_x, entity_y);

                float test_scale = entity->sprite.width / game.window.width;

                float test_scale2 = entity->sprite.height / game.window.height;

                int scale_location = glGetUniformLocation(program, "scale");
                glUniform2f(scale_location, test_scale, test_scale2);

                glBindTexture(GL_TEXTURE_2D, entity->sprite.texture);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity_ebo);
                
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

                glBlendFunc(GL_ONE, GL_ZERO);
            }
        }

        float time = SDL_GetTicks() * 0.001;

        float green_value = (sin(time)) / 2.0f + 0.5f;

        int vertex_color_location = glGetUniformLocation(program, "our_color");
        glUniform4f(vertex_color_location, 0.41, green_value, 0.53, 1.0f);

        SDL_GL_SwapWindow(game.window.window);
    
        unsigned int end_time = SDL_GetTicks();

        game.delta_time = (end_time - start_time) / 1000.f;

        //move camera
        float target_x = game.player->position.x; //- game.window.width / 2 //+ game.player->origin.x / 2;
        float target_y = game.player->position.y; //- game.window.height / 2;

        camera.x = lerp(camera.x, target_x, 0.1);
        camera.y = lerp(camera.y, target_y, 0.1);
    }
    
    clean_window(&game.window);
}