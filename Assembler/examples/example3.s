# This is an example program showcasing all the available assembly instructions
# "#" Represents a comment, the assembler is not case sensitive

main: # Program starts here, hint for assembler need to add a jmp instruction
    MOV  $10, r0 # Use $ for intermediate values
    MOV  $-10, r0 # Use $ for intermediate values

    MOV $0xFF, r0
    STORE R0, $0xA
    MOV $0xD, R2
    MOV $0xAA, r1
    STORE R1, R2

    MOV $0xFFAA, R1
    STORE R1, $0x00

    MOV $0x00, R4
    LOAD R4, R3
    LOAD $0x00, R2

    MOV $0xA, r1 # 0x for hex numbers
    MOV $-0xA, r1 # 0x for hex numbers

    MOV $0xFFFF, r1 # 0x for hex numbers
    MOV $-0xFFFF, r1 # 0x for hex numbers
    MOV $0xFFFFFF, r1 # 0x for hex numbers

    MOV $0b1, r2 # 0b for binary numbers

    ADD $20, R0
    ADD  R1, R0

    SUB $20, R0
    SUB  R1, R0

    MOV  R1, R0
    MOV  R5, R0

    ADD $0x87, R0
    ADD  R1, R0

    SUB $20, R0
    SUB  R1, R0

    MOV  R1, R0
    MOV  R5, R0

    SUB $0xFFFF, r1 # 0x for hex numbers
    SUB $-0xFFFF, r1 # 0x for hex numbers
    SUB $0xFFFFFF, r1 # 0x for hex numbers

    ADD $0xFFFF, r1 # 0x for hex numbers
    ADD $-0xFFFF, r1 # 0x for hex numbers
    ADD $0xFFFFFF, r1 # 0x for hex numbers

    ADD $0xFFFF, r1 # 0x for hex numbers
    ADD $-0xFFFF, r1 # 0x for hex numbers
    ADD $0xFFFFFF, r1 # 0x for hex numbers

    SUB $0xFFFF, r1 # 0x for hex numbers
    SUB $-0xFFFF, r1 # 0x for hex numbers
    SUB $0xFFFFFF, r1 # 0x for hex numbers

    MOV $0, R7
    CMP $0, R7
    JE main

    HLT