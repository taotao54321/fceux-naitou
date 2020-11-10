#include <cstdlib>
#include <tuple>

#include "debug.h"
#include "driver.h"

#include "core.hpp"
#include "driver.hpp"
#include "naitou.hpp"
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
    Snapshot snapshot;

    const auto dump = [&core]() {
        PRINTLN("Frame: {}", core.frame_count());
        u8 buf[11];
        core.read_bytes(0x2A9, buf);
        for (const auto b : buf)
            PRINT("{:02X} ", b);
        PRINTLN("");
    };

    auto hook_handle = core.hook_before_exec(0xC239, [&core]() {
        PRINTLN("Frame {}: NMI", core.frame_count());
    });

    core.run_frames(20);
    core.snapshot_save(snapshot);
    core.snapshot_save(snapshot);

    core.run_frame(Buttons {}.T(true));
    core.run_frames(20);
    core.snapshot_load(snapshot);
    dump();
    core.snapshot_load(snapshot);
    dump();

    core.unhook_before_exec(hook_handle);

    core.run_frame(Buttons {}.T(true));
    core.run_frames(20);

    const auto play_seq = [&core](u8 size, const Buttons* seq) {
        assert(size <= 11);
        for (const auto i : IRANGE(size)) {
            core.run_frame(seq[i]);
            core.run_frame();
        }
    };

    core.snapshot_save(snapshot);
    const auto tra = Traveller::calc();
    for (const auto src : Sq::sqs_valid()) {
        for (const auto dst : Sq::sqs_valid()) {
            const auto cur = read_cursor(core);

            std::apply(play_seq, tra.query(Traveller::vertex_sq(cur), Traveller::vertex_sq(src)));
            //DBG(read_cursor(core).get(), src.get());
            assert(read_cursor(core) == src);

            std::apply(play_seq, tra.query(Traveller::vertex_sq(src), Traveller::vertex_sq(dst)));
            //DBG(read_cursor(core).get(), dst.get());
            assert(read_cursor(core) == dst);
            core.snapshot_load(snapshot);
        }
    }

    return 0;
}
