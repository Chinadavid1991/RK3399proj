cmake_minimum_required(VERSION 3.10)
project(RK3399_spi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -pthread")
add_executable(RK3399_spi main.cpp SpiTransfer.cpp SpiTransfer.h
        MonitorUnit.cpp MonitorUnit.h ControlUnit.cpp ControlUnit.h SpinMutex.h SocketTransfer.cpp SocketTransfer.h config.h Serial.cpp Serial.h)
