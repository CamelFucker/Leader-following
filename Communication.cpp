//
// Created by nvidia on 20-6-21.
//
#include "Leader_following.h"

using namespace std;

void Communication::CAN0_update(){
    for(;;)
        switch(State)
            {
            case DRIVER_MODE:{
                usleep(SAMPLE_TIME);
                break;
            }
            case READY_STATE:{
                Communication::CAN_send(Con2CAN_steer(0,32767,100),
                                        CONTROL_STEER_MSG);
                Communication::CAN_send(Con2CAN_acc(1,130,0),
                                        CONTROL_ACC_MSG);
                usleep(SAMPLE_TIME);
                //cout << "[READY STATE]CAN0 data is updating..." << endl;
                break;
            }
            case RUN_STATE:{
                mutex mut;
                mut.lock();
                Communication::CAN_send(Con2CAN_steer(0,Control_steer_angle,Control_steer_velocity),
                                        CONTROL_STEER_MSG);
                Communication::CAN_send(Con2CAN_acc(Control_mode,Control_acceleration,Control_pressure),
                                        CONTROL_ACC_MSG);
                mut.unlock();
                usleep(SAMPLE_TIME);
                //cout << "[RUN STATE]CAN0 data is updating..." << endl;
                break;
            }
            case FINISH_STATE:{
                Communication::CAN_send(Con2CAN_steer(0,32767,100),
                                        CONTROL_STEER_MSG);
                Communication::CAN_send(Con2CAN_acc(1,130,0),
                                        CONTROL_ACC_MSG);
                usleep(SAMPLE_TIME);
                //cout << "[RUN STATE]CAN0 data is updating..." << endl;
                break;
                
            }
            default:{
                Communication::CAN_send(Con2CAN_steer(0,Control_steer_angle,Control_steer_velocity),
                                        CONTROL_STEER_MSG);
                Communication::CAN_send(Con2CAN_acc(1,130,0),
                                        CONTROL_ACC_MSG);
                usleep(SAMPLE_TIME);
                //Test mode
                //cout << "[DRIVER MODE]CAN0 data is not updating..." << endl;
            }
            

        }


            //cout << "CAN0 data is updating..." << endl;

}
//Send all messages

void Communication::CAN_receive(int id,int msg_length,bool EFF,int CAN_channel){
    for(;;){
        if(id == VEHICLE_ACC_ID){
            CAN2Val_acc(CAN_get_msg(id,EFF,CAN_channel),msg_length);
            CAN2Val_speed(CAN_get_msg(id,EFF,CAN_channel),msg_length);
            usleep(SAMPLE_TIME);
        }
        //else if(id == VEHICLE_SPEED_ID){
        //    CAN2Val_speed(CAN_get_msg(id,EFF,CAN_channel),msg_length);
        //    usleep(SAMPLE_TIME);
        //}
        else if(id == UWB_POSITION_ID){
            CAN2Val_UWB_position(CAN_get_msg(id,EFF,CAN_channel),msg_length);
            usleep(SAMPLE_TIME);
        }
        else if(id == UWB_LEADERSTATE_ID){
            CAN2Val_UWB_leaderstate(CAN_get_msg(id,EFF,CAN_channel),msg_length);
            //usleep(SAMPLE_TIME/16);
        }
    }
}
//Receive a certain ID CANmsg

void Communication::CAN_send(int *message_ptr,int id,int msg_length,bool EFF, int CAN_channel){
    //cout << "Sending..." << endl;

    /***********************Sockek_CAN config*****************************/
    //CAN send configuration
    int socket_word,nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame[2] = {{0}};
    socket_word = socket(PF_CAN,SOCK_RAW,CAN_RAW);

    if(CAN_channel == 0)
        strcpy(ifr.ifr_name,"can0");
    else if(CAN_channel == 1)
        strcpy(ifr.ifr_name,"can1");

    ioctl(socket_word,SIOCGIFINDEX,&ifr);
    addr.can_family =  AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(socket_word,(struct sockaddr *)&addr,sizeof(addr));
    setsockopt(socket_word,SOL_CAN_RAW,CAN_RAW_FILTER,NULL,0);
    /***********************************************************************/

    //CAN Information
    if(EFF)
        frame[0].can_id = CAN_EFF_FLAG | id;
    else
        frame[0].can_id = id;//CAN ID NOTICE:the input should map to HEX
    frame[0].can_dlc = msg_length;//Message Length

    //CAN message
    for(int i=0;i<msg_length;i++)
        frame[0].data[i] = *(message_ptr+i);


    //Send CAN message
    nbytes = write(socket_word,&frame[0],sizeof(frame[0]));
    // Print CAN message or error
    if(nbytes != sizeof(frame[0])){
        if(CAN_SEND_CHECK|Show_switch)
            cout << "CAN Send Error! Check bitrate. " << endl;
    }
    else{
        if(CAN_SEND_CHECK|Show_switch){
            cout << "Sending: ";
            if(id == CONTROL_STEER_ID)
                cout << "(control steer)";
            else if(id == CONTROL_ACC_ID)
                cout << "(control acc)";
            cout << " CAN ID 0x" << hex << id << " : ";
            for(int i=0; i< msg_length; i++) {
                //CANmsg_acc[i] = *(CANmsg_acc_ptr + i);
                cout << hex << message_ptr[i] << " ";
            }
            cout << endl;
        }
    };
    close(socket_word);
}
//Send a certain ID CANmsg

