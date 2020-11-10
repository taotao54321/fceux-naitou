#pragma once

#include <functional>

#include "prelude.hpp"

int LoadGame(const char* path, bool silent);

// Lua API の memory.registerexec() フックに相当。
// とりあえずナイーブな実装とする。フックの数や呼び出し頻度はたかが知れているので。
int AddHookBeforeExec(u16 addr, std::function<void()> f);
void RemoveHookBeforeExec(int id);
void ClearHookBeforeExec();
