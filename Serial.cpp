//
// Created by feng on 2021/1/5.
//

#include "Serial.h"
#include <cerrno>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <deque>
#include <algorithm>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;

int Serial::UWBFilter(const std::deque<int> &vec) {
    int sum = 0;
    for (auto item : vec) {
        sum += item;
    }
    sum = sum / (int) vec.size();
    std::vector<std::pair<int, int> > res;
    for (auto item : vec) {
        int diff = std::abs(item - sum);
        res.emplace_back(diff, item);
    }
    std::sort(res.begin(), res.end(),
              [](const std::pair<int, int> a, const std::pair<int, int> b) { return a.first < b.first; });
    int num = 0;
    int i = 0;
    for (auto var : res) {
        if (i++ < 12)
            num += var.second;
    }
    num = num / 12;
    return num;
}

Serial::Serial(const char *port) : _port(port) {
    _serial_fd = open(_port, O_RDWR | O_NOCTTY | O_SYNC);
    if (_serial_fd < 0) {
        printf("Error opening %s: %s\n", _port, strerror(errno));
    }
    set_interface_attribs(B115200);
    /* simple output */
    if (write(_serial_fd, "Hello!\n", 7) != 7) {
        printf("Error from write: %d\n", errno);
    }
    tcdrain(_serial_fd);    /* delay for output */
    cout << "init serial success" << endl;
}

int Serial::set_interface_attribs(int speed) {
    struct termios tty{};

    if (tcgetattr(_serial_fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t) speed);
    cfsetispeed(&tty, (speed_t) speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(_serial_fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}


void Serial::getData() {
    thread t([this]{
        char buf[200]{};
        int XHigh = 0, XMid = 0, XLow = 0;
        int YHigh = 0, YMid = 0, YLow = 0;
        int len = 0;
        do {
            len = read(_serial_fd, buf, sizeof(buf) - 1);
            if (len < 0) {
                printf("Error from read: %d: %s\n", len, strerror(errno));
            }
            if (buf[0] == 85 && buf[1] == 4) {
                XLow = buf[13];
                XMid = buf[14];
                XHigh = buf[15];
                YLow = buf[16];
                YMid = buf[17];
                YHigh = buf[18];
                int X = ((XHigh << 16) + (XMid << 8) + XLow);
                int Y = ((YHigh << 16) + (YMid << 8) + YLow);
                if (vecX.size() < 20 || vecY.size() < 20) {
                    vecX.push_front(X);
                    vecY.push_front(Y);
                }
                else{
                    vecX.push_front(X);
                    vecY.push_front(Y);
                    vecX.pop_back();
                    vecY.pop_back();
                    int x_ = UWBFilter(vecX);
                    int y_ = UWBFilter(vecY);
                    spin.lock();
                    _data.x = x_;
                    _data.y = y_;
                    spin.unlock();
                    this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            /* repeat read to get full message */
        } while (true);
    });
    t.detach();
}

void Serial::getData(v_ref data) {
    spin.lock();
    memcpy(data,&_data.x, sizeof(int));
    memcpy(data+ sizeof(int),&_data.y,sizeof(int));
    spin.unlock();
}
