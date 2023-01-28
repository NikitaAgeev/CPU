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
                                
COMAND(JMP, 0x08, MARK_PARAM,       \
{                                   \
    i_ptr = param;                  \
}                                   \
)

COMAND(CALL, 0x09, MARK_PARAM,      \
{                                   \
    push_ret(i_ptr);                \
    i_ptr = param;                  \
}                                   \
)

COMAND(RET, 0x0A, NO_PARAM,         \
{                                   \
    i_ptr = pop_ret;                \
}                                   \
)

COMAND(END, 0x0B, NO_PARAM,         \
{                                   \
    i_ptr = code_len;               \
}                                   \
)

COMAND(JGR, 0x0C, MARK_PARAM,       \
{                                   \
    if(pop > pop)                   \
        i_ptr = param;              \
}                                   \
)

COMAND(JLS, 0x0D, MARK_PARAM,       \
{                                   \
    if(pop < pop)                   \
        i_ptr = param;              \
}                                   \
)

COMAND(JEQ, 0x0E, MARK_PARAM,       \
{                                   \
    if(pop == pop)                  \
        i_ptr = param;              \
}                                   \
)

COMAND(JGE, 0x0F, MARK_PARAM,       \
{                                   \
    if(pop >= pop)                  \
        i_ptr = param;              \
}                                   \
)

COMAND(JLE, 0x10, MARK_PARAM,       \
{                                   \
    if(pop <= pop)                  \
        i_ptr = param;              \
}                                   \
)

COMAND(IN, 0x11, NO_PARAM,          \
{                                   \
    int i = 0;                      \
    printf("IN: ");                 \
    scanf("%d", &i);                \
                                    \
    push(i);                        \
}                                   \
)

COMAND(IND, 0x12, NO_PARAM,         \
{                                   \
    double i = 0;                   \
    printf("IND: ");                \
    scanf("%lf", &i);               \
                                    \
    push(i * 1000);                 \
}                                   \
)                                   

COMAND(OUTD, 0x13, NO_PARAM,                    \
{                                               \
    printf("OUTD: %lf\n", ((double)pop)/1000);  \
}                                               \
)

COMAND(ABS, 0x14, NO_PARAM,                     \
{                                               \
    push(abs(pop));                             \
}                                               \
)

COMAND(SQRT, 0x15, NO_PARAM,                    \
{                                               \
    push((int)(sqrt(((double)pop)/1000)*1000)); \
}                                               \
)

COMAND(OUTW, 0x16, NO_PARAM,    \
{                               \
    int i = pop;                \
    printf("%s\n", (char*)(&i));\
}                               \
)

