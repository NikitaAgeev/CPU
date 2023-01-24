#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Stack.h"
#include "ASM.h"

STACK_LOG_OPEN(NULL);

enum ERROR_S
{
    OK    = 0,
    ERROR = 1,
};

const int CPU_VER = 1;
const char* CPU_CODE = "NA";

const size_t meta_data_len = sizeof(char)*strlen(CPU_CODE) + sizeof(int) + sizeof(size_t);  


static int comand_interpritator (char* bin_code, size_t* i_ptr, size_t code_len, Stack stack);

static int take_func_param (char* bin_code, size_t i_ptr, size_t code_len, int param_mask, Stack stack);

static int skip_param (char* bin_code, size_t i_ptr, size_t code_len, int param_mask, Stack stack);

int main (int carg, char* varg[])
{
    if(carg != 2)
    {
        printf("ERROR: too few arguments\n");
        return 0;
    }

    FILE* binar_file = fopen(varg[1], "r");
    if(binar_file == nullptr)
    {
        printf("ERROR: Can't open: %s\n", varg[1]);
        return 0;
    }
    
    char* meta_data_bufer= (char*)calloc( meta_data_len, sizeof(char));;

    fread(meta_data_bufer, sizeof(char), meta_data_len, binar_file);

    if(strncmp(meta_data_bufer, CPU_CODE, strlen(CPU_CODE)))
    {
        printf("ERROR: Bed file\n");
        return 0;
    }

    if(strncmp(meta_data_bufer + strlen(CPU_CODE), (char*)&CPU_VER, sizeof(int)))
    {
        printf("ERROR: Bed version\n");
        return 0;
    }

    size_t bin_code_len = 0;
    bin_code_len = *(size_t*)(meta_data_bufer + strlen(CPU_CODE) + sizeof(int));
    char* bin_code = (char*)calloc(bin_code_len, sizeof(char));

    free(meta_data_bufer);

    fread(bin_code, sizeof(char), bin_code_len, binar_file);
    fclose(binar_file);

    Stack stack = nullptr;
    STACK_CTOR(stack);

    size_t i_ptr = 0;

    while(i_ptr < bin_code_len)
    {
        comand_interpritator(bin_code, &i_ptr, bin_code_len, stack);
    }

    free(bin_code);

    STACK_DTOR(stack);
    STACK_LOG_CLOSE();
}


#define TAKE_PARAM(PARAM_MASK)                                                  \
param  = take_func_param(bin_code, *i_ptr_ptr, code_len, PARAM_MASK, stack);    \

#define SKIP_PARAM(PARAM_MASK)                                                  \
*i_ptr_ptr += skip_param(bin_code, *i_ptr_ptr, code_len, PARAM_MASK, stack);    \

#define END_CPU         \
    free(bin_code);     \
    STACK_DTOR(stack);  \
    STACK_LOG_CLOSE();  \
    abort();            \

static int comand_interpritator (char* bin_code, size_t* i_ptr_ptr, size_t code_len, Stack stack)
{
    (void)code_len;
    
    size_t i_ptr = *i_ptr_ptr;
    int param = 0;

    switch (bin_code[i_ptr++])
    {
    case POP:
        TAKE_PARAM(NO_PARAM);
        
        POP(stack);
        
        SKIP_PARAM(NO_PARAM);
        break;
    case PUSH:
        TAKE_PARAM(ONE_PARAM);

        PUSH(stack, param);
        
        SKIP_PARAM(ONE_PARAM);
        break;
    case ADD:
        TAKE_PARAM(NO_PARAM);

        PUSH(stack, POP(stack) + POP(stack));
        
        SKIP_PARAM(NO_PARAM);
        break;
    case DEL:
        TAKE_PARAM(NO_PARAM);

        PUSH(stack, POP(stack) - POP(stack));
        
        SKIP_PARAM(NO_PARAM);
        break;
    case MULL:
        TAKE_PARAM(NO_PARAM);

        PUSH(stack, POP(stack)*POP(stack));
        
        SKIP_PARAM(NO_PARAM);
        break;
    case DIV:
        TAKE_PARAM(NO_PARAM);

        PUSH(stack, POP(stack)/POP(stack));
        
        SKIP_PARAM(NO_PARAM);
        break;
    case OUT:
        TAKE_PARAM(NO_PARAM);

        printf("OUT: %d\n", POP(stack));
        
        SKIP_PARAM(NO_PARAM);
        break;

    default:
        printf("Unknown command\n");
        END_CPU;
    }

    return OK;
}

static int take_func_param (char* bin_code, size_t i_ptr, size_t code_len, int param_mask, Stack stack)
{
    if(param_mask == ONE_PARAM)
    {
        if((code_len - i_ptr) < (sizeof(char) + sizeof(int)))
        {
            printf("ERROR: Going beyond the code array.\n");
            END_CPU;
        }

        return *(int*)(bin_code + i_ptr + sizeof(char));
    }
    
    if(param_mask == NO_PARAM)
    {
        if(i_ptr > code_len)
        {
            printf("ERROR: Going beyond the code array.\n");
            END_CPU;
        }

        return 0;
    }

    printf("ERROR: Bed param mask\n");

    END_CPU;
}


static int skip_param (char* bin_code, size_t i_ptr, size_t code_len, int param_mask, Stack stack)
{
    (void)i_ptr;
    (void)code_len;
    
    if(param_mask == NO_PARAM)
        return sizeof(char);
    
    if(param_mask == ONE_PARAM)
        return sizeof(char) + sizeof(int);

    printf("ERROR: Bed param mask\n");

    END_CPU;
}