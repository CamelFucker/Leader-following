//
// Created by nvidia on 20-6-21.
//

#ifndef LEADERFOLLOWING_LEADER_FOLLOWING_H
#define LEADERFOLLOWING_LEADER_FOLLOWING_H




#endif //LEADERFOLLOWING_LEADER_FOLLOWING_H

#define SAMPLE_TIME 20000  //us
#define EXPECTED_DISTANCE 10

#define CAN_SEND_CHECK 1
#define CAN_RECEIVE_CHECK 0
#define STATE_VALUE_PRINT 1
#define CONTROL_VALUE_PRINT 1


#include <thread>
#include <cmath>
#include <unistd.h>
//#include <curses.h>
#include <stdio.h>
#include <ctime>
#include <typeinfo>

#include "Communication.h"
#include "Statemechine.h"


//Declare signals from follower
extern int UWB_distance;
extern int UWB_fangwei;
extern int UWB_zitai;

//extern int Leader_acceleration;
extern int Follower_Speed;
extern int Follower_La_acc;

//Declare signals from leader
extern int Leader_ACC_pedal_position;
extern int Leader_Remote_position;
extern int Leader_Brake_pedal_position;
extern int Leader_Actual_acc;
extern int Leader_Speed;
extern int Leader_Pressure;
extern int Leader_Steering_wheel_angle;
extern int Leader_Steering_wheel_speed;
extern int Leader_Steering_wheel_state;
extern int Leader_Count;
extern int Leader_Check;
extern int Leader_Wheel_speed;
extern int Leader_La_acc;
extern int Leader_Yr_speed;
extern int Leader_Target_gear;
extern int Leader_Current_gear;
extern int Leader_Acc_pedal;
extern int Leader_Brake_pedal;

//Declare control signals output
extern int Control_steer_enable;
extern int Control_steer_angle;
extern int Control_steer_velocity;

extern int Control_mode; // 0:No brake;1:acc respond;2:pressure respond; 3: drive respond;
extern int Control_acceleration;
extern int Control_pressure;

//Declare Statemechine
extern int State; // Current State
extern bool Signal_emergency;
extern bool Command_ready;
extern bool Command_run;
extern bool Command_finish;
extern bool Command_end;


class Control{
private:
    // args
    static constexpr float k_v = 0.2;
    static constexpr float k_d = 0.2;
    // float signal variable
    static float leader_speed;
    static float follower_speed;
    static float leader_la_acc;
    static float follower_la_acc;
    static float fangwei_angle;
    static float zitai_angle;

    static float leader_acc_pedal_position;
    static float leader_remote_position;
    static float leader_brake_pedal_position;
    static float leader_actual_acc;
    static float leader_pressure;
    static float leader_steering_wheel_angle;
    static float leader_Steering_wheel_speed;
    //static float leader_steering_wheel_state;
    //static float leader_count;
    //static float leader_check;
    static float leader_wheel_speed;

    static float leader_yr_speed;
    //static float leader_target_gear;
    //static float leader_current_gear;
    static float leader_acc_pedal;
    static float leader_brake_pedal;

    static float long_distance;
    static float lat_distance;
    static float frontwheel_steer_angle;
    static float steer_wheel_angle;


    static float control_steer;
    static float control_acc;
    static float control_brake_pressure;
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


