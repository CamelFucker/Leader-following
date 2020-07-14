//
// Created by nvidia on 20-7-5.
//

#ifndef LEADERFOLLOWING_STATEMECHINE_H
#define LEADERFOLLOWING_STATEMECHINE_H

#endif //LEADERFOLLOWING_STATEMECHINE_H

#define DRIVER_MODE 0
#define READY_STATE 1
#define RUN_STATE 2
#define FINISH_STATE 3
#define EMERGENCY_STATE 4

class Statemechine{
public:
    static void state_transition();
private:
    static void get_cmd();
    static void stateflow();
};