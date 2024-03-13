#include "mathe.h"
#include "chunk.h"
#include "math.h"

V2i get_chunk_index(float x, float y)
{
    V2i chunk_index;

    chunk_index.x = (int) floorf(x / chunk_size);
    chunk_index.y = (int) floorf(y / chunk_size);

    return chunk_index;
}

Chunk* lookup_chunk(V2i chunk_index, int num_chunks, Chunk* chunks_array)
{
    for(int i = 0; i < num_chunks; i++)
    {
        Chunk* current_chunk = &chunks_array[i];
        
        if((current_chunk->index.x == chunk_index.x) && (current_chunk->index.y == chunk_index.y))
        {
            if(current_chunk->exists) return current_chunk;
        }
    }

    return NULL;
}

Chunk* find_free_chunk_slot(int num_chunks, Chunk* chunks_array)
{
    for(int i = 0; i < num_chunks; i++)
    {
        Chunk* current_chunk = &chunks_array[i];

        if(current_chunk->exists == false)
        {
            return current_chunk;
        }
    }

    return NULL;
}