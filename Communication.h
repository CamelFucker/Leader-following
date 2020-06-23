//
// Created by nvidia on 20-6-23.
//

#ifndef LEADERFOLLOWING_COMMUNICATION_H
#define LEADERFOLLOWING_COMMUNICATION_H

#endif //LEADERFOLLOWING_COMMUNICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <iostream>
#include <string.h>

class Communication{
public:
    static void CAN1_update();
    static void CAN2_update();
    void CAN_send(int message);
    //void CAN_recive();
private:
    int socket_word;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame[2];
};

