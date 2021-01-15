//
// Created by feng on 2020/10/20.
//

#include "SpiTransfer.h"

SpiTransfer::u8 SpiTransfer::bits = SPI_BITS;
SpiTransfer::u32 SpiTransfer::mode = SPI_MODE;
SpiTransfer::u32 SpiTransfer::speed = SPI_SPEED;
int SpiTransfer::device = -1;
int transfer(int fd,  const unsigned char *tx,  const unsigned char *rx,
             size_t len,unsigned int speed, unsigned char bits) {
    int ret;
    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)tx,
            .rx_buf = (unsigned long)rx,
            .len = static_cast<__u32>(len),
            .speed_hz = speed,
            .delay_usecs = 0,
            .bits_per_word = bits,
    };
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        printf("transfer message error...!!!\n");
    return ret;
}


int SpiTransfer::readFromFpga(const u8 *data, const u8* addr,size_t len) {
    return transfer(device, addr,data,len,speed,bits);
}

int SpiTransfer::writeToFpga(const u8 *data, const u8* addr,size_t len) {
    return transfer(device, data,addr,len,speed,bits);
}

int SpiTransfer::init(const char *spi_device) {
    static std::once_flag flag;
    std::call_once(flag,[spi_device]{
        device = open(spi_device, O_RDWR);
        int ret;
        if (device < 0)
            printf("can't open device\n");
        /*
         * spi mode
         */
        ret = ioctl(device, SPI_IOC_WR_MODE32, &mode);
        if (ret == -1)
            printf("can't set spi mode\n");

        ret = ioctl(device, SPI_IOC_RD_MODE32, &mode);
        if (ret == -1)
            printf("can't get spi mode\n");

        /*
         * bits per word
         */
        ret = ioctl(device, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
            printf("can't set bits per word\n");

        ret = ioctl(device, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1)
            printf("can't get bits per word\n");

        /*
         * max speed hz
         */
        ret = ioctl(device, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
            printf("can't set max speed hz\n");

        ret = ioctl(device, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        if (ret == -1)
            printf("can't get max speed hz\n");
        printf("bits per word: %d\n", bits);
        printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
    });

    return device;
}

bool SpiTransfer::set( u8 _bits,  u32 _mode,  u32 _speed) {
    if(ioctl(device, SPI_IOC_WR_MODE32, &_mode) >= 0 &&
       ioctl(device, SPI_IOC_RD_MODE32, &_mode) >=0 &&
       ioctl(device, SPI_IOC_WR_BITS_PER_WORD, &_bits) >= 0 &&
       ioctl(device, SPI_IOC_RD_BITS_PER_WORD, &_bits) >= 0 &&
       ioctl(device, SPI_IOC_WR_MAX_SPEED_HZ, &_speed) >= 0 &&
       ioctl(device, SPI_IOC_RD_MAX_SPEED_HZ, &_speed) >= 0 ){
        printf("bits per word: %d\n", bits);
        printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
        bits = _bits;
        mode = _mode;
        speed = _speed;
        return true;

    }
    return false;
}














