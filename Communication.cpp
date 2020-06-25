//
// Created by nvidia on 20-6-21.
//
#include "Leader_following.h"

using namespace std;

void Communication::CAN0_update(){
    for(;;){
        Communication::CAN_send(Con2CAN_steer(Control_steer_enable,Control_steer_angle,Control_steer_velocity),
                CONTROL_STEER_MSG);
        Communication::CAN_send(Con2CAN_acc(Control_mode,Control_acceleration,Control_pressure),
                CONTROL_ACC_MSG);
        usleep(SAMPLE_TIME);
        //cout << "CAN1 data is updating..." << endl;
    }
}
//Send all messages

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
    //TODO:check which is high or low

    //Send CAN message
    nbytes = write(socket_word,&frame[0],sizeof(frame[0]));
    // Print CAN message or error
    if(nbytes != sizeof(frame[0])){
        cout << "CAN Send Error! Check bitrate. " << endl;
    }
    else{
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
    };
    close(socket_word);
}
//Send a certain ID CANmsg

void Communication::CAN_receive(int id,int msg_length,bool EFF,int CAN_channel){
    for(;;){
        if(id == VEHICLE_ACC_ID)
            CAN2Val_acc(CAN_get_msg(id,EFF,CAN_channel),msg_length);
        else if(id == VEHICLE_SPEED_ID)
            CAN2Val_speed(CAN_get_msg(id,EFF,CAN_channel),msg_length);
        else if(id == UWB_POSITION_ID)
            CAN2Val_UWB_position(CAN_get_msg(id,EFF,CAN_channel),msg_length);
        else if(id == UWB_LEADERSTATE_ID)
            CAN2Val_UWB_leaderstate(CAN_get_msg(id,EFF,CAN_channel),msg_length);
    }

}
//Receive a certain ID CANmsg

int * Communication::CAN_get_msg(int id, bool EFF, int CAN_channel){
    //cout << "Receiving ID " << id << " ..." << endl;

    /***********************Sockek_CAN config*****************************/
    static int socket_word, nbytes;
    static struct sockaddr_can addr;
    static struct ifreq ifr;
    static struct can_frame frame;
    static struct can_filter rfliter[1];
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
    cout << "Receiving: ";
    if(id == VEHICLE_ACC_ID)
        cout << "(vehicle acc)";
    else if(id == VEHICLE_SPEED_ID)
        cout << "(vehicle speed)";
    else if(id == UWB_POSITION_ID)
        cout << "(UWB position)";
    else if(id == UWB_LEADERSTATE_ID)
        cout << "(UWB leader state)";
    cout << " CAN ID 0x" << hex << id << ": ";
    static int CAN_msg[8] = {0};
    for (int i=0;i<8;i++){
        CAN_msg[i] = (int)frame.data[i];
        cout << CAN_msg[i] << " " ;
    }
    cout << endl;

    close(socket_word);
    return CAN_msg;

}
//get CAN message

/*
void Communication::CAN1_update(){
    for(;;){
        time_t can2_time;
        can2_time = time(NULL);
        Follower_velocity = 0;
        Follower_acceleration = 0;
        usleep(SAMPLE_TIME);
        cout << "CAN2 data is updating..." << endl;
        //cout << "CAN2 data is updating..." << "  Time is " << can2_time << endl;
    }
}
*/
// CAN1 update

/********************************Convert value to CAN message************************************/
int * Communication::Con2CAN_steer(int steer_enable,int steer_angle,int steer_velocity){
    static int msg_steer[8] = {0};
    msg_steer[0] = steer_enable;
    msg_steer[1] = steer_velocity/4;
    msg_steer[2] = steer_angle%256;//TODO:H L error
    msg_steer[3] = steer_angle/256;
    return msg_steer;
}
//Convert steer information to CANmsg

int * Communication::Con2CAN_acc(int control_mode,int acc_value, int pressure_value){
    static int msg_acc[8] = {0};
    static int loop_number = 0;
    msg_acc[2] = control_mode;
    if(control_mode==0){//mode 0:No Brake
        msg_acc[0] = 0;
        msg_acc[1] = 0;
    }
    else if(control_mode==1){//mode 1:Require deacc
        msg_acc[0] = acc_value%256;
        msg_acc[1] = acc_value/256;
    }
    else if(control_mode==2){//mode 2:Require pressure
        msg_acc[0] = pressure_value%256;
        msg_acc[1] = pressure_value/256;
    }
    else if(control_mode==3){//mode 3:Require acc
        msg_acc[0] = acc_value%256;
        msg_acc[1] = acc_value/256;
    }
    msg_acc[7] = loop_number%16;
    loop_number ++;
    return msg_acc;
}
//Convert acc information to CANmsg
/************************************************************************************************/

/********************************Convert CAN message to value************************************/
void Communication::CAN2Val_acc(int *CANmsg_acc,int msg_length){
    Follower_acceleration = CANmsg_acc[0] + CANmsg_acc[1];//TODO:resolution
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

void Communication::CAN2Val_UWB_leaderstate(int*CANmsg_UWB_state,int msg_length){
    //CANmsg_UWB_state[8] TODO: convert CAN message to value
}
//Convert CANmsg to leader state value

void Communication::CAN2Val_speed(int*CANmsg_speed,int msg_length){
    Follower_velocity = CANmsg_speed[6] + CANmsg_speed[7] * 256;
}
//Convert CANmsg to follower speed
/************************************************************************************************/