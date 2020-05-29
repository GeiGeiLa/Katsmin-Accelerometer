#include "read_file.h"
#include <assert.h>
#include <stdbool.h>
const float SMALL_FALL_THRES_HIGH = 4.9f;
const float HIGH_FALL_THRRES_HIGH = 30.0f;
const float DECLINE_BEFORE_FALL_THRES_LOW = -0.5f;
const int WRITE_SIZE = 55;
const int SAMPLE_FREQ = 25;
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
Chunk* new_chunk(const unsigned char chunk_size)
{
    assert(chunk_size <= MAX_CHUNK_SIZE);
    Chunk* ck = (Chunk*)malloc(sizeof(Chunk));
    ck->size_limit = chunk_size;
    ck->current_size = 0;
    ck->last_index_ref = 0;
    for(int i = 0; i < chunk_size; i++)
    {
        ck->isValid[i] = false;
    }
    return ck;
}

bool chunk_append(Chunk* chunk, Acc_values acc)
{
    // not enough size
    if(chunk->current_size >= chunk->size_limit)
    {
        return false;
    }
    chunk->accs[chunk->current_size] = acc;
    chunk->isValid[chunk->current_size] = true;
    chunk->current_size++;
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

bool is_fall_simple_ver(Chunk* chunk, int thresh_hi, int thresh_lo, int interval)
{
    static int chunk_serial = 0;
    // easiest: hi
    // lose weight: lo
    // interval:
    int fall_index;
    int peak_index;
    int peak_left_down = -1;
    int peak_right_down = -1;
    int two_down_logger = 0;
    bool threlo,threhi;
    for(int onepower = 1;onepower >= -1; onepower -= 2)
    {
        peak_left_down = -1;
        peak_right_down = -1;
        for(int i = 0; i < chunk->current_size; i++)
        {
            if(onepower == 1)
            {
                threlo = chunk->accs[i].result >= thresh_lo;
            }
            else
            {
                threlo = chunk->accs[i].result <= thresh_lo * -1;
            }
            if(threlo)
            {
                if(peak_left_down == -1) peak_left_down = i;
                else peak_right_down = i;
            }
            if(peak_right_down - peak_left_down <= interval)
            {
                if(peak_left_down != -1 && peak_right_down != -1)
                {
                    printf("%d\n",peak_left_down+chunk->current_size*chunk_serial);
                    return true;
                }
            }
            else
            {
                peak_left_down = peak_right_down;
                peak_right_down = -1;
            }
//            // 如果已經在紀錄尖峰值的左點，紀錄
//            if(peak_left_down != -1)
//            {
//                two_down_logger++;
//            }
        }
    }
    chunk_serial++;
    return false;
}
XYZAxis find_offset(Chunk* chunk)
{
    float x = 0; float y = 0; float z = 0;
    // need at least data within 1 sec
    assert(chunk->current_size <= chunk->size_limit);
    for(int i = 0; i < chunk->current_size; i++)
    {
        x += chunk->accs[i].xyz_axis.x;
        y += chunk->accs[i].xyz_axis.y;
        z += chunk->accs[i].xyz_axis.z;
    }
    x /= chunk->current_size; y /= chunk->current_size; z /= chunk->current_size;
    XYZAxis xyz= {.x = x, .y = y, .z = z};
    return xyz;

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

void smooth_chunk(Chunk* current, const int smooth_window_size)
{
    assert(smooth_window_size >= 0);
    assert(smooth_window_size <= current->current_size);
    int sumX = 0;
    int sumY = 0;
    int sumZ = 0;
    int smooth_times = current->current_size / smooth_window_size ;
    int last_smooth_times = current->current_size % smooth_window_size;

    for(int t = 0; t < smooth_times; t++)
    {
        for(int i = 0; i < smooth_window_size; i++)
        {
            sumX += current->accs[smooth_times*t + i].xyz_axis.x;
            sumY += current->accs[smooth_times*t + i].xyz_axis.y;
            sumZ += current->accs[smooth_times*t + i].xyz_axis.z;
        }
        sumX /= smooth_window_size;
        sumY /= smooth_window_size;
        sumZ /= smooth_window_size;
        for (int i = 0; i < smooth_window_size; i++)
        {
            current->accs[smooth_times*t + i].xyz_axis.x = sumX;
            current->accs[smooth_times*t + i].xyz_axis.y = sumY;
            current->accs[smooth_times*t + i].xyz_axis.z = sumZ;
        }
    }
    // 後面多出來不足window size的 frame
    if(last_smooth_times > 0)
    {
        for(int l = 0; l < last_smooth_times; l++)
        {
            sumX += current->accs[smooth_times*smooth_window_size + l].xyz_axis.x;
            sumY += current->accs[smooth_times*smooth_window_size + l].xyz_axis.y;
            sumZ += current->accs[smooth_times*smooth_window_size + l].xyz_axis.z;
        }
        sumX /= last_smooth_times;
        sumY /= last_smooth_times;
        sumZ /= last_smooth_times;
        for (int i = 0; i < last_smooth_times; i++)
        {
            current->accs[smooth_times*smooth_window_size + i].xyz_axis.x = sumX;
            current->accs[smooth_times*smooth_window_size + i].xyz_axis.y = sumY;
            current->accs[smooth_times*smooth_window_size + i].xyz_axis.z = sumZ;
        }

    }



}


void sput_single_acc(Acc_values* const acc, char* cstring)
{
    snprintf(cstring, WRITE_SIZE ,"%.8f%.8f%.8f\n",
             acc->xyz_axis.x,
             acc->xyz_axis.y,
             acc->xyz_axis.z);
}
void fput_single_acc(Acc_values* const acc, FILE* const fileptr)
{
    char buf[WRITE_SIZE];
    snprintf(buf, WRITE_SIZE, "%.8f,%.8f,%.8f\n",
             acc->xyz_axis.x, acc->xyz_axis.y, acc->xyz_axis.z);
    fputs(buf,fileptr);
}

void init_cur_status(Current_State* cstate)
{
    cstate->axis_orientation = NORMAL;
    cstate->fall_times = 0;
}

void generate_vector_sum_(Chunk* chunk, XYZAxis* offset)
{
    XYZAxis* xyz = NULL;
    float x,y,z;
    for(int i = 0; i < chunk->current_size; i++)
    {
        xyz = &chunk->accs[i].xyz_axis;
        x = xyz->x;
        x -= offset->x;
        y = xyz->y;
        y -= offset->y;
        z = xyz->z;
        z -= offset->z;
        chunk->accs[i].is_result_avail = true;
        chunk->accs[i].result = x + y + z;
    }
}
