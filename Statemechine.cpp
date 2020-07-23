//
// Created by nvidia on 20-6-26.
//
#include "Leader_following.h"
using namespace std;
/*static const char help_string[] =
        "\n"
        "\n"
        "usage: instruction\n"
        "\n"
        " r: Ready to go!\n"
        " s: Stop!\n"
        "\n";*/
void Statemechine::state_transition(){
    bool command_ready = 0;
    bool command_run = 0;
    bool command_finish = 0;
    bool command_end = 0;
    for(;;){
        Statemechine::get_cmd();
        Statemechine::stateflow();
    }
}
void Statemechine::get_cmd(){
    char cmd[10];
    std::string State_name[5] = {"Driver_mode","Ready_state","Run_state","Finsh_state","Emergency_state"};
    cout << "Current state is: " << State_name[State] << ". ";
    cout << "Please enter your instruction:";
    cin >> cmd;
    cout << "Your instruction is: " << cmd << "\n";

    int i = 0;
    char str_ready[] = "ready",str_run[] = "run",str_finish[] = "fi",str_end[] = "end",str_emergency[] = "e",str_show[]="show";

    if (strcmp(cmd,str_ready)==0)
        i = 1;
    else if (strcmp(cmd,str_run)==0)
        i = 2;
    else if (strcmp(cmd,str_finish)==0)
        i = 3;
    else if (strcmp(cmd,str_end)==0)
        i = 4;
    else if (strcmp(cmd,str_emergency)==0)
        i = 5;
    else if (strcmp(cmd,str_show)==0)
        i = 6;

    switch(i){
        case 1:{
            Command_ready = 1;
            Command_run = 0;
            Command_finish = 0;
            Command_end = 0;
            cout << "Ready Commmand " << endl;
            break;
        }
        case 2:{
            Command_ready = 0;
            Command_run = 1;
            Command_finish = 0;
            Command_end = 0;
            string str_speed;
            cout << "Run Command, current desired speed is " << Desired_speed << " km/h." << endl;
            cout << "Please enter your desired speed:";
            cin >> str_speed;
            /*
            try{

            }
             */
            int input_speed = stoi(str_speed);
            //cout << stoi(str_speed) << endl;//TODO:ADD ERROR HANDLING

            if(input_speed > 0 && input_speed <= 30)
                Desired_speed = input_speed;
            break;
        }
        case 3:{
            Command_ready = 0;
            Command_run = 0;
            Command_finish = 1;
            Command_end = 0;
            cout << "Finish Command " << endl;
            break;
        }
        case 4:{
            Command_ready = 0;
            Command_run = 0;
            Command_finish = 0;
            Command_end = 1;
            cout << "End Command " << endl;
            break;
        }
        case 5:{
            Signal_emergency = 1;
            Command_ready = 0;
            Command_run = 0;
            Command_finish = 0;
            Command_end = 0;
            cout << "Emergency signal" << endl;
            break;
        }
        case 6:{
            cout << "Print current state ... " << endl;
            Show_switch = 1;
        }
        default:
            cout << "Please enter instruction.";
    }
}

void Statemechine::stateflow(){
    switch(State){
        case DRIVER_MODE:{
            if(Command_ready == 1)
                State = READY_STATE;
            else if(Command_end == 1)
                State = DRIVER_MODE;
            break;
        }
        case READY_STATE:{
            if(Signal_emergency == 1)
                State = EMERGENCY_STATE;
            else if(Command_run == 1)
                State = RUN_STATE;
            break;
        }
        case RUN_STATE:{
            if(Signal_emergency == 1)
                State = EMERGENCY_STATE;
            else if(Command_finish == 1)
                State = FINISH_STATE;
            break;
        }
        case FINISH_STATE:{
            if(Command_end== 1)
                State = DRIVER_MODE;
            else if(Command_ready == 1)
                State = READY_STATE;
            break;
        }
        case EMERGENCY_STATE:{
            if (Command_end == 1)
                State = DRIVER_MODE;
            break;
        }
    }
}