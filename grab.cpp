#include <iostream>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include "webcam.h"
#include "ppm2bmp.h"
#include "raylib.h"


#define XRES (640/2)
#define YRES (480/2)

using namespace std;

struct Frame
{
    size_t w;
    size_t h;
    Color **data;
    
    void draw()
    {
     for(int i=0;i<h;i++)
            for(int j=0;j<w;j++)
            {
            DrawPixel(j,i,data[i][j]);
            }   
    }
    
    
    void diff(Frame &frame)
    {
        if((w==frame.w)&&(h==frame.h))
        {
            for(int i=0;i<h;i++)
            {
                for(int j=0;j<w;j++)
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
        for(int i=0;i<image.height;i++)
        {
            data[i]= new Color[w];
            for(int j=0;j<image.width;j++)
            {
                int k = 3*(i*image.width+j);
                data[i][j].r = image.data[k];
                data[i][j].g = image.data[k+1];
                data[i][j].b = image.data[k+2];
                data[i][j].a = 255;
            }
        }
     }
    ~Frame()
    {
        for(int i=0;i<h;i++)
        {
            delete[] data[i];
        }
        delete[] data;
     }
};

Color toGreyScale(Color color)
{
    int avg = (30*color.r+40*color.g+30*color.b)/100;
    return (Color){avg,avg,avg,255};
}

int main(int argc, char** argv)
{
    const int screenWidth = XRES;
    const int screenHeight = YRES;

    InitWindow(screenWidth, screenHeight, "Camera monitor [Rapter maker]");

    SetTargetFPS(60);    
       
    
    Webcam webcam("/dev/video0", XRES, YRES);

    RGBImage *frame_ptr = nullptr;
    
    
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        RGBImage *frame_save = &webcam.frame();
        Frame frame_back(*frame_save);
        frame_save = &webcam.frame();
        Frame frame(*frame_save);
        
        frame.diff(frame_back);
        
        
        if(IsKeyDown(KEY_S))
        {
            ofstream image;
            image.open("frame.ppm");
            image << "P6\n" << XRES << " " << YRES << " 255\n";
            image.write((char *) frame_save->data, frame_save->size);
            image.close();
        }
        
        BeginDrawing();

            ClearBackground(BLACK);

            frame.draw();
            
         DrawFPS(10,10);
        EndDrawing();
    }

    CloseWindow(); 
    
    
    
    /*
    
    RGBImage &frame = *frame_ptr;
    
    ofstream image;
    image.open("frame.ppm");
    image << "P6\n" << XRES << " " << YRES << " 255\n";
    image.write((char *) frame.data, frame.size);
    image.close();
    
    //ppm2bmp("frame.ppm","example.bmp");
    
    
    cin.get();
    */
    return 0;

}
