#ifndef __GRAB_H__
#define __GRAB_H__

#include <iostream>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <webcam.h>
#include <raylib.h>
#include <MathCore.cpp>

#define XRES (640/2)
#define YRES (480/2)

Color white_filter(Color c)
{
    int sum = c.b+c.r+c.g;
    if(sum>3*200)
        return PINK;
    else
        return c;
}

float directionControl(matrix *m,float max_angle,bool draw)
{
   int dl = 0, dr =0;
   int h = m->h/2 , w = m->w/2;

   for(int i=w;i>0;i--)
   {
       if(m->body[h][i]) break;
       dl++;
   }

   for(int i=w;i<m->w;i++)
   {
       if(m->body[h][i]) break;
       dr++;
   }

   float angle = max_angle * (float) (dr-dl) / (float) (w);

   if(draw)
   {
       char buff[50];
       sprintf(buff,"Angle : %.2f",angle);
     DrawText(buff,w,h-30,20,GREEN);
     DrawCircle(w,h,5,RED);
     DrawLine(w,m->h,w,h,RED);
     DrawLine(w-dl,h,w+dr,h,YELLOW);
     DrawLine(w,h,w+(dr-dl),h,GREEN);
     DrawLine(w,m->h,w+(dr-dl),h,GREEN);
     DrawCircle(w+(dr-dl),h,5,GREEN);
   }

   return angle;
}

Color toGreyScale(Color);
void makeScreenshot(const RGBImage*, const char*);

struct Frame
{
    size_t w;
    size_t h;
    Color **data;

    void draw()
    {
     for(size_t i=0;i<h;i++)
            for(size_t j=0;j<w;j++)
            {
            DrawPixel(j,i,data[i][j]);
            }
    }


    void diff(Frame &frame)
    {
        if((w==frame.w)&&(h==frame.h))
        {
            for(size_t i=0;i<h;i++)
            {
                for(size_t j=0;j<w;j++)
                {
                    if(memcmp(&data[i][j],&frame.data[i][j],sizeof(Color))==0)
                    {
                        data[i][j]=BLACK;
                    }
                }
            }
        }

    }

    Frame(const RGBImage &image)
    {
        data = new Color*[image.height];
        w = image.width;
        h = image.height;
        for(size_t i=0;i<image.height;i++)
        {
            data[i]= new Color[w];
            for(size_t j=0;j<image.width;j++)
            {
                size_t k = 3*(i*image.width+j);
                data[i][j].r = image.data[k];
                data[i][j].g = image.data[k+1];
                data[i][j].b = image.data[k+2];
                data[i][j].a = 255;
            }
        }
     }
    ~Frame()
    {
        for(size_t i=0;i<h;i++)
        {
            delete[] data[i];
        }
        delete[] data;
     }
};


#endif
