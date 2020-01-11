#include <iostream>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>

#include <webcam.h>
#include <raylib.h>
#include <grab.h>

using namespace std;

#include <SFML/Network.h>
#include <unistd.h>

#ifdef DEBUG
    sfUdpSocket* udpSock;
    sfTcpListener* tcpList;
    sfTcpSocket* tcpSock;
    sfPacket* packet;
    sfIpAddress localAddr;
    void debug_setup()
    {
        printf("Run in debug mode.\n");
        udpSock = sfUdpSocket_create();
        packet = sfPacket_create();
        tcpSock = sfTcpSocket_create();
        tcpList = sfTcpListener_create();
        localAddr = sfIpAddress_getLocalAddress();
        sfUdpSocket_bind(udpSock, 6969, sfIpAddress_Broadcast);
        sfTcpListener_listen(tcpList, 0, localAddr);
        sfPacket_writeUint32(packet, sfTcpListener_getLocalPort(tcpList));
        sfPacket_writeString(packet, localAddr.address);
        sfUdpSocket_sendPacket(udpSock, packet, sfIpAddress_Broadcast, 6969);
        printf("Broadcast sendet.\n");
        sfTcpListener_accept(tcpList, &tcpSock);
        printf("Accepted from server: %s, port: %hu\n", sfTcpSocket_getRemoteAddress(tcpSock).address, sfTcpSocket_getRemotePort(tcpSock));
    }
    bool debug_send_frame(const RGBImage* frame)
    {
        sfPacket_clear(packet);
        sfPacket_writeUint32(packet, frame->width);
        sfPacket_writeUint32(packet, frame->height);
        sfPacket_writeUint32(packet, frame->size);
        sfPacket_append(packet, frame->data, frame->size);
        if(sfTcpSocket_sendPacket(tcpSock, packet) != sfSocketDone) 
        {
            printf("Connection closed. Program will be closed.\n");
            return false;
        }
        return true;
    }
    void debug_destroy()
    {
        sfUdpSocket_unbind(udpSock);
        sfUdpSocket_destroy(udpSock);
        sfTcpListener_destroy(tcpList);
        sfTcpSocket_destroy(tcpSock);
        sfPacket_destroy(packet);
    }
#endif


int main(int argc, char** argv)
{
    const RGBImage *frame_ptr = nullptr;
    Webcam webcam("/dev/video0", XRES, YRES);

#ifdef DEBUG
    debug_setup();
#endif
    while(1)
    {
        frame_ptr = &webcam.frame();
#ifdef DEBUG
        if(!debug_send_frame(frame_ptr)) break;
#endif
    }
#ifdef DEBUG
    debug_destroy();
#endif

    /*  
    const int screenWidth = XRES;
    const int screenHeight = YRES;
    InitWindow(screenWidth, screenHeight, "Camera monitor");
    SetTargetFPS(60);  
    while (!WindowShouldClose())
    {
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
    */
    return 0;
}
