//
// Created by nvidia on 20-6-21.
//
#include "Leader_following.h"
using namespace std;

void Control::Control_update(){
    for(;;){

        //cout << "UWB_distance = " << dec << UWB_distance << endl;
        //cout << "UWB_fangwei = " << dec << UWB_fangwei  << endl;
        //cout << "UWB_zitai = " << dec << UWB_zitai << endl;

        Control_steer_enable = 1;

        // Record time
        /*time_t control_time;
        control_time = time(NULL);*/

        // convert from int to float
        float leader_speed = (float)(Leader_Speed) * 0.1;// m/s
        float follower_speed = (float)(Follower_Speed)/256.0/3.6; // km/h

        float follower_la_acc = (float)(Follower_La_acc)*0.01 - 15.0; //m/s^2
        float distance = (float)(UWB_distance)/100.0; // m
        float fangwei_angle = (float)(UWB_fangwei)/1.0; //degree
        float zitai_angle = (float)(UWB_zitai)/1.0; // degree

        float leader_acc_pedal_position = (float)(Leader_ACC_pedal_position) * 0.4;
        float leader_remote_position = (float)(Leader_Remote_position) * 0.4;
        float leader_brake_pedal_position = (float)(Leader_Brake_pedal_position) * 0.01;
        float leader_actual_acc = (float)(Leader_Actual_acc) * 0.1; //m/s^2
        float leader_pressure = (float)(Leader_Pressure) * 0.01;
        float leader_steering_wheel_angle = (float)(Leader_Steering_wheel_angle) * 0.1;
        float leader_steering_wheel_speed = (float)(Leader_Steering_wheel_speed) * 4; //deg/s
        //float leader_steering_wheel_state = (float)(Leader_Steering_wheel_state) * ;
        //float leader_count = (float)(Leader_Count) * ;
        //float leader_check = (float)(Leader_Check) * ;
        float leader_wheel_speed = (float)(Leader_Wheel_speed) * 0.01;
        float leader_la_acc = (float)(Leader_La_acc) * 0.01; //m/s^2
        float leader_yr_speed = (float)(Leader_Yr_speed) * 0.01; //rad/s
        //float leader_target_gear = (float)(Leader_Target_gear) * ;
        //float leader_current_gear = (float)(Leader_Current_gear) * ;
        float leader_acc_pedal = (float)(Leader_Acc_pedal) * 0.1;// deg
        float leader_brake_pedal = (float)(Leader_Brake_pedal) * 0.1; //deg


        // Caculate middle variables

        float long_distance;
        long_distance = distance * cos(fangwei_angle/180*M_PI); //m
        float lat_distance;
        lat_distance = distance * sin(fangwei_angle/180*M_PI); // m
        float control_steer;
        control_steer = Control::Caculate_steer(lat_distance,long_distance);// degree
        float control_acc;
        control_acc = Control::Caculate_acc(leader_speed,follower_speed,leader_la_acc,long_distance); //m/s^2
        float control_brake_pressure;
        control_brake_pressure = 0.5;


        if(STATE_VALUE_PRINT){
            cout << "******** STATE VALUE ********" << endl;
            cout << "leader_speed = " << leader_speed << endl;
            cout << "leader_acceleration = " << leader_la_acc << endl;
            cout << "follower_velocity = " << follower_speed << endl;
            cout << "follower_acceleration = " << follower_la_acc << endl;
            cout << "long_distance = " << long_distance << endl;
            cout << "lat_distance = " << lat_distance << endl;
        }

        if(CONTROL_VALUE_PRINT){
            cout << "******* CONTROL VALUE *******" << endl;
            cout << "control_steer = " << control_steer << endl;
            cout << "control_acc = " << control_acc << endl;
        }


        // convert from float to int
        Control_steer_angle = (int)((control_steer + 3276.7)/0.1); // Signal value = (physical value - offset)/precision value
        Control_acceleration = (int)((control_acc + 15)/0.1); //[-15,15] m/s^2
        Control_pressure = (int)((control_brake_pressure)/0.01); // [0,1]MPa

        //cout << "control_steer = " << control_steer << endl;
        //cout << "Control is updating " << endl;
        //cout << "Control is updating" << "  Time is " << control_time << endl;
        usleep(SAMPLE_TIME);
    }
}

float Control::Caculate_steer(float lat_distance, float long_distance){
    //cout << "lat_distance = "<< to_string(lat_distance) << endl;
    float frontwheel_steer_angle = atan(2*L*lat_distance/(long_distance*long_distance))*180/M_PI;
    if(frontwheel_steer_angle > 15)
        frontwheel_steer_angle = 15;
    else if(frontwheel_steer_angle < -15)
        frontwheel_steer_angle = -15; // steer angle limit
    //cout << "Frontwheel_steer_angle = "<< to_string(frontwheel_steer_angle) << endl;
    float steer_wheel_angle = 24.1066 * frontwheel_steer_angle + 4.8505;// Caculate from steer map
    return steer_wheel_angle;
}

float Control::Caculate_acc(float v1, float v2, float a1, float long_distance){
    float control_acc;
    control_acc = a1 + k_v * (v1 - v2) + k_d * (long_distance - EXPECTED_DISTANCE);

    // Keep velocity


    //cout << "K_v = " << to_string(k_v) << " and K_d = " << to_string(k_d) << endl;
    if(control_acc > 0.2)
        control_acc = 0.2; // acc limit
    //cout << "Control_acceleration = "<< to_string(control_acc) << endl;
    return control_acc;
}

//TODO LIST

//TODO:STATEMECHINE
//TODO:RECORD DATA
