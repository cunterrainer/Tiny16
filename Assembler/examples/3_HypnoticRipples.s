main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Color = ((X - 128)^2 + (Y - 72)^2) - Frame
; Creates concentric circles radiating from the center

MOV RB, RC          ; RC = X
SUB $128, RC        ; RC = X - 128 (Center offset)
IMUL RC, RC         ; R0:R1 = RC * RC (Signed Square)
MOV R1, RD          ; RD = Low 16 bits of X^2

MOV RA, RC          ; RC = Y
SUB $72, RC         ; RC = Y - 72 (Center offset)
IMUL RC, RC         ; R0:R1 = RC * RC (Signed Square)
ADD R1, RD          ; RD = X^2 + Y^2

SUB R8, RD          ; Shift phase by Frame (Ripple Outward)
STOREB RD, R9       ; *Ptr = Color

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