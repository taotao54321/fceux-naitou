#pragma once

#include <memory>
#include <string>

#include <boost/core/noncopyable.hpp>

#include "prelude.hpp"

class Buttons {
private:
    u8 value_ { 0 }; // RLDUTSBA

public:
    [[nodiscard]] u8 value() const;

    [[nodiscard]] bool A() const;
    [[nodiscard]] bool B() const;
    [[nodiscard]] bool S() const;
    [[nodiscard]] bool T() const;
    [[nodiscard]] bool U() const;
    [[nodiscard]] bool D() const;
    [[nodiscard]] bool L() const;
    [[nodiscard]] bool R() const;

    Buttons& A(bool on);
    Buttons& B(bool on);
    Buttons& S(bool on);
    Buttons& T(bool on);
    Buttons& U(bool on);
    Buttons& D(bool on);
    Buttons& L(bool on);
    Buttons& R(bool on);

    Buttons& flipA();
    Buttons& flipB();
    Buttons& flipS();
    Buttons& flipT();
    Buttons& flipU();
    Buttons& flipD();
    Buttons& flipL();
    Buttons& flipR();
};

// 事前に Snapshot オブジェクトを確保し、それに対して save/load を行う (Lua API と同じ)。
// セーブ時に毎回内部バッファを確保するのは遅いので。

class SnapshotImpl;

class Snapshot {
private:
    std::shared_ptr<SnapshotImpl> impl_;

    friend class Core;

public:
    Snapshot();
};

class Core : private boost::noncopyable {
private:
    u32 gamepad_data_ { 0 };

public:
    explicit Core(const std::string& path_rom);

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
};
