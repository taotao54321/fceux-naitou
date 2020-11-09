#include <cstdlib>

#include "debug.h"
#include "driver.h"

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

    if (!FCEUI_Initialize()) PANIC("FCEUI_Initialize() failed");

    if (LoadGame(path_rom, true) == 0) PANIC("failed to load ROM");

    LOOP(20) {
        u8* xbuf;
        i32* soundbuf;
        i32 soundbuf_size;
        // TODO: skip=1 の方が速そうだが、エミュレーションに影響しないか確信が持てない
        FCEUI_Emulate(&xbuf, &soundbuf, &soundbuf_size, 0);
    }

    for (const auto addr : IRANGE(0x90, 0xA0)) {
        PRINTLN("{:02X}: {:02X}", addr, GetMem(addr));
    }

    return 0;
}
