//
// Created by nvidia on 20-6-21.
//
#include "Leader_following.h"
using namespace std;

void Control::Control_update(){
    for(;;){

        cout << "UWB_distance = " << dec << UWB_distance << endl;
        cout << "UWB_fangwei = " << dec << UWB_fangwei  << endl;
        cout << "UWB_zitai = " << dec << UWB_zitai << endl;


        Control_steer_enable = 1;//TODO:when to start enable

        // Record time
        /*time_t control_time;
        control_time = time(NULL);*/

        // convert from int to float
        float leader_wheel_speed = (float)(Leader_velocity);
        float follower_wheel_speed = (float)(Follower_velocity)/256.0; // m/s
        float leader_acceleration = (float)(Leader_acceleration);
        float follower_acceleration = (float)(Follower_acceleration)*0.01 - 15.0; //m/s^2
        float distance = (float)(UWB_distance)/100.0; // m
        float fangwei_angle = (float)(UWB_fangwei)/1.0; //degree
        float zitai_angle = (float)(UWB_zitai)/1.0; // degree

        cout << "distance = " << distance << endl;
        cout << "fangwei_angle = " << fangwei_angle << endl;
        cout << "zitai_angle = " << zitai_angle << endl;

        // Caculate middle variables
        float leader_velocity;
        leader_velocity = leader_wheel_speed;   //m/s
        float follower_velocity;
        follower_velocity = follower_wheel_speed; // m/s
        float long_distance;
        long_distance = distance * cos(fangwei_angle/180*M_PI); //m
        float lat_distance;
        lat_distance = distance * sin(fangwei_angle/180*M_PI); // m
        float control_steer;
        control_steer = Control::Caculate_steer(lat_distance,long_distance);
        float control_acc;
        control_acc = Control::Caculate_acc(leader_velocity,follower_velocity,leader_acceleration,long_distance);
        float brake_pressure;
        brake_pressure = 0.5;

        // convert from float to int
        Control_steer_angle = (int)((control_steer + 3276.7)/0.1); // Signal value = (physical value - offset)/precision value
        Control_acceleration = (int)((control_acc + 15)/0.1); //[-15,15] m/s^2
        Control_pressure = (int)((brake_pressure)/0.01); // [0,1]MPa

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
    //cout << "K_v = " << to_string(k_v) << " and K_d = " << to_string(k_d) << endl;
    if(control_acc > 0.2)
        control_acc = 0.2; // acc limit
    //cout << "Control_acceleration = "<< to_string(control_acc) << endl;
    return control_acc;
}

//TODO LIST

//TODO:STATEMECHINE
//TODO:RECORD DATA
