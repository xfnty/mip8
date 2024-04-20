; MIP8 Assembly Syntax:
;   `nop`    - Built-in alias for an instruction code.
;   `start:` - Label definition.
;   `.start` - Evaluation of a label to its address.
;   `$00`    - Literal byte value.
;
; Instructions:
;   nop  - Do nothing.
;   halt - Stop the execution.
;   and  - Logical AND.
;   or   - Logical OR.
;   xor  - Logical XOR.
;   add  - Add two values.
;   psh  - Push value in memory at A.
;   psi  - Push value in memory at A and increase A.
;   psp  - Push next byte in program memory.
;   psa  - Push A to the stack.
;   pop  - Pop value from the stack to memory at A.
;   ppa  - Pop value into A.
;   ppi  - Pop value from the stack to memory at A, add 1 to A.
;   dup  - Duplicate value on top of the stack.
;   over - Duplicate second topmost value on the stack.
;   drop - Drop value on top of the stack.
;   jmp  - Copy next byte in program memory into IP.
;   cmp  - Compare two values on the stack without removing them.
;   jzr  - Jump if Zero flag is set.
;   jnz  - Jump if Zero flag is not set.
;   jvf  - Jump if Overflow flag is set.
;   call - Push IP to RS then copy next byte into IP.
;   ret  - Pop value from RS into IP then add 2 to IP.
;   rti  - Return from an interrupt.
;   sei  - Set Interrupts Enabled flag.
;   cli  - Clear Interrupts Enabled flag.


psp $02
psp $02
add
halt