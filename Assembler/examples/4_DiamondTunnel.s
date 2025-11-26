main:
MOV $0, R8

frame_loop:
MOV $0x7000, R9
MOV $0, RA

y_loop:
MOV RA, R5
SUB $72, R5
CMP $0, R5
JGE y_pos
NEG R5
y_pos:

MOV $0, RB

x_loop:
MOV RB, R6
SUB $128, R6
CMP $0, R6
JGE x_pos
NEG R6
x_pos:

ADD R5, R6
SUB R8, R6
STOREB R6, R9

ADD $1, R9
ADD $1, RB
CMP $256, RB
JNE x_loop

ADD $1, RA
CMP $144, RA
JNE y_loop

vblank:
HALT
ADD $1, R8
JMP frame_loop

BRK