#include "headers.h"
enum status{ SUCCESS = 0, PROCESSING = 1};

/*
 * DON'T declare acc_value objects directly,
 * use create_acc_values() instead. 
 */

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
    union 
    {
        XYZAxis xyz_axis;
        float result;
    };
    bool is_result_avail;
};
typedef struct _acc_values Acc_values;

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
Acc_values retrive_pivot(int retrive_seconds);