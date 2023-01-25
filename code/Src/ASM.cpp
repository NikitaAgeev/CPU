#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Onegin_hed.h"
#include "ASM.h"

const int ASM_VER = 1;
const char* ASM_CODE =  "NA";

const int num_of_registers = 4;


static void make_meta_data (FILE* output_file, size_t bin_len);

static int param_interpritator (string comand_string, char* pointer, int param_type, size_t* bin_mass_len);

static int comand_interpritator (string comand_string);

static int compile (Text code, char* bin_mass, size_t* bin_mass_len);


static int it_is_i (char* str);

static int it_is_reg (char* str);

static int it_is_operation (char* str);

static int take_reg_num (char* str);


int main (int carg, char* varg[])
{
    if(carg < 3)
    {
        printf("ERROR: too few arguments\n");
        return 0;
    }
    else if(carg > 3)
    {
        printf("ERROR: too many arguments\n");
        return 0;
    }

    FILE* code_file = fopen(varg[1], "r");

    if(code_file == nullptr)
    {
        printf("ERROR: Can't open: %s\n", varg[1]);
        return 0;
    }

    Text code = {};

    text_reader(&code, code_file);
 
    fclose(code_file);


    char* bin_mass = (char*)calloc(code.str_count * 9, sizeof(char));
    size_t bin_mass_len = 0;
    
    //debug_text_print(&code);

    int error_code = compile(code, bin_mass, &bin_mass_len);

    if(error_code == ERROR_COMAND)
    {
        printf("Error entering the command\n");
        return 0;
    }
    else if(error_code == ERROR_PARAM)
    {
        printf("Error entering the command parameter\n");
        return 0;
    }

    FILE* bin_file = fopen(varg[2], "w");

    if(bin_file == nullptr)
    {
        printf("Can't open: %s\n", varg[2]);
    }


    make_meta_data(bin_file, bin_mass_len);
    fwrite(bin_mass, sizeof(char), bin_mass_len, bin_file);

    fclose(bin_file);

    free(bin_mass);

    free(code.txt);
    free(code.str_mass); 
    
    return 0;
}


static void make_meta_data (FILE* output_file, size_t bin_len)
{
    fwrite(ASM_CODE, sizeof(char), strlen(ASM_CODE), output_file);
    fwrite(&ASM_VER, sizeof(int), 1, output_file);
    fwrite(&bin_len, sizeof(size_t), 1, output_file);
}

static int param_interpritator (string comand_string, char* pointer, int param_type, size_t* bin_mass_len )
{
    *bin_mass_len += sizeof(char);

    if(param_type & NO_PARAM)
    {
        size_t after_comand_pos = 0;
        sscanf(comand_string.str, "%*s%ln", (ssize_t*)&after_comand_pos);
        
        if(after_comand_pos == comand_string.len) return OK_PARAM;
        else return ERROR_PAR;
    }
    else if(param_type & ONE_PARAM)
    {
        int param = 0;
        size_t after_comand_pos = 0;

        int num_of_read_param = sscanf(comand_string.str, "%*s %d%ln", &param, (ssize_t*)&after_comand_pos);

        if((num_of_read_param == 1) && (after_comand_pos == comand_string.len)) 
        {
            *((int*)(pointer + sizeof(char))) = param;
            *bin_mass_len += sizeof(int);

            return OK_PARAM;
        }
        
         return ERROR_PAR;
    }
    else if(param_type & (IN_MEM_PARAM | OUT_MEM_PARAM))
    {
        int zero_param_len   = 0;
        int first_param_len  = 0;
        int second_param_len = 0;

        char param_1[100] = {};
        char param_2[100] = {};

        char operation[100] = {};

        //lrgal comand string combinations for IN_MEM_PARAM
        //COMAND REG/INT    
        //COMAND REG +- INT
        //COMAND INT + REG
        //lrgal comand string combinations for OUT_MEM_PARAM
        //COMAND     
        //COMAND REG

        int num_of_read_param = sscanf(comand_string.str, "%*s%N %[rabcdx0-9]%n %[+-] %[rabcdx0-9]%n",
                                       &zero_param_len, param_1, &first_param_len, operation, param_2, &second_param_len);

        
        //printf("from: \"%s\"(len: %d)\n  param_1: \"%s\"\n   param_2: \"%s\"\n   operator: \"%s\"\n  num_of_read_param: %d\n first_param_len: %d\n second_param_len: %d\n", comand_string.str, comand_string.len,  param_1, param_2, operation, num_of_read_param, first_param_len, second_param_len);


        if((num_of_read_param == 0) && (zero_param_len == comand_string.len) && OUT_MEM_PARAM)
        {
            return OK_PARAM;
        }
        
        //COMAND A
        if((num_of_read_param == 1) && (first_param_len == comand_string.len))
        {
            if(it_is_reg(param_1))
            {
                *pointer |= REGISTER_KEY;
                *(int*)(pointer + sizeof(char)) = take_reg_num(param_1);
                *bin_mass_len += sizeof(int);

                return OK_PARAM;
            }
            else if (it_is_i(param_1) && !(param_type & OUT_MEM_PARAM))
            {                
                *pointer |= INT_NUM_KEY;
                *(int*)(pointer + sizeof(char)) = atoi(param_1);
                *bin_mass_len += sizeof(int);

                return OK_PARAM;
            }

            printf("ERROR\n");
            return ERROR_PAR;
        }

        //COMAND A +- B
        if((num_of_read_param == 3) && (second_param_len == comand_string.len) && !(param_type & OUT_MEM_PARAM))
        {
            if(!it_is_operation(operation))
                return ERROR_PAR;

            if(it_is_reg(param_1) && it_is_i(param_2))
            {
                *pointer |= INT_NUM_KEY;
                *pointer |= REGISTER_KEY;
                
                *(int*)(pointer + sizeof(char)) = take_reg_num(param_1);
                if(*operation == '+')
                    *(int*)(pointer + sizeof(char) + sizeof(int)) = atoi(param_2);
                else
                    *(int*)(pointer + sizeof(char) + sizeof(int)) = -atoi(param_2);

                *bin_mass_len += sizeof(int)*2;

                return OK_PARAM;
            }

            if(it_is_i(param_1) && (*operation == '+') && it_is_reg(param_2))
            {
                *pointer |= INT_NUM_KEY;
                *pointer |= REGISTER_KEY;
                
                *(int*)(pointer + sizeof(char)) = take_reg_num(param_2);
                *(int*)(pointer + sizeof(char) + sizeof(int)) = atoi(param_1);
                
                *bin_mass_len += sizeof(int)*2;

                return OK_PARAM;
            }

            printf("ERROR\n");
            return ERROR_PAR;
        }

        printf("ERROR\n");
        return ERROR_PAR;
    
    }

    return OK_PARAM;
}

