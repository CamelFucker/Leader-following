class Control{
private:
    // args
    static constexpr float k_v = 0.2;
    static constexpr float k_d = 0.2;
    // float signal variable
    static float leader_speed;
    static float follower_speed;
    static float leader_la_acc;
    static float follower_la_acc;
    static float fangwei_angle;
    static float zitai_angle;

    static float leader_acc_pedal_position;
    static float leader_remote_position;
    static float leader_brake_pedal_position;
    static float leader_actual_acc;
    static float leader_pressure;
    static float leader_steering_wheel_angle;
    static float leader_Steering_wheel_speed;
    //static float leader_steering_wheel_state;
    //static float leader_count;
    //static float leader_check;
    static float leader_wheel_speed;

    static float leader_yr_speed;
    //static float leader_target_gear;
    //static float leader_current_gear;
    static float leader_acc_pedal;
    static float leader_brake_pedal;

    static float long_distance;
    static float lat_distance;
    static float frontwheel_steer_angle;
    static float steer_wheel_angle;


    static float control_steer;
    static float control_acc;
    static float control_brake_pressure;
    // vehicle parads
    static const int L = 5;
    static const int r = 1;

public:
    //Control();
    //Control(float k_v, float k_d);
    static void Control_update();
    static float Caculate_steer(float lat_distance, float long_distance);
    static float Caculate_acc(float v1, float v2, float a1, float long_distance);
};
class PID{
    public:
        PID();
        ~PID(){};
        PID(float p,float i,float d);
        float pid_control(float tar,float act);
        void pid_show();
    private:
        float kp;
        float ki;
        float kd;
        float target;
        float actual;
        float error;
        float error_pre;
        float integral;
};
