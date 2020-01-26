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
        f->setColorBuffer(pixel_buffer);
        Image image = LoadImageEx(pixel_buffer,f->getWidth(),f->getHeight());
        Texture2D texture = LoadTextureFromImage(image);
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(texture,screenWidth/2-f->getWidth()/2,screenHeight/2-f->getHeight()/2,WHITE);
            DrawFPS(10,10);
        EndDrawing();
        UnloadImage(image);
        UnloadTexture(texture);
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
	if(t1.joinable()) t1.join();
	return 0;
}
