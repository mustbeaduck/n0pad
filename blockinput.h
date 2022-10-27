#ifndef __BLOCKINPUT__HEADER__
#define __BLOCKINPUT__HEADER__

#include "stdint.h"
#include "Windows.h"

void blockSomeInput(uintptr_t baseAddr, HWND hwEdit);
void restoreInput();

#endif
