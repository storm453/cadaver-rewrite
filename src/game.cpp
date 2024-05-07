#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

Game game;

float lerp(float a, float b, float t)
{
    return a * (1 - t) + b * t;
}

Camera camera;

Chunk chunks_array[999];

static const char* vertex_shader_source =
    "#version 330 core\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "void main() {\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\n";

static const char* fragment_shader_source =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D ourTexture;\n"
    "uniform vec4 our_color;\n"
    "void main() {\n"
    "    finalColor = texture(ourTexture, TexCoord) * vec4(our_color.x, our_color.y, our_color.z, 1.0f);\n"
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

//NOISE FUNCTION
static int SEED = std::time(0);

static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y)
{
    int tmp = hash[(y + SEED) % 256];
    return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y-x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3-2*s));
}

float noise2d(float x, float y)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int);
    int t = noise2(x_int+1, y_int);
    int u = noise2(x_int, y_int+1);
    int v = noise2(x_int+1, y_int+1);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

float perlin2d(float x, float y, float freq, int depth)
{
    float xa = x*freq;
    float ya = y*freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for(i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
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
         entity_space_size,  entity_space_size, 0.0,    1.0f, 1.0f,
        -entity_space_size,  entity_space_size, 0.0,    0.0f, 1.0f,
        -entity_space_size, -entity_space_size, 0.0,    0.0f, 0.0f,
         entity_space_size, -entity_space_size, 0.0,    1.0f, 0.0f,
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
        0.0,                    0.0,                0.0,    0.0f, 1.0f,
        chunk_space_size_x,     0.0,                0.0,    1.0f, 1.0f,
        0.0,                   -chunk_space_size_y, 0.0,    0.0f, 0.0f,
        chunk_space_size_x,    -chunk_space_size_y, 0.0,    1.0f, 0.0f,
    };

    unsigned int chunk_indices[] =
    {
        0, 1, 2,
        1, 2, 3,
    };

    float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

    //entity data
    unsigned int entity_vbo, entity_ebo;

    glGenBuffers(1, &entity_vbo);
    glGenBuffers(1, &entity_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, entity_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(entity_vertices), entity_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(entity_indices), entity_indices, GL_STATIC_DRAW);

    //chunk buffer
    unsigned int chunk_vbo, chunk_ebo;

    glGenBuffers(1, &chunk_vbo);
    glGenBuffers(1, &chunk_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, chunk_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(chunk_vertices), chunk_vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(chunk_indices), chunk_indices, GL_STATIC_DRAW);
    //enable attrib arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //cube data
    unsigned int cube_vbo;

    glGenBuffers(1, &cube_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    Sprite ltt_sprite = make_sprite("grass.png");
    Sprite gtt_sprite = make_sprite("grass2.png");
    Sprite gss_sprite = make_sprite("ground3.png");

    Sprite magma_sprite = make_sprite("magma.png");

    float zoom = 1;

    while(game.window.running)
    {
        
        if(game.window.input.wheel)
        {
            zoom += 0.1 * (game.window.input.wheel_value / 1);
            game.window.input.wheel = false;
        }

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

        //reset scale uniform to 1
        int scale_location = glGetUniformLocation(program, "scale");
        glUniform2f(scale_location, 1, 1);

        //render chunks
        for(int i = 0; i < array_size(chunks_array); i++)
        {
            Chunk* current_chunk = &chunks_array[i];

            V2i chunk_physical = { (current_chunk->index.x * chunk_size), (current_chunk->index.y * chunk_size) };

            glBindTexture(GL_TEXTURE_2D, ltt_sprite.texture);

            glBindBuffer(GL_ARRAY_BUFFER, chunk_vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk_ebo);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

            float chunk_space_x = (chunk_physical.x - camera.x) / game.window.width;
            float chunk_space_y = -(chunk_physical.y - camera.y) / game.window.height;

            float noise_input_x = current_chunk->index.x + 9999;
            float noise_input_y = current_chunk->index.y + 9999;

            float noise = perlin2d(noise_input_x, noise_input_y, 0.1, 4);

            int vertex_color_location = glGetUniformLocation(program, "our_color");
            glUniform4f(vertex_color_location, noise, noise, 1.0, 1.0);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(zoom, zoom, 0.0f));
            model = glm::translate(model, glm::vec3(chunk_space_x, chunk_space_y, 0.0f));

            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);

            unsigned int model_location = glGetUniformLocation(program, "model");
            glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

            unsigned int view_location = glGetUniformLocation(program, "view");
            glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

            unsigned int projection_location = glGetUniformLocation(program, "projection");
            glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }

        float time = SDL_GetTicks() * 0.001;

        float green_value = (sin(time)) / 2.0f + 0.5f;

        game.render_amount = 0;

        for(int i = 0; i < max_entity_count; i++)
        {
            Entity* entity = &game.entities[i];

            if(entity->type == entity_none) continue;

            entity_update(entity);

            if(entity->render)
            {
                game.render_entities[game.render_amount] = entity;

                game.render_amount++;
            }
        }

        //make a sorting algorithm and sort using game.render_entities[x]->depth
        for(int i = 0; i < game.render_amount; i++)
        {

        }

        //entity loop
        for(int i = 0; i < game.render_amount; i++)
        {
            Entity* entity = game.render_entities[i];

            glBindTexture(GL_TEXTURE_2D, entity->sprite.texture);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glBindBuffer(GL_ARRAY_BUFFER, entity_vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity_ebo);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

            float entity_x = (entity->position.x - camera.x) / game.window.width;
            float entity_y = (-entity->position.y + camera.y) / game.window.height;

            float entity_scale_x = entity->sprite.width / game.window.width;
            float entity_scale_y = entity->sprite.height / game.window.height;

            int vertex_color_location = glGetUniformLocation(program, "our_color");
            glUniform4f(vertex_color_location, 0.41, green_value, 0.53, 1.0f);

            //transformation matrix
            glm::mat4 model = glm::mat4(1.0f);
            
            model = glm::scale(model, glm::vec3(zoom, zoom, 0.0f));
            model = glm::translate(model, glm::vec3(entity_x, entity_y, 0.0f));
            model = glm::scale(model, glm::vec3(entity_scale_x, entity_scale_y, 0.0f)); 

            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
            
            unsigned int model_location = glGetUniformLocation(program, "model");
            glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

            unsigned int view_location = glGetUniformLocation(program, "view");
            glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

            unsigned int projection_location = glGetUniformLocation(program, "projection");
            glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

            glBlendFunc(GL_ONE, GL_ZERO);
        }

        //draw 3d stuff
        glBindTexture(GL_TEXTURE_2D, magma_sprite.texture);

        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        
        //model matrix
        glm::mat4 model = glm::mat4(1.0f);

        float angle = SDL_GetTicks() * 0.001;

        model = glm::rotate(model, glm::radians(angle * 50.0f), glm::vec3(1.0f, 0.5f, 0.0f)); 

        //view matrix
        glm::mat4 view = glm::mat4(1.0f);

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        //projection matrix
        glm::mat4 projection = glm::mat4(1.0f);

        projection = glm::perspective(glm::radians(45.0f), game.window.width / game.window.height, 0.1f, 100.0f);

        //send to vertex shader
        unsigned int model_location = glGetUniformLocation(program, "model");
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

        unsigned int view_location = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int projection_location = glGetUniformLocation(program, "projection");
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
        
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //render
        SDL_GL_SwapWindow(game.window.window);
    
        unsigned int end_time = SDL_GetTicks();

        game.delta_time = (end_time - start_time) / 1000.f;

        //move camera
        float target_x = game.player->position.x;
        float target_y = game.player->position.y;

        camera.x = lerp(camera.x, target_x, 0.05);
        camera.y = lerp(camera.y, target_y, 0.05);
    }
    
    clean_window(&game.window);
}