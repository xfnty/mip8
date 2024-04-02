# MIP-8 Emulator

## Usage
```
make args="assets\nop.bin"
```

## MIP-8 Overview
Micro-instruction processor, MIP for short, is a fantasy 8-bit stack-based CPU designed for educational purposes.

### CPU Specification
- Registers:
  - **A**: 8-bit wide address register.
  - **PC**: 8-bit wide program counter. Points to the next instruction in program memory.
  - **DS**: 32-byte long data stack. Top of the **DS** is referenced down below as **T**.
  - **RS**: 8-byte long return address stack.
  - **DSP**: 5-bit wide data stack pointer. 
  - **RSP**: 3-bit wide return stack pointer. 
Both **DS** and **RS** grow upward.
Program memory is not connected to the main memory, addressable by **A**.

### Instructions
- Stack manipulations:
  - `PSM` - fetch a word from memory pointed at by **A** and push it to **DS**.
  - `PSI` - fetch a word pointed at by **IP** and push it to **DS**.
  - `PSA` - push **A** to **DS**.
  - `PLM` - pop **T** and write it to memory at address **A**.
  - `PLA` - pop **T** into **A**.
  - `DUP` - push **T**.
  - `OVER` - push **DS**\[**DSP**-2\].
  - `DROP` - decrease **DSP**.
- Calling subroutines:
  - `JZ` - set **IP** to the most-significant half of the current word if **T** is zero.
  - `CALL` - push **IP** to **RS**, set **IP** to value from memory previously pointed at by **IP**.
  - `RET` - pop **RS** into **IP**.
- ALU instructions:
  - `XOR` - pop two values from **DS** and XOR them and push the result back.
  - `OR` - pop a and b, push a | b.
  - `AND` - pop a and b, push a & b.
  - `ADD` - pop a and b, push a + b.
  - `INCA` - add 1 to **A**.
- Other:
  - `NOP` - do nothing.
  - `HALT` - stop the execution.

### Program Memory Layout
- `0x00:0xFF` - all program memory,

### Main Memory Layout
- `0x00:0x7F` - switchable RAM page.
- `0x80` - RAM page index.


## TODO:
- [ ] Refine instruction set
- [ ] Add interrupts for input device
- [ ] Add silent mode (only IO is visible) and better crash report
