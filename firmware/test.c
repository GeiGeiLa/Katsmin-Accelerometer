#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "headers.h"
#include "read_file.h"
#define BUF_SIZE 55

Acc_values create_Acc_obj_fromstr(char* const linestr);
Acc_values acc_temp;
const int BUFFER_SIZE = BUF_SIZE;
Current_State current_state;
char buf[BUF_SIZE];
const int SMOOTH_SIZE = 2;
int main()
{
    init_cur_status(&current_state);
    puts("hi");
    FILE* input = fopen("leftfall.csv","r");
    FILE* output = fopen("outfall.txt","w");
    if(input == NULL)
    {
        printf("Error while reading file\n");
        system("pause");
        return -1;
    }
    puts("open success");
    int i = 0;
    bool chunk_succeed = false;
    Chunk* chunkptr = new_chunk(SAMPLE_FREQ);
    bool pivot_found = false;
    XYZAxis offset;
    while(fgets(buf, BUFFER_SIZE, input))
    {

        if(i++ >= 500) ;
        // prevent buffer overflow
        buf[BUF_SIZE-1]='\0';
        acc_temp = create_Acc_obj_fromstr(buf);

        chunk_succeed = chunk_append(chunkptr,acc_temp);
        // failed because chunk full
        if(!chunk_succeed)
        {
            // only execute once for the first chunk to find pivot
            if(!pivot_found)
            {
                offset = find_offset(chunkptr);
                pivot_found = true;
            }
            fix_chunk_values(chunkptr, &offset);
            generate_vector_sum_(chunkptr, &offset);
            //smooth_chunk(chunkptr, SMOOTH_SIZE);
            for(int j = 0; j < chunkptr->current_size; j++)
            {
                //fput_single_acc(&(chunkptr->accs[j]), output);
            }
            if(is_fall_simple_ver(chunkptr, -1, 2.8f, 5))
            {
                fprintf(output,"boom\n");
            }
            free(chunkptr);
            chunkptr = new_chunk(SAMPLE_FREQ);
            chunk_append(chunkptr,acc_temp);
        }
    }
    free(chunkptr);
    fclose(input);
    fclose(output);
    return 0;
}
Acc_values create_Acc_obj_fromstr(char* const linestr)
{
    Acc_values acc;
    //float x,y,z;
    // prevent buffer overflow
    linestr[BUFFER_SIZE-1] = '\0';
    sscanf(linestr,"%f,%f,%f",&acc.xyz_axis.x,&acc.xyz_axis.y,&acc.xyz_axis.z);
//    printf("%.8f__%.8f__%.8f__\n",acc.xyz_axis.x, acc.xyz_axis.y, acc.xyz_axis.z);
    return acc;

}
// unused
void strtok_example(char* str, char* delimiters)
{
    if(str == NULL || strlen(str) == 0 || delimiters == NULL)
    {
        puts("str dummy");
        return;
    }
    char* token;
    for(token = strtok(str,delimiters); token != NULL; token = strtok(NULL, delimiters))
    {
        puts(token);
    }
}
