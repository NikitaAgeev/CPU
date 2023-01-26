#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Onegin_hed.h"
#include "ASM.h"

const int ASM_VER = 3;
const char* ASM_CODE =  "NA";

const int num_of_registers = 4;


static void make_meta_data (FILE* output_file, size_t bin_len);

static int param_interpritator (string comand_string, char* pointer, int param_type, size_t* bin_mass_len);

static int comand_interpritator (string comand_string);

static int compile (Text code, char* bin_mass, size_t* bin_mass_len);


static int it_is_i (char* str);

static int it_is_reg (char* str);

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


#define BED_ARG                                                 \
{                                                               \
    printf("ERROR: bed arg in: \"%s\"\n", comand_string.str);   \
    return ERROR_PAR;                                           \
}                                                               

static int param_interpritator (string comand_string, char* pointer, int param_type, size_t* bin_mass_len )
{
    *bin_mass_len += sizeof(char);

    if(param_type & NO_PARAM)
    {
        size_t after_comand_pos = 0;
        sscanf(comand_string.str, "%*s%ln", (ssize_t*)&after_comand_pos);
        
        if(after_comand_pos == comand_string.len)
            return OK_PARAM;
        else
            BED_ARG
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
        
        BED_ARG
    }
    else if(param_type & (IN_MEM_PARAM | OUT_MEM_PARAM))
    {
        /*
        lrgal comand string combinations for IN_MEM_PARAM
        a.1.1 COMAND REG/INT    
        a.1.2 COMAND REG +- INT
        a.1.3 COMAND INT + REG
        a.2.1 COMAND [REG/INT]
        a.2.2 COMAND [REG +- INT]
        a.2.3 COMAND [INT + REG]
        lrgal comand string combinations for OUT_MEM_PARAM
        b.1.1 COMAND     
        b.1.2 COMAND REG
        b.2.1 COMAND [REG/INT]
        b.2.2 COMAND [REG +- INT]
        b.2.3 COMAND [INT + REG]
        */

        *pointer |= MEM_KEY_MASK;

        //b.1.1

        int com_len = 0;
        char end_char = 0;
        int num_to_var = 0;

        int readed_n = sscanf(comand_string.str, "%*s %n%c %n", &com_len, &end_char, &num_to_var);

        if( ((readed_n <= 0) && (com_len  == (int)comand_string.len)) || //b.1.1 -- no coment
            ((readed_n == 1) && (end_char == '%')))                 //b.1.1 -- coment                
            return OK_PARAM;            
        else if(readed_n == 0)
            BED_ARG
        
        //RAM_checker
        int RAM_flag = 0;

        if(end_char == '[')
        {
            RAM_flag = 1;
            *pointer |= RAM_KEY;
        }
        else 
            num_to_var = com_len;
         

        //a.1/2.1 && b.1/2.2
        char param_1[100] = {};
        int param_1_len = 0;
        int param_1_after = 0;
        readed_n = sscanf(comand_string.str + num_to_var, "%[rabcdx0-9] %n%c %n", param_1, &param_1_len, &end_char, &param_1_after);

        //we need read arg
        if(readed_n <= 0)
            BED_ARG

        //readed_n > 0
        if(it_is_reg(param_1))
        {
            *pointer |= REGISTER_KEY;
            *(int*)(pointer + sizeof(char)) = take_reg_num(param_1);
            *bin_mass_len += sizeof(int);
        }
        else if(it_is_i(param_1))
        {    
            *pointer |= INT_NUM_KEY;
            *(int*)(pointer + sizeof(char)) = atoi(param_1);
            *bin_mass_len += sizeof(int);
        }
        else
            BED_ARG   //don't legal param_1

        //a.1.1 && b.1.2 only no_coment no_ram
        if(readed_n == 1)
        {
            if(((param_1_len + num_to_var) == (int)comand_string.len) && !RAM_flag && (!(param_type == OUT_MEM_PARAM) || it_is_reg(param_1)))
                return OK_PARAM;

            BED_ARG
        }
        
        if(readed_n == 2)
        {
            //a.1.1 && b.1.2 coment
            if(end_char == '%') // only at the end of the command
            {
                if(!RAM_flag && (!(param_type == OUT_MEM_PARAM) || it_is_reg(param_1)))
                    return OK_PARAM;
                
                BED_ARG
            }

            //b.2.1 a.2.1
            if(end_char == ']') // only at the end of the command
            {
                if(RAM_flag && (((param_1_after + num_to_var) == (int)comand_string.len) || (comand_string.str[param_1_after + num_to_var] == '%')))
                    return OK_PARAM;

                BED_ARG
            }
        }

        //a.1/2.2/3 b.2.2/3
        int operation = 0;

        if(end_char == '+')
            operation = 1;
        else if(end_char == '-')
            operation = -1;
        else
            BED_ARG //no another operation

        char param_2[100] = {};
        int param_2_len = 0;
        int param_2_after = 0;
        readed_n = sscanf(comand_string.str + num_to_var + param_1_after, "%[rabcdx0-9] %n%c %n", param_2, &param_2_len, &end_char, &param_2_after);

        if(readed_n == 0)
            BED_ARG

        if((it_is_reg(param_2)) && (operation > 0) && !(*pointer & REGISTER_KEY))
            {
                *pointer |= REGISTER_KEY;
                *(int*)(pointer + sizeof(char)) = take_reg_num(param_2);
                *bin_mass_len += sizeof(int);
            }
        else if(it_is_i(param_2) && !(*pointer & INT_NUM_KEY))
            {
                *pointer |= INT_NUM_KEY;
                *(int*)(pointer + sizeof(char)) = atoi(param_2);
                *bin_mass_len += sizeof(int);
            }
        else
            BED_ARG

        //a.1.2 && a.1.3 only no_coment no_ram
        if(readed_n == 1)
        {
            if(((param_1_after + param_2_len + num_to_var) == (int)comand_string.len) && !RAM_flag)
                return OK_PARAM;

            BED_ARG
        }
        
        if(readed_n == 2)
        {
            //a.1.2 && a.1.3 coment
            if(end_char == '%') // only at the end of the command
            {
                if(!RAM_flag)
                    return OK_PARAM;
                
                BED_ARG
            }

            //a/b.2.2/3
            if(end_char == ']') // only at the end of the command
            {
                if(RAM_flag && (((param_1_after + param_2_after + num_to_var) == (int)comand_string.len) || (comand_string.str[param_1_after + param_2_after + num_to_var] == '%')))
                    return OK_PARAM;

                BED_ARG
            }
        }

        BED_ARG;
    
    }

    return OK_PARAM;
}


