main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
; --- Precompute Y Term ---
MOV RA, R3          ; R3 = Y
ADD R8, R3          ; R3 = Y + Frame

MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Liquid Chrome: Color = HighByte((X - Frame) * (Y + Frame)) + Frame

MOV RB, R4          ; R4 = X
SUB R8, R4          ; R4 = X - Frame

IMUL R3, R4         ; R0:R1 = R4 * R3 (Signed Multiply)
; Result is 32-bit in R0:R1.
; We want the middle bits for a smooth gradient.

SWAPB R1            ; Swap bytes of R1 to put the high byte (bits 8-15) in low position
ADD R8, R1          ; Add Frame to cycle colors
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