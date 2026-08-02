#include "platform/osx_window.hpp"

#import <AppKit/NSWindow.h>

namespace tse {
std::uintptr_t cocoaContentView(const SDL_SysWMinfo &windowInfo) {
  NSWindow *window = windowInfo.info.cocoa.window;
  return reinterpret_cast<std::uintptr_t>([window contentView]);
}
} // namespace tse

