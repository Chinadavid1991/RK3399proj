//
// Created by feng on 2020/10/21.
//

#ifndef RK3399_SPI_SPINMUTEX_H
#define RK3399_SPI_SPINMUTEX_H
#include <atomic>
class SpinMutex {
    std::atomic<bool> flag = {false};
public:
    SpinMutex() = default;
    SpinMutex(const SpinMutex&) = delete;
    SpinMutex& operator= (const SpinMutex&) = delete;
    void lock() {
        for (bool expected = false;!flag.compare_exchange_weak(expected, true);expected = false);
    }
    void unlock() {
        flag.store(false);
    }
};
#endif //RK3399_SPI_SPINMUTEX_H
