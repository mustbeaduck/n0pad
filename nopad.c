#include "nopad.h"

#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#include "stdio.h"

#define and &&
#define not !
#define or ||

const wchar_t * g_dvd[] = {
    L"⠀⢰⣶⠶⢶⣶⣶⣆⠀⣰⣶⣶⡶⢶⣦⡄",
    L"⠀⣾⣿⣀⣼⡿⠛⣿⣾⠟⣹⣿⣀⣤⡿⠃",
    L"⠀⢉⣉⣉⣁⣠⣤⣽⣥⣤⣉⣉⣉⡁⠀⠀",
    L"⠘⠛⠛⠻⠿⠿⠶⠶⠾⠿⠿⠿⠛⠛⠋ ",
};

typedef struct DotImage {
   const wchar_t ** img;
   int height;
   int width;
} DotImage;

DotImage g_image = {g_dvd, 4, 16};


static void resetField(Field * self)
{
   int paddedWidth = self->width + 2;
   wmemset(self->dataBuffer, self->space, self->height * paddedWidth);
   
   wchar_t * itr = self->dataBuffer;
   //place \r\n at the end of every line
   for ( int i = 0; i < self->height; i+=1 ) {
      itr += self->width;
      *itr = L'\r'; 
      ++itr;
      *itr = L'\n'; 
      ++itr; 
   }   
}


Field * newField(int width, int height, wchar_t space)
{
   if (width > 0 and height > 0 ) {
   
      Field * self = malloc(sizeof(Field));

      self->height = height;
      self->width = width;
      self->space = space;
      
      if ( self != NULL ) 
      {

         self->data = malloc(height * sizeof(wchar_t*));

         if ( self->data != NULL ) {
            
            int paddedWidth = width + 2;
            self->dataBuffer = malloc(paddedWidth*height*sizeof(wchar_t));

            if ( self->dataBuffer != NULL ) {

               wchar_t * rowPointer = self->dataBuffer;

               //map single chunk of memory to be used as a dynamic array 
               for ( int i = 0; i < height; i+=1 ) {

                  self->data[i] = rowPointer;
                  rowPointer += paddedWidth;

               } 

               resetField(self);

               return self;

            } else {
               free(self->data);
            }
         }
      }
   }

   return NULL;
}


static void clearVertical(Field * self, int x, int y, int height) 
{
   for ( int i = 0; i < height; i+=1 ) {
      self->data[y][x] = self->space; 
      ++y;
   }
}

static void renderImage(Field * self, DotImage * image, int x, int y)
{
   for ( int i = 0; i < image->height; i+=1 ) {
      memcpy(self->data[y]+x, image->img[i], image->width*sizeof(wchar_t));
      ++y;
   }
}


void * freeField(Field * self)
{
   free(self->dataBuffer);
   free(self->data);
   free(self);
}

void initAnimation(Field * field, PosData * pos)
{
   pos->y = field->height/2 - g_image.height/2;
   pos->x = field->width/2 - g_image.width/2;

   pos->y1 = pos->y + g_image.height - 1;
   pos->x1 = pos->x + g_image.width - 1;

   pos->movesRight = true;
   pos->movesUp = false;

   renderImage(field, &g_image, pos->x, pos->y);
}

void animateNextFrame(Field * field, PosData * pos)
{


   if ( pos->movesRight ) {

      clearVertical(field, pos->x, pos->y, g_image.height);

      ++pos->x; ++pos->x1;

      if ( pos->x1 == field->width - 1  ) 
         pos->movesRight = false;
   } else {

      clearVertical(field, pos->x1, pos->y, g_image.height);

      --pos->x; --pos->x1;

      if ( pos->x == 0 ) 
         pos->movesRight = true;
   }  

   if ( pos->movesUp ) {

      wmemset(field->data[pos->y1]+pos->x, field->space, g_image.width);

      --pos->y1; --pos->y;

      if ( pos->y == 0 ) 
         pos->movesUp = false;
   } else {

      wmemset(field->data[pos->y]+pos->x, field->space, g_image.width);
      
      ++pos->y1; ++pos->y;

      if ( pos->y1 == field->height - 1 ) 
         pos->movesUp = true;
   }
   
   renderImage(field, &g_image, pos->x, pos->y);
}
