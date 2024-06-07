#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.hpp"
#include "entity.hpp"
#include "game.hpp"
#include "animation.hpp"
#include "chunk.hpp"
#include "entity.hpp"

#include "afx.hpp"

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
    "   TexCoord = aTexCoord;\n"
    "}\n";

static const char* fragment_shader_source =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D ourTexture;\n"
    "uniform vec4 our_color;\n"
    "void main() {\n"
    "    finalColor = texture(ourTexture, vec2(TexCoord.x, TexCoord.y));\n"
    "}\n";

static const char* lmars_fragment_source =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D ourTexture;\n"
    "uniform sampler2D tileTexture;\n"
    "void main() {\n"
    "    vec2 tiling = fract(TexCoord * 16.0f);\n"
    "    float tile = uint(texture(tileTexture, TexCoord).r * 255.0);\n"
    "    finalColor = texture(ourTexture, vec2(tiling.x * 0.25f + 0.25 * tile, tiling.y));\n"
    "}\n";

Game game;
Camera camera;
Chunk chunks_array[999];

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

glm::mat4 camera_view_matrix(const Camera* my_camera)
{  
    return glm::translate(glm::mat4(1.0f), glm::vec3(-my_camera->pos.x, -my_camera->pos.y, -100.0f));
}

void setConstant(unsigned int program, const char* location, glm::mat4 data)
{
    unsigned int uniform_location = glGetUniformLocation(program, location);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(data));
}

