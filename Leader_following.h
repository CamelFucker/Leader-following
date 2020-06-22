//
// Created by nvidia on 20-6-21.
//

#ifndef LEADERFOLLOWING_LEADER_FOLLOWING_H
#define LEADERFOLLOWING_LEADER_FOLLOWING_H

#endif //LEADERFOLLOWING_LEADER_FOLLOWING_H

#define SAMPLE_TIME 2000000  //us

#include <thread>
#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>
#include <curses.h>
#include <stdio.h>
#include <ctime>

//Signals
extern int UWB_distance;
extern int UWB_fangwei;
extern int UWB_zitai;
extern int Leader_velocity;
extern int Leader_acceleration;
extern int Follower_velocity;
extern int Follower_acceleration;
extern int Control_steer_angle;
extern int Control_acceleration;


class Communication{
public:
    static void CAN1_update();
    static void CAN2_update();
};

class Control{
private:
    // args
    static constexpr float k_v = 0.2;
    static constexpr float k_d = 0.2;
    // float signal variable
    static float leader_velocity;
    static float follower_velocity;
    static float leader_acceleration;
    static float fangwei_angle;
    static float zitai_angle;
    static float long_distance;
    static float lat_distance;
    static float frontwheel_steer_angle;
    static float steer_wheel_angle;
    static float control_steer;
    static float control_acc;
    // vehicle parads
    static const int L = 5;
    static const int r = 1;

public:
    //Control();
    //Control(float k_v, float k_d);
    static void Control_update();
    static float Caculate_steer(float lat_distance, float long_distance);
    static float Caculate_acc(float v1, float v2, float a1, float long_distance);
};


