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
        //CAN1 Receive
        UWB_distance = 20;
        UWB_fangwei = 0;
        UWB_zitai = 0;
        Leader_velocity = 0;
        Leader_acceleration = 0;

        /**********Send steer message***********/
        //CAN message convertion
        static int * CANmsg_steer_ptr;
        static int CANmsg_steer[CONTROL_STEER_DLC];
        // get ptr
        CANmsg_steer_ptr = Con2CAN_steer(Control_steer_enable,Control_steer_angle,Control_steer_velocity);
        cout << "CANmsg_steer: ";
        for(int i=0; i<CONTROL_STEER_DLC; i++) {
            CANmsg_steer[i] = *(CANmsg_steer_ptr + i);
            cout << hex << CANmsg_steer[i] << " ";
        }
        cout << endl;
        Communication::CAN_send(CANmsg_steer,CONTROL_STEER_DLC,CONTROL_STEER_ID,0);
        /**************************************/

        /**********Send acc message***********/
        //CAN message convertion
        static int * CANmsg_acc_ptr;
        static int CANmsg_acc[CONTROL_ACC_DLC];
        // get ptr
        CANmsg_acc_ptr = Con2CAN_acc(Control_mode,Control_acceleration,Control_pressure);
        cout << "CANmsg_acc: ";
        for(int i=0; i<CONTROL_ACC_DLC; i++) {
            CANmsg_acc[i] = *(CANmsg_acc_ptr + i);
            cout << hex << CANmsg_acc[i] << " ";
        }
        cout << endl;
        Communication::CAN_send(CANmsg_acc,CONTROL_ACC_DLC,CONTROL_ACC_ID,0);
        /**************************************/

        usleep(SAMPLE_TIME);
        cout << "CAN1 data is updating..." << endl;
        //cout << "CAN1 data is updating..." << "  Time is " << can1_time << endl;  //show time
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

void Communication::CAN_send(int *message_ptr,int msg_length,int id,bool EFF){
    cout << "Sending..." << endl;

    //CAN send configuration
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

    //CAN Information
    if(EFF)
        frame[0].can_id = CAN_EFF_FLAG | id;
    else
        frame[0].can_id = id;//CAN ID NOTICE:the input should map to HEX
    frame[0].can_dlc = msg_length;//Message Length
    //CAN message
    for(int i=0;i<msg_length;i++)
        frame[0].data[i] = *(message_ptr+i);

    //TODO:check which is high or low
    //Send CAN message
    nbytes = write(socket_word,&frame[0],sizeof(frame[0]));
    // Print CAN message or error
    if(nbytes != sizeof(frame[0])){
        cout << "CAN Send Error" << endl;
    }
    else{
        /*
        for (int byte=0; byte<8; byte++)
            cout << hex << to_string(frame[0].data[byte]) << " ";
        cout << endl;
         */
    };
    close(socket_word);
}

//Communication::Con2CAN_steer(int steer_angle) //TODO:No velocity input model

int * Communication::Con2CAN_steer(int steer_enable,int steer_angle,int steer_velocity){
    static int msg_steer[CONTROL_STEER_DLC] = {0};
    msg_steer[0] = steer_enable;
    msg_steer[1] = steer_velocity/4;
    msg_steer[2] = steer_angle/256;
    msg_steer[3] = steer_angle%256;
    return msg_steer;
}

int * Communication::Con2CAN_acc(int control_mode,int acc_value, int pressure_value){
    static int msg_acc[CONTROL_ACC_DLC] = {0};
    static int loop_number = 0;
    msg_acc[2] = control_mode;
    if(control_mode==0){//mode 0:No Brake
        msg_acc[0] = 0;
        msg_acc[1] = 0;
    }
    else if(control_mode==1){//mode 1:Require deacc
        msg_acc[0] = acc_value/256;
        msg_acc[1] = acc_value%256;
    }
    else if(control_mode==2){//mode 2:Require pressure
        msg_acc[0] = pressure_value/256;
        msg_acc[1] = pressure_value%256;
    }
    else if(control_mode==3){//mode 3:Require acc
        msg_acc[0] = acc_value/256;
        msg_acc[1] = acc_value%256;
    }
    msg_acc[7] = loop_number%16;
    loop_number ++;
    return msg_acc;
}

