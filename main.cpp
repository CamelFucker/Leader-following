#include "Leader_following.h"

using namespace std;

int UWB_distance = 20 ;
int UWB_fangwei = 0 ;
int UWB_zitai = 0 ;
int Leader_velocity = 0 ;
int Leader_acceleration = 0 ;
int Follower_velocity = 0 ;
int Follower_acceleration = 0 ;

int Control_steer_angle = 32767 ;
int Control_steer_enable = 0 ;
int Control_steer_velocity = 56 ;

int Control_mode = 2;
int Control_acceleration = 0;
int Control_pressure = 0.5;

int main()
{
    cout << "Main Thread" << endl;

/*
    Communication follower_communication;
    Control follower_control;

    thread th1(&follower_communication.CAN1_update);
    thread th2(&follower_communication.CAN2_update);
    thread th3(&follower_control.Control_update);
    th1.join();
    th2.join();
    th3.join();
*/

    int * canmsg;
    while(1)
    {
        canmsg = follower_communication.CAN_receive(23);
        cout << *(canmsg) << " haha " << endl;
        usleep(SAMPLE_TIME);
    }

    /* CAN Send in main thread
    int test_message = 234;
    while(1){
        follower_communication.CAN_send(test_message);
        usleep(200000);
    }
    */

    return 0;
}
