//COMAND(name, bin_code, param_type, code)

COMAND(POP, 0x01, OUT_MEM_PARAM,    \
{                                   \
    param = pop;                    \
}                                   \
)

COMAND(PUSH, 0x02, IN_MEM_PARAM,    \
{                                   \
    push(param);                    \
}                                   \
)

COMAND(ADD, 0x03, NO_PARAM,         \
{                                   \
    push(pop + pop);                \
}                                   \
)

COMAND(DEL, 0x04, NO_PARAM,         \
{                                   \
    push(pop/pop);                  \
}                                   \
)

COMAND(MULL, 0x05, NO_PARAM,        \
{                                   \
    push(pop*pop);                  \
}                                   \
)                                      

COMAND(DIV, 0x06, NO_PARAM,         \
{                                   \
    push(pop - pop);                \
}                                   \
)

COMAND(OUT, 0x07, NO_PARAM,         \
{                                   \
    printf("OUT: %d\n", pop);       \
}                                   \
)
                                
COMAND(JMP, 0x08, MARC_PARAM, {printf("yes\n");})