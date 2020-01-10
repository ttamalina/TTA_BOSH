#include <iostream>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>

#include <webcam.h>
#include <raylib.h>
#include <grab.h>

using namespace std;

int main(int argc, char** argv)
{
    const int screenWidth = XRES;
    const int screenHeight = YRES;
    const RGBImage *frame_ptr = nullptr;
    Webcam webcam("/dev/video0", XRES, YRES);
    InitWindow(screenWidth, screenHeight, "Camera monitor");
    SetTargetFPS(60);    
    while (!WindowShouldClose())
    {
        frame_ptr = &webcam.frame();
        Frame frame(*frame_ptr);
        if(IsKeyDown(KEY_S))
        {
            makeScreenshot(frame_ptr, "frame.ppm");
        }

        BeginDrawing();
            ClearBackground(BLACK);
            frame.draw();
            DrawFPS(10,10);
        EndDrawing();
    }

    CloseWindow(); 
    return 0;
}
