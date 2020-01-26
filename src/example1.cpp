#include <example1.hpp>
#include <stdio.h>
#include <stdlib.h>

CMoveExample::CMoveExample(SerialComManager& f_comManager)
                            :m_comManager(f_comManager)
{}


void CMoveExample::run(){
    /*
    std::array<float,9> l_pwmMotor_values={0.07,0.08,0.09,0.1,0.11,0.1,0.09,0.08,0.07};
    float l_steeringAngle=0.0;

    for (unsigned int i=0;i<l_pwmMotor_values.size();++i){
       float l_pwmMotor= l_pwmMotor_values[i];
       m_comManager.sendMove(l_pwmMotor,l_steeringAngle);
       usleep(0.4e6);
    }
    m_comManager.sendBrake(l_steeringAngle);
    usleep(0.5e6);
    */

    //system ("/bin/stty raw");

    char com;
    float speed = 0.00;
    float angle = 0.00;
    bool exit = false;
    m_comManager.sendBrake(angle);
    while(exit == false) {
        //std::cin >> com;
        system ("/bin/stty raw");
        com = getchar();
        system ("/bin/stty cooked");
        switch (com) {
        case 'w':
            speed += 0.05;
            //angle = 0.00;
            break;
        case 's':
            speed -= 0.05;
            //angle = 0.00;
            break;
        case 'a':
            angle -= 3.00;
            break;
        case 'd':
            angle += 3.00;
            break;
        case ' ':
            m_comManager.sendBrake(0.00);
            //speed = 0.23;
            usleep(0.5e6);
            continue;

        default:
            exit = true;
            break;
        }
        if(exit == false)m_comManager.sendMove(speed, angle);

        usleep(0.4e6);
   }
    m_comManager.sendBrake(0.0);
    usleep(0.5e6);
    //system ("/bin/stty cooked");
}
