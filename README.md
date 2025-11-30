# Tiny16

**Tiny16** is a custom 16-bit computer project that includes:
- A custom **Assembler** for writing programs in a tailored assembly language.
- A full **Emulator** for simulating the Tiny16 CPU and its graphical output.

---

## Project Overview

### Assembler
A custom assembler designed to write programs for the Tiny16 CPU using a simple assembly language.

- Syntax includes support for decimal, hexadecimal (`0x`), and binary (`0b`) literals.
- Example programs can be found in [`Assembler/examples`](Assembler/examples).
- For additional information see the SPEC.txt

### Emulator
A full emulator for the Tiny16 system:
- Simulates the CPU and hardware behavior.
- Provides a graphical output window for visual feedback.

# Build
## Clone
```
git clone https://github.com/cunterrainer/Tiny16.git
```
```
cd Tiny16
```

## Build
```
vendor\premake5.exe vs2022
```
Open the Solution (.sln) There you find all the projects including the emulator and assembler aswell as tests for the assembler

### Projects in the Solution
After generating the project files, open the `.sln` file in Visual Studio.

You will find the following projects:

- **Assembler**: Console application that assembles Tiny16 programs.
- **Assembler-Lib**: Core logic of the assembler (exposed as a static library).
- **Assember-Tests**: Unit tests for the assembler, using [doctest](https://github.com/doctest/doctest).
- **Emulator**: Emulator for the Tiny16 CPU, with graphical output.

## Testing
To run the tests:

1. Build the `Assembler-Test` target.
2. Run the resulting executable — test results will be printed to the console.

You can also add new test files to the `Tests/` directory and rerun the executable to validate your changes.
