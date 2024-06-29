#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.hpp"
#include "entity.hpp"
#include "main.hpp"
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

static const char* fragment_shader_source_flat =
    "#version 330 core\n"
    "out vec4 finalColor;\n"
    "uniform vec4 our_color;\n"
    "void main() {\n"
    "    finalColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
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

int main()
{
    float previous_time = 0;

    init_window(&game.window);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) 
    {
        throw(std::string("Failed to initialize GLAD"));
    }

    //animations
    Animation anim_player_idle;
    Animation anim_player_walk;
    Animation anim_player_run;
    Animation anim_player_swing;
    Animation anim_player_stab;

    anim_player_idle.frames[0] = make_sprite("assets/player/playeridle0.png");
    anim_player_idle.frames[1] = make_sprite("assets/player/playeridle1.png");
    anim_player_idle.frames[2] = make_sprite("assets/player/playeridle2.png");
    anim_player_idle.frames[3] = make_sprite("assets/player/playeridle3.png");
    anim_player_idle.frames[4] = make_sprite("assets/player/playeridle4.png");
    anim_player_idle.frames[5] = make_sprite("assets/player/playeridle5.png");
    anim_player_idle.frames[6] = make_sprite("assets/player/playeridle6.png");
    anim_player_idle.frames[7] = make_sprite("assets/player/playeridle7.png");
    anim_player_idle.frames[8] = make_sprite("assets/player/playeridle8.png");
    anim_player_idle.frames[9] = make_sprite("assets/player/playeridle9.png");

    anim_player_idle.frame_count = 10;
    anim_player_idle.frame_rate = 0.1;

    anim_player_walk.frames[0] = make_sprite("assets/player/playerwalk0.png");
    anim_player_walk.frames[1] = make_sprite("assets/player/playerwalk1.png");
    anim_player_walk.frames[2] = make_sprite("assets/player/playerwalk2.png");
    anim_player_walk.frames[3] = make_sprite("assets/player/playerwalk3.png");
    anim_player_walk.frames[4] = make_sprite("assets/player/playerwalk4.png");
    anim_player_walk.frames[5] = make_sprite("assets/player/playerwalk5.png");
    anim_player_walk.frames[6] = make_sprite("assets/player/playerwalk6.png");
    anim_player_walk.frames[7] = make_sprite("assets/player/playerwalk7.png");

    anim_player_walk.frame_count = 8;
    anim_player_walk.frame_rate = 0.1;

    anim_player_run.frames[0] = make_sprite("assets/player/playerrun0.png");
    anim_player_run.frames[1] = make_sprite("assets/player/playerrun1.png");
    anim_player_run.frames[2] = make_sprite("assets/player/playerrun2.png");
    anim_player_run.frames[3] = make_sprite("assets/player/playerrun3.png");
    anim_player_run.frames[4] = make_sprite("assets/player/playerrun4.png");
    anim_player_run.frames[5] = make_sprite("assets/player/playerrun5.png");
    anim_player_run.frames[6] = make_sprite("assets/player/playerrun6.png");
    anim_player_run.frames[7] = make_sprite("assets/player/playerrun7.png");
    anim_player_run.frames[8] = make_sprite("assets/player/playerrun8.png");

    anim_player_run.frame_count = 9;
    anim_player_run.frame_rate = 0.1;

    anim_player_swing.frames[0] = make_sprite("assets/player/playerswing0.png");
    anim_player_swing.frames[1] = make_sprite("assets/player/playerswing1.png");
    anim_player_swing.frames[2] = make_sprite("assets/player/playerswing2.png");

    anim_player_swing.frame_count = 3;
    anim_player_swing.frame_rate = 0.1;

    anim_player_stab.frames[0] = make_sprite("assets/player/playerattack0.png");
    anim_player_stab.frames[1] = make_sprite("assets/player/playerattack1.png");
    
    for(int i = 2; i < 7; i++)
    {
        anim_player_stab.frames[i] = make_sprite("assets/player/playerattack2.png");
    }

    anim_player_stab.frames[7] = make_sprite("assets/player/playerattack3.png");

    anim_player_stab.frame_count = 8;
    anim_player_stab.frame_rate = 0.05;

    //make the player entity
    Entity* entity = &game.entities[find_free_entity()];

    *entity = make_entity(V2{ 0, 0 }, FLAG_PLAYER | FLAG_CHARACTER);

    game.player = entity;

    //make some entities
    for(int i = 0; i < 1; i++)
    {
        Entity* entity = &game.entities[find_free_entity()];

        *entity = make_entity(V2{ 5, 0 }, FLAG_ENEMY | FLAG_CHARACTER);

        entity->animation = anim_player_walk;
    }

    unsigned int program = afx::shaderProgram(vertex_shader_source, fragment_shader_source);
    unsigned int chunk_program = afx::shaderProgram(vertex_shader_source, lmars_fragment_source);
    unsigned int draw_program = afx::shaderProgram(vertex_shader_source, fragment_shader_source_flat);
    
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

    glBindBuffer(GL_ARRAY_BUFFER, entity_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity_ebo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    Sprite tiles_sheet = make_sprite("chunk_textures.png");
    Sprite magma_sprite = make_sprite("magma.png");
    Sprite player_sprite = make_sprite("player.png");

    if(game.player != NULL) 
    {
        game.player->animation = anim_player_idle;
        game.player->animation_enabled = true;

        game.player->player.idle_animation = &anim_player_idle;
        game.player->player.walk_animation = &anim_player_walk;
        game.player->player.run_animation = &anim_player_run;
        game.player->player.stab_animation = &anim_player_stab;
        game.player->player.swing_animation = &anim_player_swing;
    }
    
    previous_time = SDL_GetTicks();

    while(game.window.running)
    {
        update_window(&game.window);

        glClearColor(0.81f, 0.75f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if(game.window.input.wheel)
        {
            camera.zoom += 0.02 * (game.window.input.wheel_value);
            game.window.input.wheel = false;
        }

        if(game.player != NULL)
        {
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
                }
            }
        }

        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        
        projection = glm::perspective(glm::radians(90.0f), game.window.width / game.window.height, 0.1f, 100.0f);
        projection = glm::scale(projection, glm::vec3(1.0f, -1.0f, 1.0f));
        projection = glm::scale(projection, glm::vec3(camera.zoom, camera.zoom, 1.0f));

        view = afx::camera_view_matrix(&camera);

        glUseProgram(chunk_program);    

        //render chunks
        for(int i = 0; i < array_size(chunks_array); i++)
        {
            Chunk* current_chunk = &chunks_array[i];

            V2i chunk_physical = { (current_chunk->index.x * chunk_size), (current_chunk->index.y * chunk_size) };

            int sampler0_location = glGetUniformLocation(chunk_program, "ourTexture");
            int sampler1_location = glGetUniformLocation(chunk_program, "tileTexture"); 

            glUniform1i(sampler0_location, 0); 
            glUniform1i(sampler1_location, 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tiles_sheet.texture);
           
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, current_chunk->tileTexture);
            
            glActiveTexture(GL_TEXTURE0);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(chunk_physical.x, chunk_physical.y, 0.0f));
            model = glm::scale(model, glm::vec3(chunk_size / 2, chunk_size / 2, 0.0f));
            model = glm::translate(model, glm::vec3(1.0f, 1.0f, 0.0f));

            afx::setConstant(chunk_program, "model", model);
            afx::setConstant(chunk_program, "view", view);
            afx::setConstant(chunk_program, "projection", projection);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }

        glUseProgram(program);

        game.render_amount = 0;

        for(int i = 0; i < max_entity_count; i++)
        {
            Entity* entity = &game.entities[i];

            if(entity->flags == FLAG_NONE) continue;

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

                if(current_frame != NULL)
                {
                    entity_scale.x = current_frame->width;
                    entity_scale.y = current_frame->height;

                    glBindTexture(GL_TEXTURE_2D, current_frame->texture);
                }
            }
            else
            {
                Sprite* frame = &entity->sprite;

                entity_scale.x = frame->width;
                entity_scale.y = frame->height;

                glBindTexture(GL_TEXTURE_2D, frame->texture);
            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            int scale = 1;

            if(entity->character.velocity.x < 0)
            {
              scale = -1;
            }
            
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, glm::vec3(entity->position.x, entity->position.y, 0.0f));
            model = glm::scale(model, glm::vec3(entity_scale.x * scale, entity_scale.y, 1.0));

            afx::setConstant(program, "model", model);
            afx::setConstant(program, "view", view);
            afx::setConstant(program, "projection", projection);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

            glBlendFunc(GL_ONE, GL_ZERO);
        }

        //test
        afx::drawRectangle(draw_program, view, projection, 5, 5);

        afx::drawRectangle(draw_program, view, projection, 50, 5);

        afx::drawRectangle(draw_program, view, projection, 5, 50);

        afx::drawUI(draw_program, -580, 300);
        afx::drawUI(draw_program, -470, 300);

        float current_time = SDL_GetTicks();

        game.delta_time = (current_time - previous_time) / 1000.0f;

        previous_time = current_time;
        
        if(game.player != NULL) 
        {
            float target_x = game.player->position.x;
            float target_y = game.player->position.y;

            camera.pos.x = lerp(camera.pos.x, target_x, 0.02);
            camera.pos.y = lerp(camera.pos.y, target_y, 0.02);
        }

        SDL_GL_SwapWindow(game.window.window);
    }
    
    SDL_GL_DeleteContext(game.window.context);
    SDL_DestroyWindow(game.window.window);
}