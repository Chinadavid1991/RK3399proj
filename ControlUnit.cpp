//
// Created by feng on 2020/10/20.
//

#include "ControlUnit.h"
#include <memory.h>
#include <iostream>
using namespace std;
unsigned char ControlUnit::ComponentNo[4] = {0,0,0,0};
void ControlUnit::updateMessages(){
    thread t([this]{
        unsigned char tx[2] = {0,0};//第一个Byte为fpga地址,第二个字节为控制指令
        unsigned char rx[2] = {0,0};
        while (true){
            if(!messageDeque.empty()){
                //do spi transfer
                auto dura =  chrono::system_clock::now() + chrono::milliseconds(5);
                spin.lock();
                Message* msg = messageDeque.back();
                messageDeque.pop_back();
                spin.unlock();//quick to unlock
                for(size_t i = 1;i < msg->len;++i){
                    tx[0] = (msg->fpga + i) | 0x80;
                    tx[1] = (unsigned char)*(msg->data+i);
                    //printf("addr:%x || data: %d \n",tx[0],tx[1]);
                    while (writeToFpga(tx,rx,2) < 0);
                    
                }
                if(msg->len > 0){
                    tx[0] = msg->fpga | 0x80;
                    tx[1] = (unsigned char)*(msg->data);
                    while (writeToFpga(tx,rx,2) < 0);
                }
                delete msg;
                this_thread::sleep_until(dura);
            }
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    });
    t.detach();
}

void ControlUnit::putMessageToDeque(ControlUnit::Message *msg) {
    thread t([this,msg]{
        spin.lock();
        messageDeque.push_front(msg);
        spin.unlock();
    });
    t.detach();
}



void ControlUnit::sendControlMessageToFpga(const u8 *src,u8 N) {
    //机器人不同组件编号
    u8 x = src[0];
    //重新申请内存，独占
    u8* data = new u8[N];
    memcpy(data,src,N);
    data[0] = ComponentNo[x];
    putMessageToDeque(new Message(x*CONTROL_PAGE_SIZE_NUM,N,data));
    ComponentNo[x] = ComponentNo[x] >= 255 ? 1 : ComponentNo[x]+1;
}





