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
    sfPacket* packet;
    sfIpAddress serverAddr;
    void debug_setup()
    {
        sfIpAddress localAddr;
        sfIpAddress broadcastAddr = (sfIpAddress){"255.255.255.255"};
        printf("Run in debug mode.\n");
        udpSock = sfUdpSocket_create();
        packet = sfPacket_create();
        localAddr = sfIpAddress_getLocalAddress();
        sfUdpSocket_bind(udpSock, 6969, sfIpAddress_Broadcast);
        sfPacket_writeString(packet, localAddr.address);
        sfUdpSocket_sendPacket(udpSock, packet, sfIpAddress_Broadcast, 6969);
        printf("Broadcast sendet.\n");
        sfUdpSocket_unbind(udpSock);
        sfUdpSocket_bind(udpSock, 6969, sfIpAddress_Any);
        sfPacket_clear(packet);
        sfUdpSocket_receivePacket(udpSock, packet, &broadcastAddr, 0);
        sfPacket_readString(packet, serverAddr.address);
        printf("Broadcast accepted from server: %s\n", serverAddr.address);
    }
    void debug_send_frame(const RGBImage* frame)
    {
        size_t ssize = (frame->size / 4) - 1;
        sfPacket_clear(packet);
        sfPacket_append(packet, frame->data, ssize);
        sfUdpSocket_sendPacket(udpSock, packet, serverAddr, 6969);
        sfPacket_clear(packet);

        sfPacket_append(packet, &frame->data[ssize], ssize);
        sfUdpSocket_sendPacket(udpSock, packet, serverAddr, 6969);
        sfPacket_clear(packet);

        sfPacket_append(packet, &frame->data[ssize * 2], ssize);
        sfUdpSocket_sendPacket(udpSock, packet, serverAddr, 6969);
        sfPacket_clear(packet);

        sfPacket_append(packet, &frame->data[ssize * 3], ssize);
        sfUdpSocket_sendPacket(udpSock, packet, serverAddr, 6969);

        //printf("Packet sendet.\n");
    }
    void debug_destroy()
    {
        sfUdpSocket_unbind(udpSock);
        sfUdpSocket_destroy(udpSock);
        //sfTcpListener_destroy(tcpList);
        //sfTcpSocket_destroy(tcpSock);
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
        debug_send_frame(frame_ptr);
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
