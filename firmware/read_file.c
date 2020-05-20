#include "read_file.h"
#include <assert.h>
#include <stdbool.h>

Chunk create_chunk(unsigned char chunk_size)
{
    assert(chunk_size <= MAX_CHUNK_SIZE);
    Chunk ck;
    ck.size_limit = chunk_size;
    ck.current_size = 0;
    ck.last_index_ref = 0;
    for(int i = 0; i < MAX_CHUNK_SIZE; i++)
    {
        ck.isValid[i] = false;
    }
    return ck;
}

bool chunk_append(Chunk* chunk, Acc_values acc)
{
    // not enough size
    if(chunk->current_size == chunk->size_limit)
    {
        return false;
    }
    chunk->accs[chunk->current_size] = acc;
    chunk->isValid[chunk->current_size++] = true;
    return true;
}
void smooth_chunk(Chunk* current)
{
    int sumX = 0;
    int sumY = 0;
    int sumZ = 0;
    for(int i = 0; i < current->current_size; i++)
    {
        sumX += current->accs[i].xyz_axis.x;
        sumY += current->accs[i].xyz_axis.y;
        sumZ += current->accs[i].xyz_axis.z;
    }
    sumX /= current->current_size;
    sumY /= current->current_size;
    sumZ /= current->current_size;
    for (int i = 0; i < current->current_size; i++)
    {
        current->accs[i].xyz_axis.x = sumX;
        current->accs[i].xyz_axis.y = sumY;
        current->accs[i].xyz_axis.z = sumZ;
    }