main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Hyperbolic Plaid: Color = HighByte( (X + Y + Frame) * (X - Y + Frame) )
; Creates intersecting hyperbolic curves

MOV RB, R2          ; R2 = X
ADD RA, R2          ; R2 = X + Y
ADD R8, R2          ; R2 = Term 1: (X + Y + Frame)

MOV RB, R3          ; R3 = X
SUB RA, R3          ; R3 = X - Y
ADD R8, R3          ; R3 = Term 2: (X - Y + Frame)

IMUL R2, R3         ; R0:R1 = Term1 * Term2 (Signed Multiply)
; Result splits across R0 (High 16) and R1 (Low 16)
; We extract the high byte of the low word for smooth scaling

EXTBH R1, R4        ; R4 = (R1 >> 8) & 0xFF (Extract High Byte)
STOREB R4, R9       ; *Ptr = Color

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