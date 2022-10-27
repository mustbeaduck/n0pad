#include "blockinput.h"

#define NP_HWND_OFFS 0x335d8

typedef struct InpSrc {
   HMENU menu;
   HHOOK mouseHook;
   HHOOK kbdHook;
} InpSrc;

InpSrc g_inps;


LRESULT dodgeRightClicks(int nCode, WPARAM wParam, LPARAM lParam)
{
   if ( nCode == HC_ACTION ) {
     if ( wParam == WM_RBUTTONDOWN ) return 1;
   }

   return CallNextHookEx(0, nCode, wParam, lParam);    
}


LRESULT dodgeKeyboard(int nCode, WPARAM wParam, LPARAM lParam)
{
   return 1;
}


void blockSomeInput(uintptr_t baseAddr, HWND hwEdit)
{
   //get a window handle
   HWND nphwnd = *( HWND * ) ( baseAddr + NP_HWND_OFFS );
   g_inps.menu = GetMenu(nphwnd);

   //disable edit sub-menu
   EnableMenuItem(g_inps.menu, 1, MF_BYPOSITION | MF_GRAYED);
   DWORD thread_id = GetWindowThreadProcessId(hwEdit, NULL);

   //set da hooks
   g_inps.mouseHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)dodgeRightClicks, NULL, thread_id);
   g_inps.kbdHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)dodgeKeyboard, NULL, thread_id);
}   


void restoreInput()
{
   EnableMenuItem(g_inps.menu, 1, MF_BYPOSITION | MF_ENABLED);
   UnhookWindowsHookEx(g_inps.mouseHook);
   UnhookWindowsHookEx(g_inps.kbdHook);
}
