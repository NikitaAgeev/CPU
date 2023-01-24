/*
This is the file required to use the library, which contains a description of the basic functions for the stack.
*/

#include "Stack_settings.h"

struct stack_t;

typedef struct stack_t* Stack;

#include "Stack_debug.h"

//define_eargs===============================================================

//extra_args_define
#define STACK_EARGS     STACK_ASSERT_EARG

#define STACK_EARGS_IN  STACK_ASSERT_EARG_IN


//FUNCTION_ACCESS============================================================

int stack_ctor_f (Stack* stack, const char* name, STACK_EARGS);

void stack_dtor_f (Stack* stack, STACK_EARGS);

void stack_push_f (Stack stack, stack_el_t elem, STACK_EARGS);

stack_el_t stack_pop_f (Stack stack, STACK_EARGS);

//DEFINE_ACCESS--------------------------------------------------------------

#define POP(stack) stack_pop_f(stack, STACK_EARGS_IN)

#define PUSH(stack, elem) stack_push_f(stack, elem, STACK_EARGS_IN)

#define STACK_CTOR(stack) stack_ctor_f(&stack, #stack, STACK_EARGS_IN)

#define STACK_DTOR(stack) stack_dtor_f(&stack, STACK_EARGS_IN)

//===========================================================================
