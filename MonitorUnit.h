//
// Created by feng on 2020/10/20.
//

#ifndef RK3399_SPI_MONITORUNIT_H
#define RK3399_SPI_MONITORUNIT_H
#include "SpiTransfer.h"
#include "SpinMutex.h"
#include "config.h"
#include <atomic>
#include <cmath>
#include <memory.h>
#include <cassert>
class MonitorUnit{
    typedef unsigned char u8;
    typedef unsigned int u32;
    struct Message{
        bool flag = false;
        u8 fpga = 0;
        u8 len = 0;
        u8* data = nullptr;
        Message()= default;
        Message(bool flag, u8 fpga,u8 len,u8* data):flag(flag),fpga(fpga),len(len),data(data){}
        ~Message(){
            delete data;
        }
    };

    static constexpr u8 PAGE = MONITOR_PAGE_NUM;
    static constexpr u32 PAGESIZE = MONITOR_PAGE_SIZE_NUM;
    static const  u8 infoBitLen[PAGE][PAGESIZE];
    Message*  shareArray;
    SpinMutex spin;
public:
    MonitorUnit():shareArray(new Message[PAGE*PAGESIZE+1]){
        SpiTransfer::init();
        //初始化消息块
        for (int i = 0; i < PAGE; ++i) {
            u8 ix = i*PAGESIZE;
            u8 len = 0;
            for (int j = 0; j < PAGESIZE; ++j) {
                ix += len;//加上当前指令前所以数据长度
                len = infoBitLen[i][j];
                if(0 == len){
                    continue;
                }
                shareArray[i*PAGESIZE+j].flag = true;
                shareArray[i*PAGESIZE+j].fpga = ix;
                shareArray[i*PAGESIZE+j].data = new u8[len];
                shareArray[i*PAGESIZE+j].len = len;
            }
        }
    };

    MonitorUnit(const MonitorUnit&) = delete;
    MonitorUnit& operator=(const MonitorUnit&) = delete;
    void updateMessages();
    u8 getMonitorData(u8 page,u8 cmd,u8* data,u8& N)  ;

    template <u8 N>
    u8 getMonitorData(u8 page,u8 cmd,u8 (&data)[N])  ;
    virtual ~MonitorUnit(){
        if (shareArray != nullptr) {
            delete[] shareArray;
            shareArray = nullptr;
        }
    };
private:
    static int readFromFpga(const u8 *data, const u8* addr,size_t len){
        return SpiTransfer::readFromFpga(data, addr, len);
    }

};
template<MonitorUnit::u8 N>
MonitorUnit::u8 MonitorUnit::getMonitorData(u8 page, u8 cmd, u8 (&data)[N] ) {
    Message* msg(shareArray+page*PAGESIZE+cmd-MONITOR_PAGE_INDEX);
    assert(N == msg->len);
    spin.lock();
    memcpy(data,msg->data,msg->len);
    spin.unlock();
    return N;
}

#endif //RK3399_SPI_MONITORUNIT_H
