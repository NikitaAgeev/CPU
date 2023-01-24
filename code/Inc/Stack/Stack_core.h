/*
This is a file describing the functions and variables necessary for the library function to work.
*/

#ifndef STACK_CORE_H
#define STACK_CORE_H

struct stack_t
{
    //Additional information about the stack
    #ifndef NO_STACK_DUMP_EINFO
    const char* name;
    const char* mather_name;
    const char* mather_file;
    #endif

    //Additional information for error handling
    #ifndef NO_STACK_ASSERT
    unsigned int hash;
    u_int64_t error;
    u_int64_t warnings;
    #endif

    ssize_t size;
    ssize_t len;
    stack_el_t* mem;
};

/// [Stack] is a pointer to the control structure
typedef struct stack_t* Stack; 

//unuse variable resistens defines=================================

#if !defined NO_STACK_ASSERT && !defined NO_STACK_FUNCK_ASSERT
#define UNUSE_IF_NO_FNKASSERT(PARAM);
#else
#define UNUSE_IF_NO_FNKASSERT(PARAM)(void)PARAM;
#endif

#define UNUSE_VAR(VAR) (void)VAR;


#if !defined NO_STACK_ASSERT && !defined NO_STACK_FUNCK_ASSERT 
#define NO_USE_DINFO_EARGS  \
(void)file;                 \
(void)printer; 
#else 
#define NO_USE_DINFO_EARGS ;
#endif

#define NO_USE_R_DINFO_EARGS \
(void)file;                  \
(void)printer; 

#if !defined NO_STACK_DUMP_EINFO 
#define NO_USE_DBUG_EARGS   \
(void)my_file;              \
(void)my_func;              \
(void)line;                 
#else
#define NO_USE_DBUG_EARGS ;
#endif 

#define NO_USE_R_DBUG_EARGS \
(void)my_file;              \
(void)my_func;              \
(void)line;      

//====================================================================

enum STACK_ADR_STATUS
{
    OK = 0,
    NEW_NO_CTOR = 1,
    ADR_POISON = 13,
    ADR_NO_CONSTRUCT = 31,
    DATA_LOST = 404 
};

#endif
