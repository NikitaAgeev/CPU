/*
This file describes the stack and the main mechanisms of its operation.
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "Stack.h"
#include "Stack_core.h"
#include "Stack_debug_core.h"



//Resizing_module=====================================================================

//Resize settings---------------------------------------------------------------------

/// The stack size at which the exponential growth of the stack size becomes linear.
static const ssize_t stack_change_resize = stack_start_size * pow(stack_geometri_coef, stack_num_exp_increases);  


//function declaration----------------------------------------------------------------

///only for stack_resize_f
static void  stack_resize_up_f (Stack stack);

///only for stack_resize_f
static void stack_resize_down_f (Stack stack);

///only for stack_resize_f
static void stack_mem_init_f (Stack stack);

static void stack_resize_f (Stack stack);


//Functions---------------------------------------------------------------------------

static void stack_resize_up_f (Stack stack)
{   
    
    assert(stack != nullptr);
    assert((stack->mem != (stack_el_t*)NEW_NO_CTOR) && (stack->mem != (stack_el_t*)ADR_POISON));
    assert(stack->len >= 0);
    assert(stack->size >= 0);

    #ifndef NO_STACK_ASSERT
    assert(stack->error == 0);
    #endif

    if( stack->size >= stack_change_resize )
    {
        stack->size += stack_linear_coef;
    }
    else
    {
        stack->size *= stack_geometri_coef;
    }

    stack->mem = ( (stack_el_t*)realloc(stack->mem - 1, ((stack->size + 2) * sizeof(stack_el_t))) ) + 1;    

    if(stack->mem == NULL)
    {
        stack->mem = (stack_el_t*)DATA_LOST;
        return;
    }

    ssize_t itter = 0;

    for(itter = stack->len; itter < stack->size; itter++)
    {
        *(stack->mem + itter) = FREE_MEM_SLOT;
    }
    
    *(stack->mem + stack->size) = BACK_CANARY;

    return;

}

static void stack_resize_down_f (Stack stack)
{
    
    assert(stack != nullptr);
    assert((stack->mem != (stack_el_t*)NEW_NO_CTOR) && (stack->mem != (stack_el_t*)ADR_POISON));
    assert(stack->len >= 0);
    assert(stack->size >= 0);

    #ifndef NO_STACK_ASSERT
    assert(stack->error == 0);
    #endif

    ssize_t stack_old_size = stack->size;
    
    if(stack->size > stack_change_resize)
    {
        stack->size -= stack_linear_coef;
    }
    else
    {
        stack->size /= stack_geometri_coef;
    }

    ssize_t itter = 0;

    for(itter = stack->size; itter < stack_old_size; itter++)
    {
        *(stack->mem + itter) = POISON_MEM_SLOT;
    }

    stack->mem = (stack_el_t*)realloc(stack->mem - 1, (stack->size + 2) * sizeof(stack_el_t)) + 1;

    if(stack->mem == NULL)
    {
        stack->mem = (stack_el_t*)DATA_LOST;
        return;
    }

    *(stack->mem + stack->size) = BACK_CANARY;

    return;

}

static void stack_mem_init_f (Stack stack)
{
    
    assert(stack != nullptr);
    assert(stack->mem != (stack_el_t*)ADR_POISON);

    #ifndef NO_STACK_ASSERT
    assert(stack->error == 0);
    #endif

    stack->size = stack_start_size;
    stack->len = 1;

    stack->mem = ((stack_el_t*)calloc(stack_start_size + 2, sizeof(stack_el_t))) + 1;

    if(stack->mem == NULL)
    {
        stack->mem = (stack_el_t*)DATA_LOST;
        return;
    }

    *(stack->mem + stack->size) = BACK_CANARY;
    *(stack->mem - 1) = FIRST_CANARY;

    return;

}

static void stack_resize_f (Stack stack)
{
    
    assert(stack != nullptr);
    assert(stack->mem != (stack_el_t*)ADR_POISON);

    #ifndef NO_STACK_ASSERT
    assert(stack->error == 0);
    #endif
    
    if(stack->mem == (stack_el_t*)NEW_NO_CTOR)
    {
        stack_mem_init_f(stack);
        return;
    }
    
    if( ( (stack->len * stack_geometri_coef) < stack->size) && (stack->size != stack_start_size) && (stack->size <= stack_change_resize) )
    {
        stack_resize_down_f(stack);
    }
    else if( (stack->len > stack->size) && (stack->size < stack_change_resize) )
    {   
        stack_resize_up_f(stack);
    }
    else if( ( (stack->len + stack_linear_coef) < stack->size) && (stack->size != stack_start_size) && (stack->size > stack_change_resize) )
    {
        stack_resize_down_f(stack);
    }
    else if( (stack->len > stack->size) && (stack->size >= stack_change_resize) )
    {
        stack_resize_up_f(stack);
    }

    return;

}

//Define------------------------------------------------------------------------------

//Constructor_module==================================================================

int stack_ctor_f (Stack* stack, const char* name, STACK_EARGS)
{
    
    int clear_flag = 0;

    //anty_unuse_var
    #ifndef NO_STACK_DUMP_EINFO
    UNUSE_IF_NO_FNKASSERT(line);
    #else
    UNUSE_VAR(name);
    #endif

    if((*stack != nullptr) && (*stack != (Stack)ADR_POISON))
    {
        free((*stack)->mem);
        clear_flag = 1;
    }

    *stack = (Stack)(calloc(1, sizeof(struct stack_t)));
    
    if(*stack == nullptr)
    {
        *stack = (Stack)ADR_NO_CONSTRUCT;
        return ADR_NO_CONSTRUCT;
    }

    #ifndef NO_STACK_DUMP_EINFO
    (*stack)->mather_name = my_func;
    (*stack)->mather_file = my_file;
    (*stack)->name = name;
    #endif

    #ifndef NO_STACK_ASSERT
    (*stack)->error = 0;
    (*stack)->warnings = 0;
    if(clear_flag == 1) (*stack)->warnings = STACK_HAS_BEEN_CLEARED;
    #else
    (void)clear_flag;
    #endif

    (*stack)->len = -1;
    (*stack)->size = -1;
    (*stack)->mem = (stack_el_t*)NEW_NO_CTOR;

    make_cach(*stack);

    FUNC_ASERT(*stack);

    return OK;

}

void stack_dtor_f (Stack* stack, STACK_EARGS)
{
    
    //anty_unuse_var
    #ifndef NO_STACK_DUMP_EINFO
    UNUSE_IF_NO_FNKASSERT(my_file);
    UNUSE_IF_NO_FNKASSERT(my_func);
    UNUSE_IF_NO_FNKASSERT(line);
    #endif

    FUNC_ASERT(*stack);

    free((*stack)->mem - 1);
    
    (*stack)->mem = (stack_el_t*)ADR_POISON;
    (*stack)->len = -1;
    (*stack)->size = -1;

    free(*stack);

    *stack = (Stack)ADR_POISON;
    return;

}

//Stack_access_module=================================================================

void stack_push_f (Stack stack, stack_el_t elem, STACK_EARGS)
{
    
    //anty_unuse_var
    #ifndef NO_STACK_DUMP_EINFO
    UNUSE_IF_NO_FNKASSERT(my_file);
    UNUSE_IF_NO_FNKASSERT(my_func);
    UNUSE_IF_NO_FNKASSERT(line);
    #endif

    FUNC_ASERT(stack);

    (stack)->len ++;
    stack_resize_f(stack);

    if(stack->mem != ((stack_el_t*)DATA_LOST))
    *((stack)->mem + (stack)->len - 1) = elem;
    
    make_cach(stack);
    
    FUNC_ASERT(stack);

    return;

}

stack_el_t stack_pop_f (Stack stack, STACK_EARGS)
{ 
    
    //anty_uneuse_var
    #ifndef NO_STACK_DUMP_EINFO
    UNUSE_IF_NO_FNKASSERT(my_file);
    UNUSE_IF_NO_FNKASSERT(my_func);
    UNUSE_IF_NO_FNKASSERT(line);
    #endif

    FUNC_ASERT(stack);

    if( (stack == nullptr) || (stack == (Stack)ADR_POISON) || (stack == (Stack)DATA_LOST))
    {
        return 0;
    }

    if((stack)->len > 0)
    {
        (stack)->len --;
        stack_el_t elem = *((stack)->mem + (stack)->len);
        *((stack)->mem + (stack)->len) = FREE_MEM_SLOT;
        stack_resize_f(stack);

        make_cach(stack);
        
        FUNC_ASERT(stack);

        return elem;
    }
    else
    {

        FUNC_ASERT(stack);

        return 0;
    }

}


