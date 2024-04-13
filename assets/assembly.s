; Source file is interpreted word-by-word.
;
; MIP-8 Assembler Syntax:
;   `name` - builtin assembly command or a user-defined identifier
;   `#name=value` - define a constant
;   `name:` - defines a label
;   `.name` - evaluates to the labels address
;   `$xx` - evaluates to a single byte

#nop $00
#halt $01
#and $02
#or $03
#xor $04
#add $05
#psh $06
#psi $07
#psp $08
#psa $09
#pop $0A
#ppa $0B
#ppi $0C
#dup $0D
#over $0E
#drop $0F
#jmp $10
#cmp $11
#jzr $12
#jvf $13
#call $14
#ret $15
#rti $16
#sei $17
#cli $18

#io_port=$C3

start:
    call .setup_interrupts
    psp $01
    psp $02
    add
    psp $00
    cmp
    jzr .start
    jmp .end

end:
    halt

setup_interrupts:
    pushp $C7
    popa
    pushp .io_int
    pop
    ret    

io_int:
    pushp io_port
    popa
    push
    pop
    reti
