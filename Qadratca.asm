IND 
POP rax
IND
POP rbx
IND
POP rcx

PUSH rax
PUSH 0

JEQ .linear

PUSH 4
PUSH rax
PUSH rcx
MULL
MULL

PUSH rbx
PUSH rbx
MULL

DIV

POP rdx

PUSH rdx
PUSH 0

JGR .no_ans

PUSH rdx
PUSH 0

JEQ .one_ans

%two ans
% (-b-sqrt(D))/(2*a)
PUSH 2
PUSH rax

PUSH rdx
SQRT

PUSH -1
PUSH rbx
MULL

DIV

DEL

OUTD

% (-b+sqrt(D))/(2*a)
PUSH 2
PUSH rax

PUSH rdx
SQRT

PUSH -1
PUSH rbx
MULL

DIV

ADD

OUTD

END

:no_ans

PUSH 20302 %NO
OUTW

END

:one_ans

PUSH 2
PUSH rax

PUSH -1
PUSH rbx
MULL

DEL

OUTD

END

:linear

PUSH rbx
PUSH 0 

JEQ .const_type

PUSH rbx
PUSH -1
PUSH rcx

MULL
DIV

OUTD

END

:const_type

PUSH rcx
PUSH 0

JEQ .inf_ans

PUSH 20302 %NO
OUTW

END

:inf_ans

PUSH 4607561 %INF
OUTW

END