static int it_is_i (char* str)
{
    int int_param = 0;
    int read_len = 0;
    int read_num = sscanf(str, "%d%n", &int_param, &read_len);
    
    if((read_num == 1) && (read_len == (int)strlen(str)))
        return 1;

    return 0;
}

static int it_is_reg (char* str)
{
    return (strlen(str) == 3) && (str[0] == 'r') && (str[2] == 'x') && ((str[1] - 'a') >= 0) && ((str[1] - 'a') < num_of_registers); 
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
    if(*comand_string.str == '%')
    {
        return COMENT;
    }

    INTERPRITATOR_UNIT(POP)
    INTERPRITATOR_UNIT(PUSH)
    INTERPRITATOR_UNIT(ADD)
    INTERPRITATOR_UNIT(DEL)
    INTERPRITATOR_UNIT(MULL)
    INTERPRITATOR_UNIT(DIV)
    INTERPRITATOR_UNIT(OUT)
    
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
        COMAND_CASE(POP,  OUT_MEM_PARAM)
        COMAND_CASE(PUSH, IN_MEM_PARAM)
        COMAND_CASE(ADD,  NO_PARAM)
        COMAND_CASE(DEL,  NO_PARAM)
        COMAND_CASE(MULL, NO_PARAM)
        COMAND_CASE(DIV,  NO_PARAM)
        COMAND_CASE(OUT,  NO_PARAM)

        case COMENT:
            break;

        default:
            return ERROR_COMAND;
            break;
        }
    }

    return OK;
}

#undef COMAND_CASE