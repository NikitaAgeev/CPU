IN

POP rax

PUSH 1

CALL .FACTORIAL

OUT

END


:FACTORIAL

POP rbx
PUSH rbx
PUSH rbx + 1

PUSH rbx + 1
PUSH rax

JLE .NO_CALL

CALL .FACTORIAL

:NO_CALL

MULL

RET


