#include "read_file.h"
#include <assert.h>
#include <stdbool.h>

const float SMALL_FALL_THRES_HIGH = 4.9f;
const float HIGH_FALL_THRRES_HIGH = 30.0f;
const float DECLINE_BEFORE_FALL_THRES_LOW = -0.5f;

Acc_values create_acc_values()
{
    Acc_values acc;
    acc.is_result_avail = false;
    acc.result = 0;
    acc.xyz_axis.x = 0;
    acc.xyz_axis.y = 0;
    acc.xyz_axis.z = 0;
    return acc;
}

// 要想辦法判斷是鄭很大痕是負很大
Chunk create_chunk(unsigned char chunk_size)
{
    assert(chunk_size <= MAX_CHUNK_SIZE);
    Chunk ck;
    ck.size_limit = chunk_size;
    ck.current_size = 0;
    ck.last_index_ref = 0;
    for(int i = 0; i < size_limit; i++)
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
bool chunk_append_withbuffer(Chunk* chunk,float xc, float yc, float zc)
{
    // not enough size
    if(chunk->current_size == chunk->size_limit)
    {
        return false;
    }
    chunk->accs[chunk->current_size].xyz_axis.x = xc;
    chunk->accs[chunk->current_size].xyz_axis.y = yc;
    chunk->accs[chunk->current_size].xyz_axis.z = zc;
    chunk->isValid[chunk->current_size++] = true;
    return true;
};

bool is_fall_simple_ver(int* target_buf, int thresh_hi, int thresh_lo, int interval)
{
    // easiest: hi
    // lose weight: lo
    // interval:
    int fall_index;
    int peak_index;
    for(int i = 0; i < MAX_CHUNK_SIZE; i++)
    {
        if(target_buf[i] <= thresh_lo && thresh_lo != -1)
        {

        }
        else
        {
            if(target_buf[i] >= thresh_hi)
            {

            }
            if(thresh_hi - thresh_lo <= interval)
            {
                return true;
            }
        }
    }
}
XYZAxis find_offset(Chunk* chunk)
{
    float x = 0; float y = 0; float z = 0;
    // need at least data within 1 sec
    assert(chunk->current_size <= chunk->size_limit);
    assert(chunk->current_size >= SAMPLE_FREQ);
    for(int i = 0; i < SAMPLE_FREQ; i++)
    {
        x += chunk->accs[i].xyz_axis.x;
        y += chunk->accs[i].xyz_axis.y;
        z += chunk->accs[i].xyz_axis.z;
    }
    x /= SAMPLE_FREQ; y /= SAMPLE_FREQ; z /= SAMPLE_FREQ;
}
// 扣掉 offset 來歸零
void fix_chunk_values(Chunk* current, XYZAxis* offsets_xyz)
{

    float* xptr, *yptr, *zptr;
    for(int i = 0; i < current->current_size; i++)
    {
        // set pointers for data because var name is too long
        xptr = &current->accs[i].xyz_axis.x;
        yptr = &current->accs[i].xyz_axis.y;
        zptr = &current->accs[i].xyz_axis.z;
        *xptr -= offsets_xyz->x;
        *yptr -= offsets_xyz->y;
        *zptr -= offsets_xyz->z;
    }
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
}

void init_cur_status(Current_State* cstate)
{
    cstate->axis_orientation = NORMAL;
    cstate->fall_times = 0;
}
