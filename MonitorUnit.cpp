//
// Created by feng on 2020/10/20.
//

#include "MonitorUnit.h"
#include <thread>
#include <chrono>
#include <iostream>
using namespace std;

const  MonitorUnit::u8 MonitorUnit::infoBitLen[PAGE][PAGESIZE]=MonitorUnitMap;
void MonitorUnit::updateMessages() {
    thread t([this]{
            u8 tx[2] = {0,0};  //接收的数据在第二个Byte
            u8 rx[2] = {0,0};//发送的第一个Byte为需要获取数据的地址
            while (true){
                auto dura =  chrono::system_clock::now() + chrono::milliseconds(10);
                for(size_t i = 0;i < PAGE*PAGESIZE;++i){
                    Message* msg = shareArray+i;
                    if(!msg->flag){
                        continue;
                    }
                    spin.lock();
                    for(size_t j = 0;j < msg->len;++j){
                        tx[0] = msg->fpga + j;
                        readFromFpga(rx, tx, 2);
                        memcpy(msg->data+j,rx + 1,1);//拷贝接收到的1Byte数据
                        memset(rx,0,2);
                    }
                    spin.unlock();

                }
                this_thread::sleep_until(dura);
            }
    });
    t.detach();
}

MonitorUnit::u8
MonitorUnit::getMonitorData(u8 page, u8 cmd, u8 *data, u8 &N) {
    Message* msg(shareArray+(page*PAGESIZE+cmd-MONITOR_PAGE_INDEX));
    spin.lock();
    memcpy(data,msg->data,msg->len);
    N = msg->len;
    spin.unlock();
    return N;
}






