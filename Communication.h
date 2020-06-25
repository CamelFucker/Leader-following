//
// Created by nvidia on 20-6-23.
//

#ifndef LEADERFOLLOWING_COMMUNICATION_H
#define LEADERFOLLOWING_COMMUNICATION_H

#endif //LEADERFOLLOWING_COMMUNICATION_H

#define CAN_EFF_FLAG 0x80000000U //Extended Frame Mark

#define CAN_EFF_MASK 0x1FFFFFFFU //Extended Frame format

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
#include <string>

class Communication{
public:
    static void CAN1_update();
    static void CAN2_update();
    static void CAN_send(int *message_ptr,int msg_length,int id,bool EFF, int CAN_channel);
    static int * CAN_receive(int id,bool EFF);

private:
    //CAN_send func
    static int socket_word;
    static int nbytes;
    static struct sockaddr_can addr;
    static struct ifreq ifr;
    static struct can_frame frame[2];
    //CAN_steer_msg
    static int * Con2CAN_steer(int steer_enable,int steer_angle,int steer_velocity);
    static const int CONTROL_STEER_DLC = 4;
    static const int CONTROL_STEER_ID = 0x23;

    //CAN_acc_msg
    static int * Con2CAN_acc(int control_mode, int acc_value, int pressure_value);
    static const int CONTROL_ACC_DLC = 8;
    static const int CONTROL_ACC_ID = 0x33df;

    //CAN Receive
    static void CAN2Val_acc(int *message_ptr,int msg_length);
    static void CAN2Val_UWB(int *message_ptr,int msg_length);


};

