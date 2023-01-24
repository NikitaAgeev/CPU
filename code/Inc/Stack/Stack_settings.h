/*
This is the file needed to permanently configure the stack
*/

typedef int stack_el_t;

//#define NO_STACK_ASSERT

//#define NO_STACK_DUMP_EINFO

//#define NO_STACK_FUNCK_ASSERT

//#define CRITICAL_WARNINGS


//Resize settings
static const ssize_t stack_start_size = 10;         //< Start stack size

static const ssize_t stack_geometri_coef = 2;       //< Geometric progression coefficient for exponential growth of stack size.

static const ssize_t stack_linear_coef = 10;        //< Linear stack size growth factor

static const ssize_t stack_num_exp_increases = 2;   //< The number of increases with exponential growth of the stack size


