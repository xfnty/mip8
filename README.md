# MIP-8 Emulator

## Usage

<details>
    <summary>Dependencies</summary>
    <ul>
        <li><a href="https://gnuwin32.sourceforge.net/packages/make.htm">GNU Make for Windows</a></li>
        <li><a href="https://www.bellard.org/tcc/">Tiny C compiler</a></li>
    </ul>
</details>

- Building and running: `make`
- Running with arguments: `make args="assets\fibonacci.bin"`

Command line arguments:
- `-i` - Execute program interactively.
- `-p` - Print disassembly.
- `<rom>` - Program ROM file.

## Overview
Minimal instruction processor, MIP for short, is a fantasy 8-bit stack-based CPU.

## Charachteristics
- 8-bit address and data buses.
- Up to 32 Kb of RAM.
- Up to 16 Kb of NVRAM.

### CPU Specification
- Registers:
  - **A**: 8-bit wide address register.
  - **F**: status register (**Z** - last calculated value was zero, **O** - **T** overflow happened).
  - **IP**: 8-bit wide instruction pointer. Points to the instruction in program memory that is being executed.
  - **IRA**: 8-bit wide interrupt return address. Points to the instruction that was being executed when the interrupt happened.
  - **DS**: 32-byte long data stack. Top of the **DS** is referenced down below as **T**.
  - **RS**: 8-byte long return address stack.
  - **DSS**: 5-bit wide data stack size register. 
  - **RSS**: 3-bit wide return stack size register. 

Both **DS** and **RS** grow upward.
Program memory is not connected to the main memory, addressable by **A**.

### Instructions
- ALU instructions:
  - `AND` - pop two bytes from **DS**, logically multiply them and push the result back.
  - `OR` - pop a and b; push a|b.
  - `XOR` - pop a, b; push a^b.
  - `ADD` - pop a, b; push a+b.
- Stack manipulations:
  - `PUSH` - push byte from main memory at **A**.
  - `PUSHI` - push memory[**A**], **A**++.
  - `PUSHP` - push progmem[**PC**], **PC**++.
  - `PUSHA` - push **A** to **DS**.
  - `POP` - pop **T** and write it to memory at address **A**.
  - `POPA` - pop **T** into **A**.
  - `POPI` - pop **T** into **A** and increase **A**.
  - `DUP` - push **T**.
  - `OVER` - push **DS**\[**DSP**-2\].
  - `DROP` - decrease **DSP**.
- Branching:
  - `JUMP` - pop **T** into **IP**.
  - `CMP` - compare two values on the stack.
  - `JZR` - set **IP** to next byte if **Z** is set.
  - `JOV` - set **IP** to next byte if **O** is set.
- Calling subroutines:
  - `CALL` - push **IP** to **RS**, set **IP** to the next byte in memory.
  - `RET` - pop **RS** into **IP**, add 2 to **IP**.
  - `RETI` - return from an interrupt.
  - `SEI` - Enable interrupts.
  - `CLI` - Disable interrupts.
- Other:
  - `NOP` - do nothing.
  - `HALT` - stop the execution.

### Main Memory Layout
- `00:7F` (128) - switchable RAM page.
- `80` - RAM page index.
- `81:C0` (64) - NVRAM page.
- `C1` - NVRAM page index.
- `C2` - Timer Period.
- `C3` - Timer Counter.
- `C4` - Timer overflow interrupt address.
- `C5:FB` (56) - Unused.
- `FC:FF` - Debug port.
