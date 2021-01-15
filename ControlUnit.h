//
// Created by feng on 2020/10/20.
//

#ifndef RK3399_SPI_CONTROLUNIT_H
#define RK3399_SPI_CONTROLUNIT_H
#include "config.h"
#include "SpiTransfer.h"
#include "SpinMutex.h"
#include <deque>
#include <chrono>
#include <vector>
class ControlUnit {
    typedef unsigned char u8;
    typedef unsigned int u32;
    struct Message{
        u8 fpga = 0;
        u8 len = 0;
        u8* data = nullptr;
        Message()= default;
        Message(u8 fpga,u8 len,u8* data):fpga(fpga),len(len),data(data){}
        ~Message(){
            delete data;
        }
    };
    std::deque<Message*> messageDeque;
    SpinMutex spin;
    static u8 ComponentNo[CONTROL_PAGE_NUM];
public:
    ControlUnit(){
        SpiTransfer::init();
        u8 data[4] = {0,0,0,0};
        for(u8 i = 0;i < 4;++i){
            data[0] = i;
            sendControlMessageToFpga(data,4);
        }
    };
    ControlUnit(ControlUnit&) = delete;
    ControlUnit& operator=(ControlUnit&) = delete;
    void updateMessages();
    template <u8 N>
    void sendControlMessageToFpga(const u8 (&src)[N]);
    void sendControlMessageToFpga(const u8* src,u8 N);
private:
    void putMessageToDeque(Message* msg);
    static int writeToFpga(const u8 *data, const u8* addr,size_t len){
        return SpiTransfer::writeToFpga(data,addr,2);
    }


};

template<ControlUnit::u8 N>
void ControlUnit::sendControlMessageToFpga(const u8 (&src)[N] ) {
    //机器人不同组件编号
    u8 x = src[0];
    //重新申请内存，独占
    u8* data = new u8[N];
    memcpy(data,src,N);
    data[0] = ComponentNo[x];
    putMessageToDeque(new Message(x*CONTROL_PAGE_SIZE_NUM,N,data));
    ComponentNo[x] = ComponentNo[x] >= 255 ? 1 : ComponentNo[x]+1;
}
#endif //RK3399_SPI_CONTROLUNIT_H
