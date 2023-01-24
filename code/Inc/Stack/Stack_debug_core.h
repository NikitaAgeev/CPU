/*
The file required by library functions for debag
*/

#ifndef STACK_DEBUG_CORE_H
#define STACK_DEBUG_CORE_H

/// Memory filling patterns corresponding to different statuses of memory elements
/// On their basis, constants of elements are generated
enum STACK_ELEM_STATUS
{
    FREE_MEM_SLOT_E = 0xF5,    
    POISON_MEM_SLOT_E = 0xDE,
    FIRST_CANARY_E = 0xFC,
    BACK_CANARY_E = 0xBC
};

/// Constants describing stack elements corresponding to some statuses generated based on STACK_ELEM_STATUS
extern const stack_el_t FREE_MEM_SLOT;
extern const stack_el_t POISON_MEM_SLOT;
extern const stack_el_t FIRST_CANARY;
extern const stack_el_t BACK_CANARY;


/// Error masks in the stack
enum STACK_ERROR
{
    BAD_FIRST_CANARY        = 1 << 0,
    BAD_BACK_CANARY         = 1 << 1,
    DAMAGE_FREE_SLOT        = 1 << 2,
    DAMAGE_SLOT_TO_POISON   = 1 << 3, 
    BAD_HASH                = 1 << 4,
    BAD_LEN                 = 1 << 5,
    STACK_MEM_IS_POISON     = 1 << 6,
};

/// Warning masks in the stack
enum STACK_WARNINGS
{
    FIRST_CANARY_INSIDE    = 1 << 0,
    BACK_CANARY_INSIDE     = 1 << 1,
    FREE_SLOT_INSIDE       = 1 << 2,
    POISON_SLOT_INSIDE     = 1 << 3,
    STACK_HAS_BEEN_CLEARED = 1 << 4,
};


//change stack cach
void make_cach (Stack stack);

//fing stack cach
unsigned int find_cach (Stack stack);

//FUNK_ASSERT=====================================================================
//Description of the ASSERT function inside standard library functions

#if !defined NO_STACK_ASSERT && !defined NO_STACK_FUNCK_ASSERT

int stack_funk_assert_f(Stack stack, const char* parant_func, STACK_ASSERT_EARG);
#define STACK_A_E_A_I_CORE my_file, __FUNCTION__, line, file, printer
#define FUNC_ASERT(STACK) stack_funk_assert_f(STACK, my_func, STACK_A_E_A_I_CORE)

#else

#define FUNC_ASERT(STACK); 

#endif

#endif