static int it_is_i (char* str)
{
    int int_param = 0;
    int read_len = 0;
    int read_num = sscanf(str, "%d%n", &int_param, &read_len);
    
    if((read_num == 1) && (read_len == strlen(str)))
        return 1;

    return 0;
}

static int it_is_reg (char* str)
{
    return (strlen(str) == 3) && (str[0] == 'r') && (str[2] == 'x') && ((str[1] - 'a') >= 0) && ((str[1] - 'a') < num_of_registers); 
}

static int it_is_operation (char* str)
{
    if(strlen(str) == 1)
        return 1;
    
    return 0;
}

static int take_reg_num (char* str)
{
    return str[1] - 'a';
}


#define INTERPRITATOR_UNIT(COMAND)                          \
if(!strncmp(comand_string.str, #COMAND, strlen(#COMAND)))   \
{                                                           \
    return COMAND;                                          \
}           


static int comand_interpritator (string comand_string)
{
    
    INTERPRITATOR_UNIT(POP)
    INTERPRITATOR_UNIT(PUSH)
    INTERPRITATOR_UNIT(ADD)
    INTERPRITATOR_UNIT(DEL)
    INTERPRITATOR_UNIT(MULL)
    INTERPRITATOR_UNIT(DIV)
    INTERPRITATOR_UNIT(OUT)
    INTERPRITATOR_UNIT(TEST)
    
    return ERROR_COM;
}

#undef INTERPRITATOR_UNIT

#define COMAND_CASE(COMAND, PARAM)                                                                              \
case COMAND:                                                                                                    \
bin_mass[*bin_mass_len] = COMAND;                                                                               \
                                                                                                                \
if(param_interpritator(code.str_mass[itterator], bin_mass + *bin_mass_len, PARAM, bin_mass_len) == ERROR_PAR)   \
return ERROR_PARAM;                                                                                             \
                                                                                                                \
break;                                                                                                          \

static int compile (Text code, char* bin_mass, size_t* bin_mass_len)
{
    size_t itterator = 0;

    for(itterator = 0; itterator < code.str_count; itterator ++)
    {
        if(code.str_mass[itterator].len == 0) continue;
        
        int comand = comand_interpritator(code.str_mass[itterator]);
        switch (comand)
        {
        COMAND_CASE(POP,  NO_PARAM)
        COMAND_CASE(PUSH, IN_MEM_PARAM)
        COMAND_CASE(ADD,  NO_PARAM)
        COMAND_CASE(DEL,  NO_PARAM)
        COMAND_CASE(MULL, NO_PARAM)
        COMAND_CASE(DIV,  NO_PARAM)
        COMAND_CASE(OUT,  NO_PARAM)
        COMAND_CASE(TEST, IN_MEM_PARAM)
        default:
            return ERROR_COMAND;
            break;
        }
    }

    return OK;
}

#undef COMAND_CASE