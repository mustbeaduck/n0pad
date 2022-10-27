#ifndef __NOPAD__HEADER__
#define __NOPAD__HEADER__

#include "wchar.h"
#include "stdbool.h"

typedef struct Field {

   int width;
   int height;

   wchar_t ** data;
   wchar_t * dataBuffer;

   wchar_t space;
   
} Field;


typedef struct PosData {

   bool movesRight;
   bool movesUp;
   
   int x1;
   int y1;

   int x;
   int y;

} PosData;


Field * newField(int width, int height, wchar_t space);
void animateNextFrame(Field * field, PosData * pos);
void initAnimation(Field * field, PosData * pos);
void * freeField(Field * self);

#endif
