#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Stack.h"
#include "ASM.h"
#include "math.h"


STACK_LOG_OPEN(NULL);

enum ERROR_S
{
    OK    = 0,
    ERROR = 1,
};


const size_t meta_data_len = sizeof(char)*strlen(ASM_CODE) + sizeof(int) + sizeof(size_t);  


static int comand_interpritator (char* bin_code, size_t* i_ptr, size_t code_len, Stack stack, int* register_mass, int* RAM, Stack ret_stack);

static int* take_func_param (char* bin_code, size_t* i_ptr, size_t code_len, int param_mask, Stack stack, int* register_mass, int* RAM, Stack ret_stack);

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

    if(strncmp(meta_data_bufer, ASM_CODE, strlen(ASM_CODE)))
    {
        printf("ERROR: Bed file\n");
        return 0;
    }

    if(strncmp(meta_data_bufer + strlen(ASM_CODE), (char*)&CPU_VER, sizeof(int)))
    {
        printf("ERROR: Bed version\n");
        return 0;
    }

    size_t bin_code_len = 0;
    bin_code_len = *(size_t*)(meta_data_bufer + strlen(ASM_CODE) + sizeof(int));
    char* bin_code = (char*)calloc(bin_code_len, sizeof(char));

    free(meta_data_bufer);

    fread(bin_code, sizeof(char), bin_code_len, binar_file);
    fclose(binar_file);

    Stack stack = nullptr;
    STACK_CTOR(stack);

    Stack ret_stack = nullptr;
    STACK_CTOR(ret_stack);

    size_t i_ptr = 0;

    int* register_mass = (int*)calloc(num_of_registers + 1, sizeof(arg_t));
    int* RAM_mass = (int*)calloc(RAM_size, sizeof(arg_t));

    while(i_ptr < bin_code_len)
    {
        comand_interpritator(bin_code, &i_ptr, bin_code_len, stack, register_mass, RAM_mass, ret_stack);
    }

    free(bin_code);
    free(register_mass);

    free(RAM_mass);

    STACK_DTOR(stack);
    STACK_DTOR(ret_stack);
    STACK_LOG_CLOSE();
}


#define TAKE_PARAM(PARAM_MASK)                                                                              \
par  = take_func_param(bin_code, i_ptr_ptr, code_len, PARAM_MASK, stack, register_mass, RAM, ret_stack);    \

#define param *par
#define i_ptr *i_ptr_ptr

#define pop POP(stack)
#define push(PARAM) PUSH(stack, PARAM)

#define pop_ret POP(ret_stack)
#define push_ret(PARAM) PUSH(ret_stack, PARAM)


#define END_CPU             \
    free(bin_code);         \
    free(register_mass);    \
    free(RAM);              \
    STACK_DTOR(stack);      \
    STACK_DTOR(ret_stack);  \
    STACK_LOG_CLOSE();      \
    abort();            

#define END_CASE(PARAM_MASK)                                \
    if(PARAM_MASK & (IN_MEM_PARAM | MARK_PARAM)) free(par);   

static inline unsigned char make_comand_case(unsigned char comand, int param_type)
{
    if(param_type & (RAM_KEY | REGISTER_KEY | IN_MEM_PARAM))
        return comand | MEM_KEY_MASK;
    
    return comand;
}

#define COMAND(NAME, BIN_CODE, PARAM_TYPE, CODE)    \
case NAME | ((PARAM_TYPE & (IN_MEM_PARAM | OUT_MEM_PARAM))? MEM_KEY_MASK: 0):            \
    TAKE_PARAM(PARAM_TYPE);                         \
                                                    \
    CODE                                            \
                                                    \
    END_CASE(PARAM_TYPE);                           \
    break;          

