#include <serialPortHandler.hpp>
#include <SerialComManager.hpp>
#include <MessageHandler.hpp>
#include <Message.hpp>
#include <boost/function.hpp>
#include <example1.hpp>

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


    const int screenWidth = XRES;
    const int screenHeight = YRES;
    InitWindow(screenWidth, screenHeight, "Camera monitor");
    SetTargetFPS(30);
    Color * pixel_buffer = new Color[XRES*YRES];
    while (!WindowShouldClose())
    {
        frame_ptr = &webcam.frame();
        if(IsKeyDown(KEY_S))
        {
            makeScreenshot(frame_ptr, "frame.ppm");
        }
        //Frame frame(*frame_ptr);
        frame *f = new frame(*frame_ptr);
        Color * frame_color_buffer = f->toColorBuffer();
        matrix * grey = f->toGreyScaleBuffer();



        grey->setColorBuffer(pixel_buffer);

        matrix * temp = grey->gaussian_filter(1.4);

        delete grey;

        grey = temp->sobol_filter();

        delete temp;

        grey->threshold(70);

        for(int i =0 ; i<grey->h; i++)
        {
            for(int j=0; j<grey->w; j++)
            {
                if(grey->body[i][j]>70)
                {
                    pixel_buffer[i*grey->w+j] = white_filter(frame_color_buffer[i*grey->w+j]);
                }
                else
                    pixel_buffer[i*grey->w+j] = frame_color_buffer[i*grey->w+j];

            }

        }

        Image image = LoadImageEx(pixel_buffer,grey->w,grey->h);
        Texture2D texture = LoadTextureFromImage(image);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture,screenWidth/2-grey->w/2,screenHeight/2-grey->h/2,WHITE);
        directionControl(grey,20,true);
        DrawFPS(10,10);
        EndDrawing();
        UnloadImage(image);
        UnloadTexture(texture);
        delete grey;
        delete f;
    }

    CloseWindow();
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
