#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

constexpr int tile_size = 16;
constexpr int chunk_tiles = 16;
constexpr int chunk_size =  tile_size * chunk_tiles;
constexpr int chunk_load = 4;

enum TileType
{
    tile_water,
    tile_dirt,
    tile_grass,
    tile_stone,
    tile_snow
};

struct Tile
{
    TileType type;
    float brightness;
};

struct Chunk
{
    std::uint8_t tiles[chunk_tiles * chunk_tiles];
    V2i index;
    bool exists = false;
    float noise;
};
//x + y * width for index in array of tile

V2i get_chunk_index(float x, float y);
Chunk* lookup_chunk(V2i chunk_index, int num_chunks, Chunk* chunks_array);
Chunk* find_free_chunk_slot(int num_chunks, Chunk* chunks_array);