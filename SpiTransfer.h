//
// Created by feng on 2020/10/20.
//

#ifndef RK3399_SPI_SPITRANSFER_H
#define RK3399_SPI_SPITRANSFER_H
#include <future>
#include "config.h"
#include "SpinMutex.h"
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */
#include <asm/types.h>
#include <linux/spi/spidev.h>
#include <cstdio>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <cstdlib>
#include <fcntl.h>

int transfer(int fd, const unsigned char *tx, const unsigned char *rx,
             size_t len,unsigned int speed, unsigned char bits);
#ifdef __cplusplus
}
#endif /* __cplusplus */


class SpiTransfer {
    typedef unsigned char u8;
    typedef unsigned int u32;
    static int device;
    static u8 bits;
    static u32 mode;
    static u32 speed;

public:
    SpiTransfer()= delete;

    /*          spi read and write
     * data : creat receive buf by yourself
     */
    static bool set(u8 _bits = SPI_BITS,u32 _mode = SPI_MODE,u32 _speed = SPI_SPEED);
    static int init(const char *spi_device = SPI_DEVICE);
    static int readFromFpga(const u8* data,const u8* addr,size_t len = 2);
    static int writeToFpga(const u8* data,const u8* addr,size_t len = 2);

    friend int transfer(int fd,  const unsigned char *tx,  const unsigned char *rx,
                        size_t len,unsigned int speed, unsigned char bits);

    virtual ~SpiTransfer() = default;


};
#endif //RK3399_SPI_SPITRANSFER_H
