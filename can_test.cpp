#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <iostream>

int main(){
    int s,nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame[2] = {{0}};
    std::cout << "Flag1" << std::endl;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    strcpy(ifr.ifr_name,"can0");

    ioctl(s,SIOCGIFINDEX,&ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s,(struct sockaddr *)&addr,sizeof(addr));
    setsockopt(s,SOL_CAN_RAW,CAN_RAW_FILTER,NULL,0);
    std::cout << "Flag2" << std::endl;

    frame[0].can_id = 0x11;
    frame[0]. can_dlc = 1;
    frame[0].data[0] = 'Y';
    frame[0].can_id = 0x22;
    frame[0]. can_dlc = 1;
    frame[0].data[0] = 'N';
    //循环发送两个报文

    while(1)
    {
        nbytes = write(s,&frame[0],sizeof(frame[0]));
        if(nbytes != sizeof(frame[0]))
        {
            std::cout << "Send Error Frame[1]" << std::endl;
            break;
        }
        sleep(1);
        std::cout << "updating ..." << std::endl;
    }
    close(s);
    return 0;
}
