# This is an example program showcasing all the available assembly instructions
# "#" Represents a comment, the assembler is not case sensitive

main: # Program starts here, hint for assembler need to add a jmp instruction
    MOV  $10, r0 # Use $ for intermediate values
    MOV $0xA, r1 # 0x for hex numbers
    MOV $0b1, r2 # 0b for binary numbers

    ADD $20, R0
    ADD  R1, R0

    JMP subtraction
    
subtraction:
    SUB $2, R0
    SUB R1, R0

    MOV $10, R0
    CMP $10, R0
    JE IsEqual
    # Else block

IsEqual:
    LOAD $0xFF, R0

    # RS is not allowed to be modified
    MOV R1, R2
    SUB $4, R2
    LOAD $0, R1

    MOV $9, R1
    STORE R1, $0xFF

    HLT # stop execution needs to exist