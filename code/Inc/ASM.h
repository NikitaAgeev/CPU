#ifndef ASM
#define ASM

enum PARAM_TYPE
{
    NO_PARAM  = 1 << 0,
    ONE_PARAM = 1 << 1,
};

enum PARAM_MASK
{
    ERROR_PAR = 1 << 0,
    OK_PARAM = 0,
};

enum COMPAIL_STATUS
{
    //OK = 0, conflict
    ERROR_COMAND = 1 << 0,
    ERROR_PARAM = 1 << 1   
};

enum COMAND_CODE
{
    ERROR_COM = 0x00,
    
    POP   = 0x01,
    PUSH  = 0x02,
    ADD   = 0x03,
    DEL   = 0x04,
    MULL  = 0x05,
    DIV   = 0x06,
    OUT   = 0x07,
};

#endif