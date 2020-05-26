#ifndef _READ_FILE_H
#define _READ_FILE_H
#include <stdbool.h>
#include "headers.h"
enum calc_status{ SUCCESS = 0, PROCESSING = 1};
enum axis_status{ NORMAL = 0, INVERTED = 1 };
#define MAX_CHUNK_SIZE 5
#define SAMPLE_FREQ 25
#define BREATH_INTERVAL_SEC 15
// Struct definitions
struct _xyz
{
    float x;
    float y;
    float z;
};
typedef struct _xyz XYZAxis;
struct _acc_values
{
    XYZAxis xyz_axis;
    float result;
    bool is_result_avail;
};
typedef struct _acc_values Acc_values;

struct _chunk_acc
{
    Acc_values accs[MAX_CHUNK_SIZE];
    bool isValid[MAX_CHUNK_SIZE];
    unsigned char size_limit;
    unsigned char current_size;
    unsigned char last_index_ref;
};
typedef struct _chunk_acc Chunk;
/**
 * Create and initialize unsigned char properties
 *
 */
Chunk create_chunk(unsigned char chunk_size);

// append element to chunk
bool chunk_append(Chunk* chunk, Acc_values acc);

void delete_chunk_element(Chunk* chunk, unsigned char index);

void fix_chunk_values(Chunk* current, XYZAxis* offsets_xyz);
// average smooth algorithm to smooth
void smooth_chunk(Chunk* current);

/*
 * Create acc instances
 * -------------------------------------
 * returns: instance with default values
 */
Acc_values create_acc_values(void);
/*
 * Send BLE notification to Android App
 * ------------------------------------
 * message: any char*
 * returns: enum status
 */
int send_notification(const char* message);
/*
 * Find biases for each axis.
 * Acc must be steadily put in somewhere in
 * the first few seconds.
 * If failed (not steady), return an instance whose
 * property: is_result_avail
 * ------------------------------------
 * retrive_seconds: how many seconds we assume that
 * ACC is steadily placed.
 * return: object with biases for each axis
 */

struct _cur_stat
{
    unsigned char axis_orientation;
    unsigned int fall_times;
};
typedef struct _cur_stat Current_State;

void init_cur_status(Current_State* cstate);

XYZAxis find_offset(Chunk* chunk);

Chunk create_chunk(unsigned char chunk_size);

bool is_fall_simple_ver(int* target_buf, int thresh_hi, int thresh_lo, int interval);

bool chunk_append_withbuffer(Chunk* chunk,float xc, float yc, float zc);

bool chunk_append(Chunk* chunk, Acc_values acc);

#endif
