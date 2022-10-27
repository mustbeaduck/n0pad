#include "inttypes.h"
#include "Windows.h"
#include "stdbool.h"
#include "winuser.h"
#include "pthread.h"
#include "stdio.h"

#include "nopad.h"
#include "blockinput.h"

#define EM_SETZOOM 0x4e1

#define HW_EDIT_OFFS 0x335c8


bool run = true;
HINSTANCE dllHandle;


typedef struct NpadData {

    int * pLen;
    wchar_t ** pBuff;

} NpadData;

NpadData npData;


int oldLen;
wchar_t * oldBuff;


void * aWayOut(void * par)
{
    system("pause");
    run = false;
}


void setupBuffer(uintptr_t ewObj, Field * field)
{
   npData.pBuff = *( wchar_t *** ) ewObj;
   npData.pLen = ( int * ) (ewObj + sizeof(int) * 4);
   
   oldLen = *npData.pLen;
   oldBuff = *npData.pBuff;

   size_t len = field->height * (field->width+2);

   if ( *npData.pLen > len ) {
      *npData.pLen = len; 
      *npData.pBuff = field->dataBuffer;
   } else {
      *npData.pBuff = field->dataBuffer;
      *npData.pLen = len; 
   }
}


void restoreBuffer()
{
   if ( *npData.pLen > oldLen ) {
      *npData.pLen = oldLen;
      *npData.pBuff = oldBuff;
   } else {
      *npData.pBuff = oldBuff;
      *npData.pLen = oldLen;
   }
}


void entryPoint()
{
   AllocConsole();
   FILE * myStdout;
   freopen_s(&myStdout, "CONOUT$", "w", stdout);

   uintptr_t baseAddr = ( uintptr_t ) GetModuleHandle(0);
   
   HWND hwEdit = * (HWND * ) (baseAddr + HW_EDIT_OFFS);

   blockSomeInput(baseAddr, hwEdit);

   //edit-window data
   uintptr_t ewObj = (IsWindowUnicode(hwEdit) ? GetWindowLongPtrW : GetWindowLongPtrA)(hwEdit, 0);

   Field * field = newField(200, 50, L' ');
   setupBuffer(ewObj, field);
   
   //trigger newlines render
   SendMessageW(hwEdit, EM_SETZOOM, 1, 1);
   
   //a way out of the loop
   pthread_t th;
   if ( pthread_create(&th, NULL, aWayOut, NULL) ) {
     fprintf(stderr, "something went wrong while creating a thread\n");
     return;
   } 
   
   PosData pos;
   initAnimation(field, &pos);

   while ( run ) {
      animateNextFrame(field, &pos);
      RedrawWindow(hwEdit, NULL, NULL,  RDW_INVALIDATE);
      Sleep(100);
   }  


   pthread_join(th, NULL);

   restoreBuffer();
   freeField(field);
   restoreInput();

   fclose(myStdout);
   FreeConsole();
   FreeLibraryAndExitThread(dllHandle, 0);
}


bool __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
   if ( fdwReason == DLL_PROCESS_ATTACH ) {

      dllHandle = hinstDLL;
      CloseHandle(CreateThread(NULL, (SIZE_T)NULL, (LPTHREAD_START_ROUTINE)entryPoint, NULL, 0, NULL));

      return true; 
    }
    
   return false;
} 
