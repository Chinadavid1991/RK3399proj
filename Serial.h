//
// Created by feng on 2021/1/5.
//

#ifndef RK3399_SPI_SERIAL_H
#define RK3399_SPI_SERIAL_H

#include <deque>
#include "SpinMutex.h"
class Serial {
    struct Data {
        int x;
        int y;
        Data():x(0),y(0){}
        Data(int x, int y) : x(x), y(y) {}
    }_data{0,0};
    const char *_port;
    int _serial_fd = -1;
    using v_ref = char (&)[8];
    std::deque<int> vecX;
    std::deque<int> vecY;
    SpinMutex spin;

public:
    Serial(const char *port = "/dev/ttysWK3");
    void getData(v_ref data);
    void getData();
    virtual ~Serial() = default;

private:
    int set_interface_attribs(int speed);

    static int UWBFilter(const std::deque<int> &vec);
};


#endif //RK3399_SPI_SERIAL_H