static int comand_interpritator (char* bin_code, size_t* i_ptr_ptr, size_t code_len, Stack stack, int* register_mass, int* RAM, Stack ret_stack)
{
    (void)code_len;
    
    int* par = NULL;

    unsigned char comand = bin_code[*i_ptr_ptr];

    if(comand & MEM_KEY_MASK)
        comand &= ~(RAM_KEY | REGISTER_KEY | INT_NUM_KEY);

    switch (comand)
    {
    
    #include <comands.h>

    default:
        printf("Unknown command: \"%.2X\"(%.2X) line: %lu\n", comand, bin_code[*i_ptr_ptr], *i_ptr_ptr);
        END_CPU;
    }
    
    return OK;
}

#undef COMAND

#define TAKE_INT *(arg_t*)(bin_code + *i_ptr_ptr)


#pragma GCC diagnostic ignored "-Wuse-after-free"


#define CHECK_INT_CODE_OVERFLOW                         \
if((code_len - *i_ptr_ptr) < sizeof(arg_t))               \
{                                                       \
    printf("ERROR: Going beyond the code array.\n");    \
    free(output);                                       \
    END_CPU;                                            \
}

#pragma GCC diagnostic warning "-Wuse-after-free"

#define CHECK_STACK_OVERFLOW                            \
if((adr_num > 0) && (adr_num < 0))                      \
{                                                       \
    printf("ERROR: stack overflow.\n");                 \
    END_CPU;                                            \
}                                                       


static int* take_func_param (char* bin_code, size_t* i_ptr_ptr, size_t code_len, int param_mask, Stack stack, int* register_mass, int* RAM, Stack ret_stack)
{
    char comand = bin_code[*i_ptr_ptr];
    *i_ptr_ptr += sizeof(comand_t);

    arg_t* output = (arg_t*)calloc(1, sizeof(arg_t));
    *output = 0;
    
    if(*i_ptr_ptr > code_len)
    {
        printf("ERROR: Going beyond the code array.\n");
        free(output);
        END_CPU;
    }

    if(param_mask == NO_PARAM)
    {
        free(output);
        return 0;
    }

    if(param_mask == IN_MEM_PARAM)
    {   
        if(comand & REGISTER_KEY)
        {
            CHECK_INT_CODE_OVERFLOW;

            *output += register_mass[TAKE_INT];
            *i_ptr_ptr += sizeof(arg_t);
        }

        if(comand & INT_NUM_KEY)
        {
            CHECK_INT_CODE_OVERFLOW;

            *output += TAKE_INT;
            *i_ptr_ptr += sizeof(arg_t);
        }

        if(comand & RAM_KEY)
        {
            int adr_num = *output;

            CHECK_STACK_OVERFLOW;

            *output += RAM[adr_num];
        }

        return output;
    }

    if(param_mask == OUT_MEM_PARAM)
    {
        free(output);
        
        if(comand & REGISTER_KEY)
        {
            CHECK_INT_CODE_OVERFLOW;
            
            output = register_mass + TAKE_INT;
            
            *i_ptr_ptr += sizeof(arg_t);

            if(!(comand & RAM_KEY))
                return output;
        }

        if(comand & RAM_KEY)
        {
            int adr_num = 0;
            if(comand & REGISTER_KEY)            
                adr_num = *output;
            
            if(comand & INT_NUM_KEY)
            {
                
                if((code_len - *i_ptr_ptr) < sizeof(arg_t))               
                {                                                       
                    printf("ERROR: Going beyond the code array.\n");    
                    END_CPU;                                            
                }

                adr_num += TAKE_INT;
                *i_ptr_ptr += sizeof(arg_t);
            }
            
            CHECK_STACK_OVERFLOW;

            output = RAM + adr_num;
            
            return output;
        }

        return register_mass + num_of_registers + 1;
    }

    if(param_mask == MARK_PARAM)
    {
        CHECK_INT_CODE_OVERFLOW;

        *output = TAKE_INT;
        *i_ptr_ptr += sizeof(arg_t);
        
        return output;
    }

    printf("ERROR: Bed param mask\n");

    END_CPU;
}