int main()
{
    init_window(&game.window);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) 
    {
        throw(std::string("Failed to initialize GLAD"));
    }

    //make the player entity
    Entity* entity = &game.entities[find_free_entity()];

    *entity = make_entity(entity_player, Vec2{ 0, 0 }, "player.png");

    game.player = entity;

    unsigned int program = afx::shaderProgram(vertex_shader_source, fragment_shader_source);
    unsigned int chunk_program = afx::shaderProgram(vertex_shader_source, lmars_fragment_source);
    
    unsigned int vao;
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    float entity_vertices[] =
    {
        //vertices          texCoords
         1.0,  1.0, 0.0,    1.0f, 1.0f,
        -1.0,  1.0, 0.0,    0.0f, 1.0f,
        -1.0, -1.0, 0.0,    0.0f, 0.0f,
         1.0, -1.0, 0.0,    1.0f, 0.0f,
    };

    unsigned int entity_indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };

    unsigned int entity_vbo, entity_ebo;

    glGenBuffers(1, &entity_vbo);
    glGenBuffers(1, &entity_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, entity_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(entity_vertices), entity_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(entity_indices), entity_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    Sprite tiles_sheet = make_sprite("chunk_textures.png");
    Sprite magma_sprite = make_sprite("magma.png");

    Animation anim_player_idle;
    Animation anim_player_run;
    Animation anim_player_walk;
    Animation anim_player_swing;
    Animation anim_player_stab;

    anim_player_idle = make_animation_txt("player_idle.txt");
    anim_player_run = make_animation_txt("player_run.txt");
    anim_player_walk = make_animation_txt("player_walk.txt");
    anim_player_swing = make_animation_txt("player_swing.txt");

    anim_player_stab.frames[0] = make_sprite("assets/player/playerattack0.png");
    anim_player_stab.frames[1] = make_sprite("assets/player/playerattack1.png");
    
    for(int i = 2; i < 7; i++)
    {
        anim_player_stab.frames[i] = make_sprite("assets/player/playerattack2.png");
    }

    anim_player_stab.frames[7] = make_sprite("assets/player/playerattack3.png");

    anim_player_stab.frame_count = 8;
    anim_player_stab.frame_rate = 0.05;

    game.player->animation = anim_player_idle;
    game.player->animation_enabled = true;

    game.player->player.idle_animation = &anim_player_idle;
    game.player->player.walk_animation = &anim_player_walk;
    game.player->player.run_animation = &anim_player_run;
    game.player->player.stab_animation = &anim_player_stab;
    game.player->player.swing_animation = &anim_player_swing;
    
    while(game.window.running)
    {
        unsigned int start_time = SDL_GetTicks();

        update_window(&game.window);

        glClearColor(0.81f, 0.75f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if(game.window.input.wheel)
        {
            camera.zoom += 0.02 * (game.window.input.wheel_value);
            game.window.input.wheel = false;
        }

        V2i chunk_index = get_chunk_index(game.player->position.x, game.player->position.y);
        
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
                        float noise_x = loop_chunk_index.x + 999;
                        float noise_y = loop_chunk_index.y + 999;
                        
                        new_chunk->index = loop_chunk_index;
                        new_chunk->exists = true;

                        //tiles
                        for(int  k = 0; k < chunk_tiles * chunk_tiles; k++)
                        {
                            int tile_x = k % chunk_tiles;
                            int tile_y = floor(k / chunk_tiles);

                            float noise_arg_x = (noise_x * chunk_size) + (tile_x * tile_size);
                            float noise_arg_y = (noise_y * chunk_size) + (tile_y * tile_size);
                                                                                       
                            float tile_noise = perlin2d(noise_arg_x, noise_arg_y, 0.001, 4);

                            TileType tile;

                            if(tile_noise > 0 && tile_noise < 0.45)
                            {
                                tile = tile_water;
                            }
                            else if(tile_noise > 0.45 && tile_noise < 0.6)
                            {
                                tile = tile_dirt;
                            }
                            else if(tile_noise > 0.6 && tile_noise < 0.7)
                            {
                                tile = tile_grass;
                            }
                            else if(tile_noise > 0.7)
                            {
                                tile = tile_stone;
                            }

                            new_chunk->tiles[k] = tile;
                        }

                        glGenTextures(1, &new_chunk->tileTexture);
                        glBindTexture(GL_TEXTURE_2D, new_chunk->tileTexture);

                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, chunk_tiles, chunk_tiles, 0, GL_RED, GL_UNSIGNED_BYTE, new_chunk->tiles);                   
                  }
                }
                else
                {
                    //DO THIS LALREADY BRUH
                }
            }
        }

        glm::mat4 projection = glm::mat4(1.0f);
        
        projection = glm::perspective(glm::radians(90.0f), game.window.width / game.window.height, 0.1f, 100.0f);
        projection = glm::scale(projection, glm::vec3(1.0f, -1.0f, 1.0f));
        projection = glm::scale(projection, glm::vec3(camera.zoom, camera.zoom, 1.0f));

        glUseProgram(chunk_program);

        //render chunks
        for(int i = 0; i < array_size(chunks_array); i++)
        {
            Chunk* current_chunk = &chunks_array[i];

            V2i chunk_physical = { (current_chunk->index.x * chunk_size * 2), (current_chunk->index.y * chunk_size * 2) };

            int sampler0_location = glGetUniformLocation(chunk_program, "ourTexture");
            int sampler1_location = glGetUniformLocation(chunk_program, "tileTexture"); 

            glUniform1i(sampler0_location, 0);
            glUniform1i(sampler1_location, 1); 

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tiles_sheet.texture);
           
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, current_chunk->tileTexture);
            
            glActiveTexture(GL_TEXTURE0);

            glBindBuffer(GL_ARRAY_BUFFER, entity_vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity_ebo);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(chunk_physical.x, chunk_physical.y, 0.0f));
            model = glm::scale(model, glm::vec3(chunk_size, chunk_size, 0.0f));
            model = glm::translate(model, glm::vec3(1.0f, 1.0f, 0.0f));
    
            glm::mat4 view = glm::mat4(1.0f);

            view = camera_view_matrix(&camera);

            setConstant(chunk_program, "model", model);
            setConstant(chunk_program, "view", view);
            setConstant(chunk_program, "projection", projection);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
         }

        glUseProgram(program);

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

        for(int i = 0; i < game.render_amount; i++)
        {
            Entity* entity = game.render_entities[i];

            V2 entity_scale;

            if(entity->animation_enabled)
            {
                Sprite* current_frame = step_animation(&entity->animation, game.delta_time);

                entity_scale.x = current_frame->width;
                entity_scale.y = current_frame->height;

                glBindTexture(GL_TEXTURE_2D, current_frame->texture);
            }
            else
            {
                entity_scale.x = entity->sprite.width;
                entity_scale.y = entity->sprite.height;

                glBindTexture(GL_TEXTURE_2D, entity->sprite.texture);
            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glBindBuffer(GL_ARRAY_BUFFER, entity_vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity_ebo);

            float entity_x = (entity->position.x);
            float entity_y = (entity->position.y);

            int scale = 1;

            if(entity->velocity.x < 0)
            {
              scale = -1;
            }
            
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, glm::vec3(entity->position.x, entity->position.y, 0.0f));
            model = glm::scale(model, glm::vec3(entity_scale.x, entity_scale.y, 1.0));
        
            glm::mat4 view = camera_view_matrix(&camera);

            setConstant(program, "model", model);
            setConstant(program, "view", view);
            setConstant(program, "projection", projection);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

            glBlendFunc(GL_ONE, GL_ZERO);
        }

        unsigned int end_time = SDL_GetTicks();

        float target_x = game.player->position.x;
        float target_y = game.player->position.y;

        camera.pos.x = lerp(camera.pos.x, target_x, 0.05);
        camera.pos.y = lerp(camera.pos.y, target_y, 0.05);

        game.delta_time = (end_time - start_time) / 1000.0f;

        SDL_GL_SwapWindow(game.window.window);
    }
    
    SDL_GL_DeleteContext(game.window.context);
    SDL_DestroyWindow(game.window.window);
}