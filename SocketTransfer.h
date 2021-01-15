//
// Created by feng on 2020/10/30.
//

#ifndef RK3399_SPI_SOCKETTRANSFER_H
#define RK3399_SPI_SOCKETTRANSFER_H
#include <sys/socket.h>
#include <string>
#include "MonitorUnit.h"
#include "ControlUnit.h"
#include "config.h"
#include "Serial.h"
#include <iostream>
class SocketTransfer {

    using u8 = unsigned char;
    using u32 = unsigned int;
    const int port = 80;
    int listen_fd= -1;
    Serial& serial;
    MonitorUnit& monitorUnit;
    ControlUnit& controlUnit;

    static constexpr u8 MAX_BUF_SIZE = 32;
    static constexpr u8 MIN_BUF_SIZE = 4;
    const int CommandToAddr[4][32] = CommandMap;
public:
    static void showMessage(u8* data,int len){
        for(int i = 0; i < len;++i){
            if(i == 0){
                printf("reveive message:[%u,",data[i]);
            }
            else if(i == len -1){
                printf("%u]\n",data[i]);
            }
            else{
                printf("%u,",data[i]);
            }
        }
    }
    SocketTransfer(MonitorUnit &monitorUnit, ControlUnit& controlUnit, Serial& serial,int port = SOCKET_PORT);
    void connectAndRespons();

};


#endif //RK3399_SPI_SOCKETTRANSFER_H
