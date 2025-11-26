main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
; --- Precompute Horizon Offset ---
MOV RA, R3          ; R3 = Y
ADD $20, R3         ; R3 = Y + 20 (Shift horizon up/down)

MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Perspective Grid: Color = HighByte((X - 128) * (Y + 20)) ^ Y + Frame
; Creates a 3D floor plane effect

MOV RB, R4          ; R4 = X
SUB $128, R4        ; R4 = X - 128 (Center X)

IMUL R3, R4         ; R0:R1 = R4 * R3 (Signed Multiply)
; Result in R1 (Low 16 bits) contains the scaled value.
; We use high byte to effectively divide by 256 for projection.

EXTBH R1, R5        ; R5 = (R1 >> 8) & 0xFF
XOR RA, R5          ; R5 = ProjectedX ^ Y (Create grid pattern)
ADD R8, R5          ; R5 = Pattern + Frame

STOREB R5, R9       ; *Ptr = Color

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