#include <cstdlib>

#include "debug.h"
#include "driver.h"

#include "core.hpp"
#include "driver.hpp"
#include "prelude.hpp"

namespace {

void usage() {
    EPRINTLN("Usage: fceux <naitou.nes>");
    std::exit(1);
}

} // anonymous namespace

int main(const int argc, const char* const* argv) {
    if (argc != 2) usage();
    const auto path_rom = argv[1];

    Core core(path_rom);

    core.run_frames(20);
    core.run_frame(Buttons {}.T(true));
    core.run_frames(20);

    u8 buf[11];
    core.read_bytes(0x2A9, buf);
    for (const auto b : buf)
        PRINT("{:02X} ", b);
    PRINTLN("");

    return 0;
}
