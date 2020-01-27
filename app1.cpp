#include <serialPortHandler.hpp>
#include <SerialComManager.hpp>
#include <MessageHandler.hpp>
#include <Message.hpp>
#include <boost/function.hpp>
#include <example1.hpp>


#include "CannyEdgeDetector.h"

#include <webcam.h>
#include <raylib.h>
#include <grab.h>

#include <thread>

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

Color white_filter(Color c)
{
    int sum = c.b+c.r+c.g;
    if(sum>3*200)
        return PINK;
    else
        return c;
}

void print(std::string str)
{
    std::cout<<str<<std::endl;
}

void cameraWindowTask(void)
{
    const RGBImage *frame_ptr = nullptr;
    Webcam webcam("/dev/video0", XRES, YRES);

    const int camx = XRES;
    const int camy = YRES;
    const int screenWidth = XRES;
    const int screenHeight = YRES;
    InitWindow(screenWidth, screenHeight, "Camera monitor");
    SetTargetFPS(30);
    Color * pixel_buffer = new Color[XRES*YRES];
    Clock cpu_benchmark;
    int cpu_fps = 0,benchmark_contor = 0;
    float frame_work = 0, draw_work = 0;
    char text_buff[256];
    CannyEdgeDetector edge_detector;

    uint8_t * pix_buf = new uint8_t[camx*camy*3];

    while (!WindowShouldClose())
    {
        frame_ptr = &webcam.frame();
        if(IsKeyDown(KEY_S))
        {
            makeScreenshot(frame_ptr, "frame.ppm");
        }

        Clock frame_timer;

        frame *f = new frame(*frame_ptr);
        f->setColorBuffer(pixel_buffer);
        float ration = 0.15;
        int camx_new = ration*camx, camy_new = camy*ration;
        matrix * grey = f->toGreyScaleBuffer();
        grey->setSize(camx_new,camy_new);
        int normal_light = grey->max();
        normal_light-=normal_light/5;
        grey->threshold(normal_light);
        grey->setPixelBuffer(pix_buf);
        //uint8_t * work = edge_detector.ProcessImage(pix_buf,camx_new,camy_new,1.4,20,80);
        matrix * temp = grey;//new matrix(pix_buf,0,camx_new,camy_new);
        //delete grey;

        temp->setSize(camx,camy);
        for(int i=0;i<temp->h;i++)
        {
            for(int j=0;j<temp->w;j++)
            {
                if(temp->body[i][j])
                {
                    pixel_buffer[i*temp->w+j] = PINK;
                }
            }
        }
        Clock draw_timer;
        Image image = LoadImageEx(pixel_buffer,grey->w,grey->h);
        Texture2D texture = LoadTextureFromImage(image);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture,screenWidth/2-temp->w/2,screenHeight/2-temp->h/2,WHITE);
        directionControl(grey,20,true);
        sprintf(text_buff,"CPU FPS : %d\nFrame time = %.3fs\nDraw time = %.3fs\n",cpu_fps,frame_timer.getElapsed(),draw_timer.getElapsed());
        DrawText(text_buff,10,50,30,RED);
        DrawFPS(10,10);
        EndDrawing();
        UnloadImage(image);
        UnloadTexture(texture);
        delete temp;
        delete f;

       if(cpu_benchmark.isElapsed(1.0))
       {
           cpu_fps = benchmark_contor;
           cpu_benchmark.update();
           benchmark_contor=0;
       }
       benchmark_contor++;

    }

    CloseWindow();
    delete[] pix_buf;
    delete[] pixel_buffer;

}

int main(int argc, char* argv[])
{
    std::thread t1(cameraWindowTask);
    try
    {
        // Create a resnponse handler object
        ResponseHandler  	l_responseHandler;
        // Create a communication manager object
        SerialComManager 	l_communicationManager(l_responseHandler);


        // Create a callback object for response handling
        ResponseHandler::CallbackFncPtrType l_callbackFncObj=ResponseHandler::createCallbackFncPtr(&print);
        // Attach object to Move and Brake response
        l_responseHandler.attach(message::MCTL,l_callbackFncObj);
        l_responseHandler.attach(message::BRAK,l_callbackFncObj);
        // Create a move object
        CMoveExample		l_moveObj(l_communicationManager);
        // Run the move object
        l_moveObj.run();

        l_responseHandler.detach(message::MCTL,l_callbackFncObj);
        l_responseHandler.detach(message::BRAK,l_callbackFncObj);
        // Close all ports and threads
        l_communicationManager.closeAll();
    }
    catch (exception& e)
    {
        cerr << "Exception: " << e.what() << "\n";
    }
    if(t1.joinable())
        t1.join();
    return 0;
}
