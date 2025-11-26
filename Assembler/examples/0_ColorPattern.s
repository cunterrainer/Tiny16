main:
MOV $0, R0

frame:
MOV $0x6FFF, R1
MOV $144, R2
MOV R0, R3

y_loop:
MOV $256, R4
MOV R3, R5

x_loop:
STORE R5, R1
ADD $1, R1
ADD $1, R5
SUB $1, R4
CMP $0, R4
JE next_line
JMP x_loop

next_line:
ADD $1, R3
SUB $1, R2
CMP $0, R2
JE vblank
JMP y_loop

vblank:
HALT
ADD $3, R0
JMP frame