#include "Leader_following.h"

using namespace std;
// Define and initialize signals from follower
int UWB_distance = 20 ;
int UWB_fangwei = 0 ;
int UWB_zitai = 0 ;

int Follower_Speed = 0 ;
int Follower_La_acc = 0 ;

//Define and initialize signals from leader
int Leader_ACC_pedal_position = 0;
int Leader_Remote_position = 0;
int Leader_Brake_pedal_position = 0;
int Leader_Actual_acc = 0;
int Leader_Speed = 0;

int Leader_Pressure = 0;
int Leader_Steering_wheel_angle = 0;
int Leader_Steering_wheel_speed = 0;
int Leader_Steering_wheel_state = 0;
int Leader_Count = 0;
int Leader_Check = 0;
int Leader_Wheel_speed = 0;
int Leader_La_acc = 0;
int Leader_Yr_speed = 0;
int Leader_Target_gear = 0;
int Leader_Current_gear = 0;
int Leader_Acc_pedal = 0;
int Leader_Brake_pedal = 0;

//Define and initialize control signals
int Control_steer_angle = 32767 ;
int Control_steer_enable = 0 ;
int Control_steer_velocity = 56 ;

int Control_mode = 2;
int Control_acceleration = 0;
int Control_pressure = 0.5;

//Define Statemechine
int State = 2;
bool Signal_emergency = 0;
bool Command_ready = 0;
bool Command_run = 0;
bool Command_finish = 0;
bool Command_end = 0;

float err_integral = 0;

int main()
{
    Communication follower_communication;
    Control follower_control;
    Statemechine follower_state_mechine;

    thread th0(&follower_state_mechine.state_transition);
    thread th1(&follower_communication.CAN_receive,UWB_POSITION_MSG);//receive UWB position (args:ID CANchannel)
    thread th2(&follower_communication.CAN_receive,UWB_LEADERSTATE_MSG);//receive UWB leader_state
    //thread th3(&follower_communication.CAN_receive,VEHICLE_SPEED_MSG);//receive follower speed
    thread th6(&follower_communication.CAN_receive,VEHICLE_ACC_MSG);//receive follower ACC

    thread th4(&follower_control.Control_update);//control signal update

    thread th5(&follower_communication.CAN0_update);//send control signals

    th0.join();
    th1.join();
    th2.join();
    //th3.join();
    th6.join();
    th4.join();
    th5.join();
    
    return 0;
}
