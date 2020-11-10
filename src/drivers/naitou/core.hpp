#pragma once

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include <boost/core/noncopyable.hpp>

#include "driver.hpp"
#include "prelude.hpp"
#include "util.hpp"

class Buttons {
private:
    u8 value_ { 0 }; // RLDUTSBA

public:
    constexpr Buttons() = default;

    [[nodiscard]] constexpr u8 value() const { return value_; }

    [[nodiscard]] constexpr bool is_empty() const { return value_ == 0; }

    [[nodiscard]] constexpr bool A() const { return BIT_TEST(value_, 0); }
    [[nodiscard]] constexpr bool B() const { return BIT_TEST(value_, 1); }
    [[nodiscard]] constexpr bool S() const { return BIT_TEST(value_, 2); }
    [[nodiscard]] constexpr bool T() const { return BIT_TEST(value_, 3); }
    [[nodiscard]] constexpr bool U() const { return BIT_TEST(value_, 4); }
    [[nodiscard]] constexpr bool D() const { return BIT_TEST(value_, 5); }
    [[nodiscard]] constexpr bool L() const { return BIT_TEST(value_, 6); }
    [[nodiscard]] constexpr bool R() const { return BIT_TEST(value_, 7); }

    [[nodiscard]] constexpr Buttons& clear() {
        value_ = 0;
        return *this;
    }

    constexpr Buttons& A(bool on) {
        value_ = BIT_ASSIGN(value_, 0, on);
        return *this;
    }
    constexpr Buttons& B(bool on) {
        value_ = BIT_ASSIGN(value_, 1, on);
        return *this;
    }
    constexpr Buttons& S(bool on) {
        value_ = BIT_ASSIGN(value_, 2, on);
        return *this;
    }
    constexpr Buttons& T(bool on) {
        value_ = BIT_ASSIGN(value_, 3, on);
        return *this;
    }
    constexpr Buttons& U(bool on) {
        value_ = BIT_ASSIGN(value_, 4, on);
        return *this;
    }
    constexpr Buttons& D(bool on) {
        value_ = BIT_ASSIGN(value_, 5, on);
        return *this;
    }
    constexpr Buttons& L(bool on) {
        value_ = BIT_ASSIGN(value_, 6, on);
        return *this;
    }
    constexpr Buttons& R(bool on) {
        value_ = BIT_ASSIGN(value_, 7, on);
        return *this;
    }

    constexpr Buttons& flipA() {
        value_ = BIT_FLIP(value_, 0);
        return *this;
    }
    constexpr Buttons& flipB() {
        value_ = BIT_FLIP(value_, 1);
        return *this;
    }
    constexpr Buttons& flipS() {
        value_ = BIT_FLIP(value_, 2);
        return *this;
    }
    constexpr Buttons& flipT() {
        value_ = BIT_FLIP(value_, 3);
        return *this;
    }
    constexpr Buttons& flipU() {
        value_ = BIT_FLIP(value_, 4);
        return *this;
    }
    constexpr Buttons& flipD() {
        value_ = BIT_FLIP(value_, 5);
        return *this;
    }
    constexpr Buttons& flipL() {
        value_ = BIT_FLIP(value_, 6);
        return *this;
    }
    constexpr Buttons& flipR() {
        value_ = BIT_FLIP(value_, 7);
        return *this;
    }
};

// 事前に Snapshot オブジェクトを確保し、それに対して save/load を行う (Lua API と同じ)。
// セーブ時に毎回内部バッファを確保するのは遅いので。

class SnapshotImpl;

class Snapshot : private boost::noncopyable {
private:
    std::shared_ptr<SnapshotImpl> impl_;

    friend class Core;

public:
    Snapshot();
};

// フック解除用
class HookHandle {
private:
    int id_;

    explicit HookHandle(int id);

    friend class Core;
};

class Core : private boost::noncopyable {
private:
    u32 gamepad_data_ { 0 };

public:
    explicit Core(const std::string& path_rom);

    [[nodiscard]] int frame_count() const;

    // 無入力で 1 フレーム進める。
    void run_frame();

    // 入力 buttons で 1 フレーム進める。
    void run_frame(Buttons buttons);

    // 無入力で n フレーム進める。
    void run_frames(int n);

    // 入力 buttons で n フレーム進める。
    void run_frames(int n, Buttons buttons);

    u8 read_u8(u16 addr);

    template <size_t N>
    void read_bytes(u16 addr, u8 (&buf)[N]) {
        read_bytes(addr, N, buf);
    }

    template <size_t N>
    void read_bytes(u16 addr, std::array<u8, N>& buf) {
        read_bytes(addr, N, buf.data());
    }

    template <class OutputIt>
    void read_bytes(u16 addr, const std::size_t size, OutputIt first) {
        LOOP(size) {
            *first++ = read_u8(addr++);
        }
    }

    void write_u8(u16 addr, u8 value);

    template <class InputIt>
    void write_bytes(u16 addr, InputIt first, InputIt last) {
        for (; first != last; ++first)
            write_u8(addr++, *first);
    }

    void snapshot_load(Snapshot& snapshot);

    void snapshot_save(Snapshot& snapshot) const;

    template <class F>
    HookHandle hook_before_exec(u16 addr, F&& f) {
        return HookHandle(AddHookBeforeExec(addr, std::function<void()>(std::forward<F>(f))));
    }

    void unhook_before_exec(HookHandle handle);

    void clear_hooks_before_exec();
};
