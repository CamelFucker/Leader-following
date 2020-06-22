//
// Created by nvidia on 20-6-21.
//
#include "Leader_following.h"
using namespace std;

void Communication::CAN1_update(){
    for(;;){
        /*
        time_t can1_time;
        can1_time = time(NULL);
         */
        UWB_distance = 20;
        UWB_fangwei = 0;
        UWB_zitai = 0;
        Leader_velocity = 0;
        Leader_acceleration = 0;
        usleep(SAMPLE_TIME);
        cout << "CAN1 data is updating..." << endl;
        //cout << "CAN1 data is updating..." << "  Time is " << can1_time << endl;
    }
}

void Communication::CAN2_update(){
    for(;;){
        /*
        time_t can2_time;
        can2_time = time(NULL);
         */
        Follower_velocity = 0;
        Follower_acceleration = 0;
        usleep(SAMPLE_TIME);
        cout << "CAN2 data is updating..." << endl;
        //cout << "CAN2 data is updating..." << "  Time is " << can2_time << endl;
    }
}
