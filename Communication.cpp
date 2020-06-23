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

void Communication::CAN_send(int message){
    cout << "Sending..." << endl;
    int socket_word,nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame[2] = {{0}};
    socket_word = socket(PF_CAN,SOCK_RAW,CAN_RAW);
    strcpy(ifr.ifr_name,"can0");

    ioctl(socket_word,SIOCGIFINDEX,&ifr);
    addr.can_family =  AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(socket_word,(struct sockaddr *)&addr,sizeof(addr));
    setsockopt(socket_word,SOL_CAN_RAW,CAN_RAW_FILTER,NULL,0);
    //TODO:Convert int message to CAN message


    frame[0].can_id = 0x11;//CAN ID
    frame[0].can_dlc = 8;//Message Length
    //CAN message
    frame[0].data[0] = 0x01;//Low
    frame[0].data[1] = 0x02;
    frame[0].data[2] = 0x03;
    frame[0].data[3] = 0x04;
    frame[0].data[4] = 0x05;
    frame[0].data[5] = 0x06;
    frame[0].data[6] = 0x07;
    frame[0].data[7] = 0x08;//High

    nbytes = write(socket_word,&frame[0],sizeof(frame[0]));

    if(nbytes != sizeof(frame[0])){
        cout << "CAN Send Error" << endl;
    }
    else{
        for (int byte=7; byte>-1; byte--)
            cout << hex << to_string(frame[0].data[byte]) << " ";
        cout << endl;
    };
    close(socket_word);
}

//void Communication::CAN_recieve(){

//}