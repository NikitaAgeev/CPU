#ifndef ASM
#define ASM

enum PARAM_TYPE
{
    NO_PARAM      = 1 << 0,
    ONE_PARAM     = 1 << 1,
    IN_MEM_PARAM  = 1 << 2,
    OUT_MEM_PARAM = 1 << 3,
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


#define COMAND(NAME, BIN_CODE, PARAM_TYPE, CODE)            \
    NAME = BIN_CODE,

enum COMAND_CODE
{
    ERROR_COM    = 0x00,
    COMENT       = 0x7F,

    MEM_KEY_MASK = 0x10 << 3,
    RAM_KEY      = 0x10 << 2,
    REGISTER_KEY = 0x10 << 1,
    INT_NUM_KEY  = 0x10 << 0,

    #include <comands.h>

};

#undef COMAND

#endif
