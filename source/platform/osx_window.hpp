#pragma once

#include <SDL_syswm.h>

#include <cstdint>

namespace tse {
std::uintptr_t cocoaContentView(const SDL_SysWMinfo &windowInfo);
}

