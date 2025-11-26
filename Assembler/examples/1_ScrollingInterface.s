main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Color = ((X + Y) ^ (X - Y)) + Frame

MOV RB, R0          ; R0 = X
ADD RA, R0          ; R0 = X + Y

MOV RB, R1          ; R1 = X
SUB RA, R1          ; R1 = X - Y (Wraps on negative, which is fine)

XOR R1, R0          ; R0 = (X + Y) ^ (X - Y)
ADD R8, R0          ; R0 = Pattern + Frame (Offset color)

STOREB R0, R9       ; *Ptr = Color

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