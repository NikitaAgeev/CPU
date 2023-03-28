# CPU
This is a CPU simulation project. It consists of its own assembler and a processor simulator. There is also a code generation system for the command system.
# CPU
The central processor has a stack, return stack and RAM + GRAM

## How to start 
`./[CPU] [bin_file].bin`

Launches extension files `.bin`
# ASM
## How to start
`./[ASM] [asm_file].asm [bin_file].bin`
## Syntax

### Parametrs
#### Label 
`:[label]` - creating a label in the code

`comand .[label]` - passing a label as an argument

#### Register
`rax` - register name 
`x = 'a-d'` - register variants

#### Number
Any integer (`n`) is an argument

#### Combination of number and register
Any 'rax +- n' is an argument

#### RAM
`[n] or [rax] or [rax + n]` - memory slot with address inside square brackets

### Comands type
The commands have 4 types
#### NO_PARAM
Comand don't have any argumets
#### MARK_PARAM 
The command argument is a label
#### IN_MEM_PARAM 
A number, the contents of a register or a memory cell are passed as an argument to the function.
#### OUT_MEM_PARAM
Еhe argument is the memory cell in which to put the result or the absence of a cell if the result is not needed.

### Code generation DSL
* `pop` - pop from stack
* `push()` - push to stack
* `pop_ret` - pop from ret_stack
* `push_ret()` - push to ret_stack
* `i_ptr` - instruction pointer
* `param` - comand parametr
* `code_len` - code len

### How add new comand
Add to file comands.h new `COMAND([name], [bin_code], [param_type], [code])`

