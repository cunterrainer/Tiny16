#include <cstdint>
#include <vector>

#include "CPU.hpp"
#include "File.hpp"
#include "Result.hpp"

int main()
{
    Result<std::vector<std::uint8_t>> e = LoadFile("examples/example1.ty");
    if (e.IsErr())
        return EXIT_FAILURE;

    CPU cpu;
    cpu.Execute(e.ForceUnwrap());
    CPU_PRINT_REGISTERS(cpu);
    return 0;
}