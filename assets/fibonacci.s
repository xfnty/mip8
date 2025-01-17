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

; Example program: Fibonacci sequence calculator
start:
    psp $01
    dup dup dup
    ; Save IO port address into A
    psp $FF
    ppa
    ; Print first two numbers
    pop 
    pop

loop:
    ; Save previous value in A
    over
    ppa

    add
    
    ; Jump to .end if the sum is greater than 255
    jvf .end

    ; Save previous value in stack
    psa

    ; Output current number
    psp $FF
    ppa
    over
    pop
    
    ; Restore previous value in both stack and A
    ppa
    psa

    jmp .loop

end:
    halt