int * Communication::CAN_get_msg(int id, bool EFF, int CAN_channel){
    //cout << "Receiving ID " << id << " ..." << endl;

    /***********************Sockek_CAN config*****************************/
    int socket_word, nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    struct can_filter rfliter[1];
    mutex mut;
    mut.lock();
    socket_word = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    //Choose CAN channel
    if(CAN_channel == 0)
        strcpy(ifr.ifr_name, "can0" );
    else if(CAN_channel == 1)
        strcpy(ifr.ifr_name, "can1");

    ioctl(socket_word, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    struct can_filter rfilter[1];
    bind(socket_word, (struct sockaddr *)&addr, sizeof(addr));
    //定义接收规则，只接收表示符等于 id 的报文
    rfilter[0].can_id = id;
    if(EFF)
        rfilter[0].can_mask = CAN_EFF_MASK;
    else
        rfilter[0].can_mask = CAN_SFF_MASK;
    //设置过滤规则
    setsockopt(socket_word, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
    nbytes = read(socket_word, &frame, sizeof(frame));
    if(nbytes > 0)
    {
        //cout << "Receiving..." << endl;
    }
    /**********************************************************************/
    //cout << "**********************" << endl;
    // Convert CANmsg to int ptr
    if(CAN_RECEIVE_CHECK|Show_switch){
        cout << "Receiving: ";
        if(id == VEHICLE_ACC_ID)
            cout << "(vehicle acc)";
        //else if(id == VEHICLE_SPEED_ID)
        //    cout << "(vehicle speed)";
        else if(id == UWB_POSITION_ID)
            cout << "(UWB position)";
        else if(id == UWB_LEADERSTATE_ID)
            cout << "(UWB leader state)";
        cout << " CAN ID 0x" << hex << id << ": ";
    }

    static int CAN_msg[8] = {0};

    for (int i=0;i<8;i++){
        CAN_msg[i] = (int)frame.data[i];
        if(CAN_RECEIVE_CHECK|Show_switch)
            cout << CAN_msg[i] << " " ;
    }
    if(CAN_RECEIVE_CHECK|Show_switch)
        cout << endl;

    close(socket_word);
    mut.unlock();
    return CAN_msg;

}
//get CAN message

/********************************Convert value to CAN message************************************/
int * Communication::Con2CAN_steer(int steer_enable,int steer_angle,int steer_velocity){
    static int msg_steer[8] = {0};
    msg_steer[0] = steer_enable;
    msg_steer[1] = steer_velocity/4;
    msg_steer[2] = steer_angle%256;
    msg_steer[3] = steer_angle/256;
    return msg_steer;
}
//Convert steer information to CANmsg

int * Communication::Con2CAN_acc(int control_mode,int acc_value, int pressure_value){
    static int msg_acc[8] = {0};
    static int loop_number = 0;
    if(acc_value > 150){
        control_mode = 2;
    }
    else if(acc_value < 150){
        control_mode = 1;
    }
    else control_mode = 0;
    msg_acc[2] = control_mode * 16;
    if(control_mode==0){//mode 0:No Brake
        msg_acc[0] = 0;
        msg_acc[1] = 0;
    }
    else if(control_mode==2){//mode 1:Require deacc
        msg_acc[0] = acc_value%256;
        msg_acc[1] = acc_value/256;
    }
    else if(control_mode==1){//mode 2:Require pressure
        msg_acc[0] = acc_value%256;
        msg_acc[1] = acc_value/256;
    }
    /*else if(control_mode==3){//mode 3:Require acc
        msg_acc[0] = acc_value%256;
        msg_acc[1] = acc_value/256;
    }
     */
    msg_acc[7] = loop_number%16;
    loop_number ++;
    return msg_acc;
}
//Convert acc information to CANmsg
/************************************************************************************************/

/********************************Convert CAN message to value************************************/
void Communication::CAN2Val_acc(int *CANmsg_acc,int msg_length){
    Follower_La_acc = CANmsg_acc[1] + CANmsg_acc[2] * 256;
}
//Convert CANmsg to follower acc value

void Communication::CAN2Val_UWB_position(int*CANmsg_UWB,int msg_length){
    //cout << "UWB_position" << endl;
    UWB_distance = 256 * CANmsg_UWB[3] + CANmsg_UWB[2];
    if (CANmsg_UWB[5] >= 0x80)
    {
        UWB_fangwei = 256 * CANmsg_UWB[5] + CANmsg_UWB[4] - 65536;
    }
    else {
        UWB_fangwei = 256 * CANmsg_UWB[5] + CANmsg_UWB[4];
    }
    if (CANmsg_UWB[7] >= 0x80)
    {
        UWB_zitai = 256 * CANmsg_UWB[7] + CANmsg_UWB[6] - 65536;
    }
    else {
        UWB_zitai = 256 * CANmsg_UWB[7] + CANmsg_UWB[6];
    }
}
//Convrt CANmsg to UWB position value

void Communication::CAN2Val_UWB_leaderstate(int*msg,int msg_length){
    if(msg[0] == 0xA1)
        CAN2Val_acc_pedal(msg,msg_length);
    else if(msg[0] == 0xA2)
        CAN2Val_brake(msg,msg_length);
    else if(msg[0] == 0xA3)
        CAN2Val_steering_wheel(msg,msg_length);
    else if(msg[0] == 0xA4)
        CAN2Val_wheel(msg,msg_length);
    else if(msg[0] == 0xA5)
        CAN2Val_la_yr(msg,msg_length);
    else if(msg[0] == 0xA6)
        CAN2Val_gear_position(msg,msg_length);
    else if(msg[0] == 0xA7)
        CAN2Val_pedal_angle(msg,msg_length);
    //CANmsg_UWB_state[8]

}
//Convert CANmsg to leader state value

void Communication::CAN2Val_speed(int*CANmsg_speed,int msg_length){
    Follower_Speed = CANmsg_speed[3] + CANmsg_speed[4] * 256;
}

void Communication::CAN2Val_acc_pedal(int*CANmsg_acc_pedal,int msg_length){
    mutex mut;
    lock_guard<mutex> lock(mut);
    Leader_ACC_pedal_position = CANmsg_acc_pedal[1];
    Leader_Remote_position = CANmsg_acc_pedal[4];
    //cout << "Leader_ACC_pedal_position = " << Leader_ACC_pedal_position << endl;
}

void Communication::CAN2Val_brake(int*CANmsg_brake,int msg_length){
    mutex mut;
    lock_guard<mutex> lock(mut);
    Leader_Brake_pedal_position = CANmsg_brake[1];
    Leader_Actual_acc = (CANmsg_brake[3] * 256 + CANmsg_brake[2]);
    /*
    if(CANmsg_brake[3] >= 0x80)
        Leader_Actual_acc = (CANmsg_brake[3] * 256 + CANmsg_brake[2]) - 0xFFFF - 1;
    else
        Leader_Actual_acc = (CANmsg_brake[3] * 256 + CANmsg_brake[2]);
    */
    Leader_Speed = (CANmsg_brake[5] * 256 + CANmsg_brake[4]);
    Leader_Pressure = CANmsg_brake[6];
    //cout << "Leader_Speed = " << Leader_Speed << endl;
}

void Communication::CAN2Val_steering_wheel(int*CANmsg_steering_wheel,int msg_length){
    mutex mut;
    lock_guard<mutex> lock(mut);
    if(CANmsg_steering_wheel[2] >= 0x80)
        Leader_Steering_wheel_angle = (CANmsg_steering_wheel[2] * 256 + CANmsg_steering_wheel[1]) - 0xFFFF - 1;
    else
        Leader_Steering_wheel_angle = (CANmsg_steering_wheel[2] * 256 + CANmsg_steering_wheel[1]);
    Leader_Steering_wheel_speed = CANmsg_steering_wheel[3];
    Leader_Steering_wheel_state = CANmsg_steering_wheel[4];
    Leader_Count = CANmsg_steering_wheel[5];
    Leader_Check = CANmsg_steering_wheel[7];
}

void Communication::CAN2Val_wheel(int*CANmsg_wheel_speed,int msg_length){
    mutex mut;
    lock_guard<mutex> lock(mut);
    Leader_Wheel_speed = CANmsg_wheel_speed[3] * (256 * 256) + CANmsg_wheel_speed[2] * 256 + CANmsg_wheel_speed[1];
}

void Communication::CAN2Val_la_yr(int*CANmsg_la_yr,int msg_length){
    mutex mut;
    lock_guard<mutex> lock(mut);
    Leader_La_acc = CANmsg_la_yr[1] + CANmsg_la_yr[2] * 256;
    Leader_Yr_speed = CANmsg_la_yr[1] + CANmsg_la_yr[2] * 256;
}

void Communication::CAN2Val_gear_position(int*CANmsg_gear_position,int msg_length){
    mutex mut;
    lock_guard<mutex> lock(mut);
    Leader_Target_gear = CANmsg_gear_position[1];
    Leader_Current_gear = CANmsg_gear_position[5];
}

void Communication::CAN2Val_pedal_angle(int*CANmsg_pedal_angle,int msg_length){
    mutex mut;
    lock_guard<mutex> lock(mut);
    Leader_Acc_pedal = CANmsg_pedal_angle[1] + CANmsg_pedal_angle[2] * 256;
    Leader_Brake_pedal = CANmsg_pedal_angle[6] + CANmsg_pedal_angle[7] * 256;
}

//Convert CANmsg to follower speed
/************************************************************************************************/