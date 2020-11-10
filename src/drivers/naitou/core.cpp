#include <cstdio>
#include <string>

#include <boost/core/noncopyable.hpp>

#include <zlib.h>

#include "debug.h"
#include "driver.h"
#include "emufile.h"
#include "fceu.h"
#include "git.h"
#include "movie.h"
#include "state.h"

#include "core.hpp"
#include "driver.hpp"
#include "prelude.hpp"

//--------------------------------------------------------------------
// Snapshot
//--------------------------------------------------------------------

class SnapshotImpl : private boost::noncopyable {
private:
    EMUFILE_MEMORY file_ {};

    friend class Core;

public:
    SnapshotImpl() = default;

    void clear() { file_.truncate(0); }
};

Snapshot::Snapshot()
    : impl_(new SnapshotImpl) {}

//--------------------------------------------------------------------
// HookHandle
//--------------------------------------------------------------------

HookHandle::HookHandle(int id)
    : id_(id) {}

//--------------------------------------------------------------------
// Core
//--------------------------------------------------------------------

Core::Core(const std::string& path_rom) {
    if (!FCEUI_Initialize()) PANIC("FCEUI_Initialize() failed");

    if (LoadGame(path_rom.c_str(), true) == 0) PANIC("failed to load ROM");

    // 入力ポート 0 のみ有効 (標準ゲームパッド)
    FCEUI_SetInput(0, SI_GAMEPAD, &gamepad_data_, 0);
    FCEUI_SetInput(1, SI_NONE, nullptr, 0);
    FCEUI_SetInputFC(SIFC_NONE, nullptr, 0);
    FCEUI_SetInputFourscore(false);
}

int Core::frame_count() const {
    return FCEUMOV_GetFrame();
}

void Core::run_frame() {
    run_frame(Buttons {});
}

void Core::run_frame(Buttons buttons) {
    gamepad_data_ = buttons.value();

    u8* xbuf;
    i32* soundbuf;
    i32 soundbuf_size;
    FCEUI_Emulate(&xbuf, &soundbuf, &soundbuf_size, 0);
}

void Core::run_frames(int n) {
    run_frames(n, Buttons {});
}

void Core::run_frames(int n, Buttons buttons) {
    LOOP(n) { run_frame(buttons); }
}

u8 Core::read_u8(u16 addr) {
    return GetMem(addr);
}

void Core::write_u8(u16 addr, u8 value) {
    BWrite[addr](addr, value);
}

void Core::snapshot_load(Snapshot& snapshot) {
    auto& file = snapshot.impl_->file_;

    file.fseek(0, SEEK_SET);
    if (!FCEUSS_LoadFP(&file, SSLOADPARAM_NOBACKUP))
        PANIC("FCEUSS_LoadFP() failed");
}

void Core::snapshot_save(Snapshot& snapshot) const {
    snapshot.impl_->clear();

    auto& file = snapshot.impl_->file_;
    if (!FCEUSS_SaveMS(&file, Z_NO_COMPRESSION))
        PANIC("FCEUSS_SaveMS() failed");
}

void Core::unhook_before_exec(HookHandle handle) {
    RemoveHookBeforeExec(handle.id_);
}

void Core::clear_hooks_before_exec() {
    ClearHookBeforeExec();
}
