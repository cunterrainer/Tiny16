#ifndef EXAMPLES_HPP
#define EXAMPLES_HPP

#include <string>
#include <string_view>
#include <unordered_map>

static const std::unordered_map<std::string_view, std::string> s_ExamplesMap = {
    { "Color pattern", 
    R"(
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
JMP frame)"
    },

    {
        "Scrolling interface",
    R"(
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
)"
},

    {
        "Scrolling digital wave",
        R"(
main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Color = ((X + Frame) ^ Y) + ((Y + Frame) ^ X)

MOV RB, R0          ; R0 = X
ADD R8, R0          ; R0 = X + Frame
XOR RA, R0          ; R0 = (X + Frame) ^ Y

MOV RA, R1          ; R1 = Y
ADD R8, R1          ; R1 = Y + Frame
XOR RB, R1          ; R1 = (Y + Frame) ^ X

ADD R1, R0          ; R0 = Combine both terms
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
)"
},

{
    "Hypnotic Ripples",
    R"(
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
)"
},

{
    "Diamond tunnel",
    R"(
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
)"
},

{
    "Fractal Scroll",
    R"(
main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Color = (X + Frame) & (Y - Frame)
; Generates a moving Sierpinski-like fractal texture

MOV RB, R0          ; R0 = X
ADD R8, R0          ; R0 = X + Frame

MOV RA, R1          ; R1 = Y
SUB R8, R1          ; R1 = Y - Frame

AND R0, R1          ; R1 = (X + Frame) & (Y - Frame)
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
)"
},

{
    "Manhatten tunnel",
    R"(
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
)"
},

{
    "Liquid chrome",
    R"(
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
)"
},

{
    "Hyperbolic plaid",
    R"(
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
)"
},

{
    "Kaleidoscope",
    R"(
main:
MOV $0, R8          ; R8 = Frame Counter

frame_loop:
MOV $0x7000, R9     ; R9 = Pixel Pointer (Start of VRAM)
MOV $0, RA          ; RA = Y Coordinate

y_loop:
; --- Precompute |Y - 72| ---
MOV RA, R3          ; R3 = Y
SUB $72, R3         ; R3 = Y - 72
CMP $0, R3          ; Check if negative
JGE y_pos
NEG R3              ; R3 = |Y - 72|
y_pos:

MOV $0, RB          ; RB = X Coordinate

x_loop:
; --- Pixel Pattern Logic ---
; Kaleidoscope: Color = ||X - 128| - |Y - 72|| + Frame
; Creates a geometric cross pattern that ripples

MOV RB, R4          ; R4 = X
SUB $128, R4        ; R4 = X - 128
CMP $0, R4          ; Check if negative
JGE x_pos
NEG R4              ; R4 = |X - 128|
x_pos:

SUB R3, R4          ; R4 = |X| - |Y|
CMP $0, R4          ; Check if negative
JGE diff_pos
NEG R4              ; R4 = ||X| - |Y||
diff_pos:

ADD R8, R4          ; R4 = Pattern + Frame
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
)"
},

{
    "Perspective grid",
    R"(
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
)"
}
};


#endif // EXAMPLES_HPP