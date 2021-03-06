#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <functional>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "driver.h"
#include "emufile.h"
#include "file.h"
#include "git.h"
#include "types.h"

#include "driver.hpp"
#include "prelude.hpp"

// これらは定数
int KillFCEUXonFrame = 0;
int closeFinishedMovie = 0;
int dendy = 0;
int pal_emulation = 0;
bool swapDuty = false;
bool turbo = false;

namespace {

int is_loaded = 0;

struct HookExec {
    int id;
    u16 addr;
    std::function<void()> f;
    HookExec(int id, u16 addr, std::function<void()> f)
        : id(id)
        , addr(addr)
        , f(std::move(f)) {}
};

std::vector<HookExec> hooks_before_exec;

int gen_hook_id() {
    static int id = 0;
    return id++;
}

} // namespace anonymous

//--------------------------------------------------------------------
// hook
//--------------------------------------------------------------------

void FCEUD_CallHookBeforeExec(const u16 addr) {
    for (const auto& hook : hooks_before_exec) {
        if (hook.addr == addr)
            hook.f();
    }
}

int AddHookBeforeExec(const u16 addr, std::function<void()> f) {
    const int id = gen_hook_id();
    hooks_before_exec.emplace_back(id, addr, f);
    return id;
}

void RemoveHookBeforeExec(const int id) {
    using std::begin, std::end;

    const auto first = begin(hooks_before_exec);
    const auto last = end(hooks_before_exec);

    const auto it = std::find_if(first, last, [id](const auto& hook) { return hook.id == id; });
    if (it == last) PANIC("RemoveHookBeforeExec(): invalid id: {}", id);

    hooks_before_exec.erase(it);
}

void ClearHookBeforeExec() {
    hooks_before_exec.clear();
}

//--------------------------------------------------------------------
// message
//--------------------------------------------------------------------

void FCEUD_PrintError(const char* s) {
    std::fputs(s, stderr);
    std::putc('\n', stderr);
}

void FCEUD_Message(const char* s) {
    std::fputs(s, stderr);
}

//--------------------------------------------------------------------
// lua
//--------------------------------------------------------------------

void WinLuaOnStart(intptr_t) {}
void WinLuaOnStop(intptr_t) {}

int LuaKillMessageBox() {
    return 0;
}

void PrintToWindowConsole(intptr_t, const char* str) {
    std::puts(str);
}

int LuaPrintfToWindowConsole(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int res = vprintf(format, args);
    va_end(args);
    return res;
}

//--------------------------------------------------------------------
// file I/O
//--------------------------------------------------------------------

std::FILE* FCEUD_UTF8fopen(const char* fn, const char* mode) {
    return std::fopen(fn, mode);
}

EMUFILE_FILE* FCEUD_UTF8_fstream(const char* n, const char* m) {
    return new EMUFILE_FILE(n, m);
}

FCEUFILE* FCEUD_OpenArchiveIndex(ArchiveScanRecord&, std::string&, int) {
    return nullptr;
}

FCEUFILE* FCEUD_OpenArchiveIndex(ArchiveScanRecord&, std::string&, int, int*) {
    return nullptr;
}

FCEUFILE* FCEUD_OpenArchive(ArchiveScanRecord&, std::string&, std::string*) {
    return nullptr;
}

FCEUFILE* FCEUD_OpenArchive(ArchiveScanRecord&, std::string&, std::string*, int*) {
    return nullptr;
}

ArchiveScanRecord FCEUD_ScanArchive(std::string) {
    return {};
}

//--------------------------------------------------------------------
// load
//--------------------------------------------------------------------

int LoadGame(const char* path, bool silent) {
    if (is_loaded != 0) FCEUI_CloseGame();

    if (!FCEUI_LoadGame(path, 1, silent)) return 0;

    is_loaded = 1;

    return 1;
}

int reloadLastGame() {
    return 0;
}

//--------------------------------------------------------------------
// video
//--------------------------------------------------------------------

void FCEUD_SetPalette(uint8, uint8, uint8, uint8) {}

void FCEUD_GetPalette(uint8, uint8* r, uint8* g, uint8* b) {
    *r = *b = *g = 0;
}

//--------------------------------------------------------------------
// netplay
//--------------------------------------------------------------------

int FCEUD_SendData(void*, uint32) {
    return 0;
}

int FCEUD_RecvData(void*, uint32) {
    return 0;
}

void FCEUD_NetplayText(uint8*) {}

void FCEUD_NetworkClose() {}

//--------------------------------------------------------------------
// sound
//--------------------------------------------------------------------

void FCEUD_SoundToggle() {}
void FCEUD_SoundVolumeAdjust(int) {}

//--------------------------------------------------------------------
// savestate
//--------------------------------------------------------------------

void FCEUD_SaveStateAs() {}
void FCEUD_LoadStateFrom() {}

//--------------------------------------------------------------------
// input
//--------------------------------------------------------------------

void FCEUI_UseInputPreset(int) {}

void FCEUD_SetInput(bool fourscore, bool microphone, ESI port0, ESI port1, ESIFC fcexp) {}

bool FCEUD_ShouldDrawInputAids() {
    return false;
}

void GetMouseData(uint32 (&md)[3]) {
    md[0] = 0;
    md[1] = 0;
    md[2] = 0;
}

unsigned int* GetKeyboard() {
    return nullptr;
}

//--------------------------------------------------------------------
// movie
//--------------------------------------------------------------------

void FCEUD_MovieRecordTo() {}
void FCEUD_MovieReplayFrom() {}

//--------------------------------------------------------------------
// avi
//--------------------------------------------------------------------

void FCEUI_AviVideoUpdate(const unsigned char*) {}

bool FCEUI_AviEnableHUDrecording() {
    return false;
}

bool FCEUI_AviDisableMovieMessages() {
    return false;
}

bool FCEUI_AviIsRecording() {
    return false;
}

void FCEUD_AviRecordTo() {}
void FCEUD_AviStop() {}

int FCEUD_ShowStatusIcon() {
    return 0;
}
void FCEUD_ToggleStatusIcon() {}
void FCEUD_HideMenuToggle() {}

void FCEUD_DebugBreakpoint(int) {}
void FCEUD_TraceInstruction(uint8*, int) {}
void FCEUD_UpdateNTView(int, bool) {}
void FCEUD_UpdatePPUView(int, int) {}

bool FCEUD_PauseAfterPlayback() {
    return false;
}

void FCEUD_VideoChanged() {}

//--------------------------------------------------------------------
// time
//--------------------------------------------------------------------

uint64 FCEUD_GetTime() {
    using Clock = std::chrono::steady_clock;

    static const auto START = Clock::now();

    const auto now = Clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - START).count();
}

uint64 FCEUD_GetTimeFreq() {
    return 1000;
}

void FCEUD_SetEmulationSpeed(int) {}
void FCEUD_TurboOn() {}
void FCEUD_TurboOff() {}
void FCEUD_TurboToggle() {}

void RefreshThrottleFPS() {}

//--------------------------------------------------------------------
// misc
//--------------------------------------------------------------------

const char* FCEUD_GetCompilerString() {
    return "g++";
}
