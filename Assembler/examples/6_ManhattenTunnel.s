main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
; --- Precompute DY ---
MOV RA, R3          ; R3 = Y
SUB $72, R3         ; R3 = Y - 72
CMP $0, R3
JGE y_pos
NEG R3              ; R3 = |Y - 72|
y_pos:

MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Manhattan Tunnel: Color = 2500 / (|X-128| + |Y-72| + 1) + Frame

MOV RB, R4          ; R4 = X
SUB $128, R4        ; R4 = X - 128
CMP $0, R4
JGE x_pos
NEG R4              ; R4 = |X - 128|
x_pos:

ADD R3, R4          ; R4 = |X| + |Y| (Manhattan Distance)
ADD $1, R4          ; R4 = Distance + 1 (Prevent divide by zero)

MOV $2500, R2       ; R2 = Constant Numerator
DIV R4, R2          ; R1 = 2500 / R4 (Quotient in R1, Remainder in R0)

ADD R8, R1          ; R1 = BaseColor + Frame (Animation)
STOREB R1, R9       ; *Ptr = Color

; --- Loop Management ---
ADD $1, R9          ; Increment Pixel Pointer
ADD $1, RB          ; Increment X
CMP $256, RB        ; Check if X == 256
JNE x_loop

ADD $1, RA          ; Increment Y
CMP $144, RA        ; Check if Y == 144
JNE y_loop

vblank:
HALT                ; Wait for VBlank
ADD $1, R8          ; Increment Frame Counter
JMP frame_loop      ; Draw next frame

BRK