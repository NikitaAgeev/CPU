/*
This file describes the stack debug
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "Stack.h"
#include "Stack_core.h"
#include "Stack_debug_core.h"



enum STACK_ASSERT
{
    //OK = 0, - conflicting name
    WARNING = 1,
    ERROR = 1 << 1,
};


//ELEM_CONST_GENS==============================================

stack_el_t free_stack_mem_gen()
{
    size_t itter = 0;
    stack_el_t ans = 0;

    for(itter = 0; itter < sizeof(stack_el_t); itter++)
    {
        *(((char*)&ans ) + itter) = (char)FREE_MEM_SLOT_E;
    }
    return  ans;
}

stack_el_t poison_stack_mem_gen()
{
    size_t itter = 0;
    stack_el_t ans = 0;

    for(itter = 0; itter < sizeof(stack_el_t); itter++)
    {
        *(((char*)&ans ) + itter) = (char)POISON_MEM_SLOT_E;
    }
    return  ans;

}

stack_el_t first_canary_stack_mem_gen()
{
    size_t itter = 0;
    stack_el_t ans = 0;

    for(itter = 0; itter < sizeof(stack_el_t); itter++)
    {
        *(((char*)&ans ) + itter) = (char)FIRST_CANARY_E;
    }
    return  ans;

}

stack_el_t back_canary_stack_mem_gen()
{
    size_t itter = 0;
    stack_el_t ans = 0;

    for(itter = 0; itter < sizeof(stack_el_t); itter++)
    {
        *(((char*)&ans ) + itter) = (char)BACK_CANARY_E;
    }
    return  ans;

}

//ELEM_CONSTS_GENERATION--------------------------------------------------

const stack_el_t FREE_MEM_SLOT = free_stack_mem_gen();

const stack_el_t POISON_MEM_SLOT = poison_stack_mem_gen();

const stack_el_t FIRST_CANARY = first_canary_stack_mem_gen();

const stack_el_t BACK_CANARY = back_canary_stack_mem_gen();

//============================================================

//HASH_GENERATOR==============================================

#if !defined NO_STACK_ASSERT

static unsigned int MurmurHash2 (char* data, size_t n, size_t size);

static unsigned int MurmurHash2 (char* data, size_t n, size_t size)
{
  size_t len = n * size;
  const unsigned int m = 0x5bd1e995;
  const unsigned int seed = 0;
  const int r = 24;

  unsigned int h = seed^len;

  const unsigned char* dt = (const unsigned char *)data;
  unsigned int k = 0;

  while (len >= 4)
  {
      k  = dt[0];
      k |= dt[1] << 8;
      k |= dt[2] << 16;
      k |= dt[3] << 24;

      k *= m;
      k ^= k >> r;
      k *= m;

      h *= m;
      h ^= k;

      dt += 4;
      len -= 4;
  }

#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

  switch (len)
  {
    case 3:
      h ^= dt[2] << 16;
    case 2:
      h ^= dt[1] << 8;
    case 1:
      h ^= dt[0];
      h *= m;
  };

#pragma GCC diagnostic warning "-Wimplicit-fallthrough"

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

#endif

//HASH_TO_STACK-----------------------------------------------

void make_cach (Stack stack)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    #ifndef NO_STACK_ASSERT
    if(stack->mem != (stack_el_t*)NEW_NO_CTOR)
    {
        //unsigned int old_hash = stack->hash;
        u_int64_t warnings = stack->warnings;
        u_int64_t errors = stack->error;

        stack->warnings = 0;
        stack->error = 0;

        stack->hash = 0;

        unsigned int new_hash = 0;
        new_hash += MurmurHash2((char*)stack, 1, sizeof(struct stack_t));
        if(stack->mem != (stack_el_t*)NEW_NO_CTOR) new_hash += MurmurHash2((char*)stack->mem, stack->size, sizeof(stack_el_t));

        stack->warnings = warnings;
        stack->error = errors;

        stack->hash = new_hash;
    }
    else stack->hash = 0;
    #endif
}

unsigned int find_cach (Stack stack)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    #ifndef NO_STACK_ASSERT
    if(stack->mem != (stack_el_t*)NEW_NO_CTOR)
    {
        unsigned int old_hash = stack->hash;
        
        u_int64_t warnings = stack->warnings;
        u_int64_t errors = stack->error;

        stack->warnings = 0;
        stack->error = 0;
        
        stack->hash = 0;

        unsigned int new_hash = 0;
        new_hash += MurmurHash2((char*)stack, 1, sizeof(struct stack_t));
        if(stack->mem != (stack_el_t*)NEW_NO_CTOR) new_hash += MurmurHash2((char*)stack->mem, stack->size, sizeof(stack_el_t));
        
        stack->warnings = warnings;
        stack->error = errors;

        stack->hash = old_hash;
        
        return new_hash;
    }
    else return 0;
    #else
    printf("end_if is called, it shouldn't be like this\n");
    exit(1);
    #endif
}

//============================================================

//STACK_DUMP==================================================

void stack_mem_dump_f (Stack stack, void (*printer)(stack_el_t elem, FILE* log_file), FILE* log_file)
{
    
    ssize_t itter = 0;
    ssize_t mem_itter = 0;

    if(stack == (Stack)ADR_POISON)
    {
        fprintf(log_file, "Steck is ded\n");
        return;
    }
    
    if(stack->mem == (stack_el_t*)NEW_NO_CTOR)
    {
        fprintf(log_file, "mem not alocated\n");
        return;
    }

    if(stack->mem == (stack_el_t*)ADR_POISON)
    {
        fprintf(log_file, "mem is poison\n");
        return;
    }

    for(itter = -1; itter < stack->size + 1; itter++)
    {
        fprintf(log_file, "%04ld | ", itter);

        for(mem_itter = 0; mem_itter < (ssize_t)sizeof(stack_el_t); mem_itter++)
        {
            fprintf(log_file, "%02hhX ", *(((char*)(stack->mem + itter)) + mem_itter) );
            if((mem_itter + 1) % 4 == 0)
            {
                fprintf(log_file, "| ");
            }
        }
        
        if(*(stack->mem + itter) == FREE_MEM_SLOT) fprintf(log_file, "< FREE_SLOT");
        else if(*(stack->mem + itter) == POISON_MEM_SLOT) fprintf(log_file, "< POISON_SLOT");
        else if(*(stack->mem + itter) == FIRST_CANARY) fprintf(log_file, "< FIRST_CANARY");
        else if(*(stack->mem + itter) == BACK_CANARY) fprintf(log_file, "< BACK_CANARY");
        else if(  printer != NULL)
        {
            fprintf(log_file, "< ");
            (*printer)(*(stack->mem + itter), log_file);
        }

        if(itter == stack->len - 1)
        {
            fprintf(log_file, " << STACK_HEAD");
        }


        fprintf(log_file, "\n");
    }
}

void stack_status_dump_f (Stack stack, FILE* log_file)
{
    
    if(stack == (Stack)ADR_POISON)
    {
        fprintf(log_file, "Stack is ded\n");
        return;
    }

    fprintf(log_file, "adr:         %p\n", (void*)stack);
    
    #ifndef NO_STACK_DUMP_EINFO
    fprintf(log_file, "name:        %s\n", stack->name);
    fprintf(log_file, "mather_funk: %s:%s\n", stack->mather_file, stack->mather_name);
    #endif

    #ifndef NO_STACK_ASSERT
    fprintf(log_file, "hash:        %08X\n", stack->hash);

    if(stack->error) stack_errno_f(stack, log_file);
    else fprintf(log_file, "errors:      %08lX\n", stack->error);
    
    if(stack->warnings) stack_warno_f(stack, log_file);
    else fprintf(log_file, "warnings:    %08lX\n", stack->warnings);
    #endif

    fprintf(log_file, "size:        %ld\n", stack->size);
    fprintf(log_file, "len:         %ld\n", stack->len);
}

void stack_dump_f (Stack stack, void (*printer)(stack_el_t elem, FILE* log_file), FILE* log_file)
{
    
    stack_status_dump_f(stack, log_file);
    
    fprintf(log_file, "memory dump:\n");
    stack_mem_dump_f(stack, printer, log_file);
}



//ASSERT_FUNK=================================================

//STATIC------------------------------------------------------

//unuse function resistans
#if !defined NO_STACK_ASSERT
static int cheack_canary(Stack stack)
{
    #if !defined NO_STACK_ASSERT
    int err_out = OK; 

    if(*(stack->mem - 1) != FIRST_CANARY)
    {
        stack->error |= BAD_FIRST_CANARY;
        err_out |= ERROR;
    }
    if(*(stack->mem + stack->size) != BACK_CANARY)
    {
        stack->error |= BAD_BACK_CANARY;
        err_out |= ERROR;
    }

    if(err_out | ERROR) return err_out;

    ssize_t itter = 0;

    for(itter = 0; itter < stack->size; itter++)
    {
        if(*(stack->mem + itter) == BACK_CANARY)
        {
            stack->warnings |= BACK_CANARY_INSIDE;
            err_out |= WARNING;
        }
        if(*(stack->mem + itter) == FIRST_CANARY)
        {
            stack->warnings |= FIRST_CANARY_INSIDE;
            err_out |= WARNING;
        }
    }

    if(err_out | WARNING) return err_out;
    #endif
    return OK;
}

static int cheack_free_slot (Stack stack)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    #if !defined NO_STACK_ASSERT
    ssize_t itter = 0;
    int err_out = OK;

    for(itter = 0; itter < stack->size; itter++)
    {
        
        stack_el_t elem = *(stack->mem + itter); //опасно для плюсов

        if((itter < stack->len) && (elem == FREE_MEM_SLOT))
        {
            stack->warnings = FREE_SLOT_INSIDE;
            err_out |= WARNING;
        }
        
        if(err_out | WARNING) return err_out;

        if((itter >= stack->len) && (elem != FREE_MEM_SLOT))
        {
            stack->error = DAMAGE_FREE_SLOT;
            err_out |= ERROR;
        }

        if(err_out | ERROR) return err_out;

    }
    #endif
    return OK;

}

static int cheack_poison (Stack stack)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    #if !defined NO_STACK_ASSERT  
    ssize_t itter = 0;
    int err_out = OK;

    for(itter = 0; itter < stack->size; itter++)
    {
        
        stack_el_t elem = *(stack->mem + itter); //опасно для плюсов

        if((itter < stack->len) && (elem == POISON_MEM_SLOT))
        {
            stack->warnings = POISON_SLOT_INSIDE;
            err_out |= WARNING;
        }
        
        if(err_out | WARNING) return err_out;

        if((itter >= stack->len) && (elem == POISON_MEM_SLOT))
        {
            stack->error = DAMAGE_SLOT_TO_POISON ;
            err_out |= ERROR;
        }

        if(err_out | ERROR) return err_out;

    }
    #endif
    return OK;
}

static int cheack_hash (Stack stack)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    #if !defined NO_STACK_ASSERT
    if(stack->hash != find_cach(stack))
    {
        stack->error |= BAD_HASH;
        return ERROR;
    }
    else 
    {
        return OK;
    }
    #endif
    return OK;
}

static int make_error_steck (Stack stack)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    #ifndef NO_STACK_ASSERT
    if(stack == (Stack)ADR_POISON)
    {
        return ERROR;
    }

    stack->warnings = stack->warnings & STACK_HAS_BEEN_CLEARED;

    if(stack->len > stack->size)
    {
        stack->error |= BAD_LEN;
        return ERROR;
    }

    if(stack->mem == (stack_el_t*)ADR_POISON)
    {
        stack->error |= STACK_MEM_IS_POISON;
        return ERROR;
    }

    if(cheack_hash(stack) == ERROR)
    {
        return ERROR;
    } 

    if(stack->mem == (stack_el_t*)NEW_NO_CTOR)
    {
        if(stack->warnings)
        {
            return WARNING;
        }
        else
        {
            return OK;
        }
    }

    cheack_canary(stack);

    if(stack->error & (BAD_FIRST_CANARY | BAD_BACK_CANARY))
    {
        return ERROR;
    }

    cheack_free_slot(stack);

    if(stack->error & (DAMAGE_FREE_SLOT) )
    {
        return ERROR;
    }

    cheack_poison(stack);

    if(stack->error & (DAMAGE_SLOT_TO_POISON))
    {
        return ERROR;
    }

    if(stack->warnings)
    {
        return WARNING;
    }
    #else
    printf("make_error_stack is called, it shouldn't be like this\n");
    exit(1);
    #endif
    return OK;

}
#endif

//External----------------------------------------------------

int stack_errno_f (Stack stack, FILE* log_file)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    UNUSE_IF_NO_FNKASSERT(log_file);
    #ifndef NO_STACK_ASSERT
    if(stack->error) fprintf(log_file, "Stack is error: %08lX \n", stack->error);

    if(stack->error & BAD_FIRST_CANARY)      fprintf(log_file, "> The first canary is damaged.\n");
    if(stack->error & BAD_BACK_CANARY)       fprintf(log_file, "> The back canary is damaged.\n");
    if(stack->error & DAMAGE_FREE_SLOT)      fprintf(log_file, "> The free memory area is damaged.\n");
    if(stack->error & DAMAGE_SLOT_TO_POISON) fprintf(log_file, "> Poison was found in an empty area.\n");
    if(stack->error & BAD_HASH)              fprintf(log_file, "> Memory is corrupted (bad hash).\n");
    if(stack->error & BAD_LEN)               fprintf(log_file, "> Stack length error.\n");
    if(stack->error & STACK_MEM_IS_POISON)   fprintf(log_file, "> Stack memory is poisoned.\n");

    if(stack->error) return 1;
    else             return 0;
    #else
    return 0;
    #endif

}

int stack_warno_f (Stack stack, FILE* log_file)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    UNUSE_IF_NO_FNKASSERT(log_file);
    #ifndef NO_STACK_ASSERT
    if(stack->warnings != 0) fprintf(log_file, "Stack is warning: %08lX \n", stack->warnings);

    if(stack->warnings & FIRST_CANARY_INSIDE)     fprintf(log_file, "> The first canary in the body of the stack.\n");
    if(stack->warnings & BACK_CANARY_INSIDE)      fprintf(log_file, "> The back canary in the body of the stack.\n");
    if(stack->warnings & FREE_SLOT_INSIDE)        fprintf(log_file, "> Free slot in the body of the stack.\n");
    if(stack->warnings & POISON_SLOT_INSIDE)      fprintf(log_file, "> Poison slot in the body of the stack.\n");
    if(stack->warnings & STACK_HAS_BEEN_CLEARED)  fprintf(log_file, "> Stack has been cleared.\n");

    if(stack->warnings) return 1;
    else                return 0;
    #else
    return 0;
    #endif

}

u_int64_t stack_error_code (Stack stack)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    #ifndef NO_STACK_ASSERT
    return stack->error;
    #else
    return 0;
    #endif
}

u_int64_t stack_warning_code(Stack stack)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    #ifndef NO_STACK_ASSERT
    return stack->warnings;
    #else
    return 0;
    #endif
}

int stack_funk_assert_f(Stack stack, const char* parant_func, STACK_ASSERT_EARG)
{
    UNUSE_IF_NO_FNKASSERT(stack);
    UNUSE_IF_NO_FNKASSERT(parant_func);

    NO_USE_DBUG_EARGS
    NO_USE_DINFO_EARGS

    #if !defined NO_STACK_ASSERT && !defined NO_STACK_FUNCK_ASSERT
    int assert_result = make_error_steck(stack);
    
    if(assert_result == ERROR)
    {
        fprintf(file, "ERROR assertation at function \"%s\" at %s:%s:%lu\n", my_func, my_file, parant_func, line);
        stack_dump_f(stack, printer, file);
        fprintf(file, "\n\n");
        
        fclose(file);

        printf("Stack assertation failed\n");
        abort();
    }
    else if(assert_result == WARNING)
    {
        fprintf(file, "Warning assertation at function \"%s\" at %s:%s:%lu\n", my_func, my_file, parant_func, line);
        stack_dump_f(stack, printer, file);
        fprintf(file, "\n\n");

        #ifndef CRITICAL_WARNINGS
        return WARNING;
        #else
        fclose(file);

        printf("Stack assertation failed\n");
        abort();
        #endif

    }
    else
    {
        return OK;
    }
    #endif
    return OK;
}

int stack_assert_f (Stack stack, STACK_ASSERT_DINFO_EARG_R, STACK_ASSERT_DEBUG_EARG_R)
{
    UNUSE_IF_NO_FNKASSERT(stack);

    NO_USE_R_DBUG_EARGS
    NO_USE_R_DINFO_EARGS

    #ifndef NO_STACK_ASSERT
    int assert_result = make_error_steck(stack);
    
    if(assert_result == ERROR)
    {
        printf("Stack assertation failed\n");

        fprintf(file, "ERROR assertation at function %s:%s:%lu\n", my_file, my_func, line);
        stack_dump_f(stack, printer, file);
        fprintf(file, "\n\n");
        
        fclose(file);
        abort();
    }
    else if(assert_result == WARNING)
    {
        fprintf(file, "Warning assertation at function %s:%s:%lu\n", my_file, my_func, line);
        stack_dump_f(stack, printer, file);
        fprintf(file, "\n\n");

        #ifndef CRITICAL_WARNINGS
        return WARNING;
        #else
        fclose(file);
        
        fclose(file);
        abort();
        #endif
        
    }
    else
    {
        return OK;
    }
    #endif
    return OK;
}

int stack_verifi_f (Stack stack, STACK_ASSERT_DINFO_EARG_R, STACK_ASSERT_DEBUG_EARG_R)
{
    UNUSE_IF_NO_FNKASSERT(stack);

    NO_USE_DBUG_EARGS
    NO_USE_DINFO_EARGS


    #ifndef NO_STACK_ASSERT
    int assert_result = make_error_steck(stack);
    
    if(assert_result == ERROR)
    {
        fprintf(file, "ERROR verification at function %s:%s:%lu\n", my_file, my_func, line);
        stack_dump_f(stack, printer, file);
        fprintf(file, "\n\n");
        
        return ERROR;
    }
    else if(assert_result == WARNING)
    {
        fprintf(file, "Warning verification at function %s:%s:%lu\n", my_file, my_func, line);
        stack_dump_f(stack, printer, file);
        fprintf(file, "\n\n");

        return WARNING;
        
    }
    else
    {
        return OK;
    }
    #else
    return OK;
    #endif
    
}

//============================================================