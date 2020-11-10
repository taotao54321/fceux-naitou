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
// util
//--------------------------------------------------------------------

namespace {

template <class Integer>
Integer BIT_I(int i) {
    return Integer(1) << i;
}

template <class Integer>
Integer BIT_GET(Integer x, int i) {
    return x & BIT_I<Integer>(i);
}

template <class Integer>
bool BIT_TEST(Integer x, int i) {
    return bool(BIT_GET(x, i));
}

template <class Integer>
Integer BIT_SET(Integer x, int i) {
    return x | BIT_I<Integer>(i);
}

template <class Integer>
Integer BIT_UNSET(Integer x, int i) {
    return x & ~BIT_I<Integer>(i);
}

template <class Integer>
Integer BIT_ASSIGN(Integer x, int i, bool on) {
    return on ? BIT_SET(x, i) : BIT_UNSET(x, i);
}

template <class Integer>
Integer BIT_FLIP(Integer x, int i) {
    return x ^ BIT_I<Integer>(i);
}

} // anonymous namespace

//--------------------------------------------------------------------
// Buttons
//--------------------------------------------------------------------

u8 Buttons::value() const { return value_; }

bool Buttons::A() const { return BIT_TEST(value_, 0); }
bool Buttons::B() const { return BIT_TEST(value_, 1); }
bool Buttons::S() const { return BIT_TEST(value_, 2); }
bool Buttons::T() const { return BIT_TEST(value_, 3); }
bool Buttons::U() const { return BIT_TEST(value_, 4); }
bool Buttons::D() const { return BIT_TEST(value_, 5); }
bool Buttons::L() const { return BIT_TEST(value_, 6); }
bool Buttons::R() const { return BIT_TEST(value_, 7); }

Buttons& Buttons::A(bool on) {
    value_ = BIT_ASSIGN(value_, 0, on);
    return *this;
}
Buttons& Buttons::B(bool on) {
    value_ = BIT_ASSIGN(value_, 1, on);
    return *this;
}
Buttons& Buttons::S(bool on) {
    value_ = BIT_ASSIGN(value_, 2, on);
    return *this;
}
Buttons& Buttons::T(bool on) {
    value_ = BIT_ASSIGN(value_, 3, on);
    return *this;
}
Buttons& Buttons::U(bool on) {
    value_ = BIT_ASSIGN(value_, 4, on);
    return *this;
}
Buttons& Buttons::D(bool on) {
    value_ = BIT_ASSIGN(value_, 5, on);
    return *this;
}
Buttons& Buttons::L(bool on) {
    value_ = BIT_ASSIGN(value_, 6, on);
    return *this;
}
Buttons& Buttons::R(bool on) {
    value_ = BIT_ASSIGN(value_, 7, on);
    return *this;
}

Buttons& Buttons::flipA() {
    value_ = BIT_FLIP(value_, 0);
    return *this;
}
Buttons& Buttons::flipB() {
    value_ = BIT_FLIP(value_, 1);
    return *this;
}
Buttons& Buttons::flipS() {
    value_ = BIT_FLIP(value_, 2);
    return *this;
}
Buttons& Buttons::flipT() {
    value_ = BIT_FLIP(value_, 3);
    return *this;
}
Buttons& Buttons::flipU() {
    value_ = BIT_FLIP(value_, 4);
    return *this;
}
Buttons& Buttons::flipD() {
    value_ = BIT_FLIP(value_, 5);
    return *this;
}
Buttons& Buttons::flipL() {
    value_ = BIT_FLIP(value_, 6);
    return *this;
}
Buttons& Buttons::flipR() {
    value_ = BIT_FLIP(value_, 7);
    return *this;
}

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
