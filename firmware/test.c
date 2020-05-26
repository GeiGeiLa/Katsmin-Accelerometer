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
const int CHUNK_SIZE = 5;
int main()
{
    init_cur_status(&current_state);
    puts("hi");
    FILE* input = fopen("fall.csv","r");
    FILE* output = fopen("outfall.csv","w");
    if(input == NULL)
    {
        printf("Error while reading file\n");
        system("pause");
        return -1;
    }
    puts("open success");
    int i = 0;
    Chunk theChunk;
    bool chunk_succeed = false;
    while(fgets(buf, BUFFER_SIZE, input))
    {
        // prevent buffer overflow
        buf[BUF_SIZE-1]='\0';
        if(i == 0)
        {
            theChunk = create_chunk(5);
        }
        acc_temp = create_Acc_obj_fromstr(buf);
        chunk_succeed = chunk_append(&theChunk,acc_temp);
        if(!chunk_succeed)
        {

        }
        if(++i == 5)
        {
            i = 0;
        }
    }
    return 0;
}
Acc_values create_Acc_obj_fromstr(char* const linestr)
{
    float x,y,z;
    // prevent buffer overflow
    linestr[BUFFER_SIZE-1] = '\0';
    Acc_values acc = create_acc_values();
    sscanf(linestr,"%f,%f%f",&acc.xyz_axis.x,&acc.xyz_axis.y,&acc.xyz_axis.z);
    printf("%.8f__%.8f__%.8f__\n",acc.xyz_axis.x, acc.xyz_axis.y, acc.xyz_axis.z);
    // strtok 不知道為什會把小數點當成delimiter
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
