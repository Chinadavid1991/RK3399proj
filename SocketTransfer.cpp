//
// Created by feng on 2020/10/30.
//

#include <netinet/in.h>
#include <unistd.h>
#include "SocketTransfer.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <thread>
#include "MonitorUnit.h"
#include "ControlUnit.h"
using namespace std;


SocketTransfer::SocketTransfer(MonitorUnit& monitorUnit, ControlUnit& controlUnit,Serial& serial, const int port):
                port(port),monitorUnit(monitorUnit),controlUnit(controlUnit),serial(serial){
    int ret = 0;
    sockaddr_in t_sockaddr = {};
    t_sockaddr.sin_family = AF_INET;
    t_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    t_sockaddr.sin_port = htons(port);
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        fprintf(stderr, "socket error %s errno: %d\n", strerror(errno), errno);
    }
    ret = bind(listen_fd,(struct sockaddr *) &t_sockaddr,sizeof(t_sockaddr));
    if (ret < 0) {
        fprintf(stderr, "bind socket error %s errno: %d\n", strerror(errno), errno);
    }
    ret = listen(listen_fd, 1024);//
    if (ret < 0) {
        fprintf(stderr, "listen error %s errno: %d\n", strerror(errno), errno);
    }


}

void SocketTransfer::connectAndRespons() {

    u8 buf[MAX_BUF_SIZE] = {0};//监视数据长度为4，控制数据为4+X
    for(;;) {
        int conn_fd = accept(listen_fd, (struct sockaddr*)nullptr, nullptr);
        if(conn_fd < 0) {
            fprintf(stderr, "accpet socket error: %s errno :%d\n", strerror(errno), errno);
            continue;
        }
        memset(buf,0,MAX_BUF_SIZE);
        int recv_len = recv(conn_fd, buf, MAX_BUF_SIZE, 0);
        showMessage(buf,recv_len);
        std::cout << "recv_len is:" << recv_len << std::endl;

        if(MIN_BUF_SIZE == recv_len && 13 == buf[2] && 3 == buf[0]){
            char data[8];
            serial.getData(data);
            send(conn_fd, data,8,0);//发送数据
        }
        else if(MIN_BUF_SIZE == recv_len)//监视指令
        {	
	    
            u8 data[16];
            u8 len = 0;
	    int cmd = buf[2] -1;
	    cmd = CommandToAddr[buf[0]][cmd];
            if(monitorUnit.getMonitorData(buf[0],cmd,data,len) < 0){
                fprintf(stderr, "reveive data: %s errno :%d\n", strerror(errno), errno);
            }
	    if(18 == buf[2]){
                printf("##########[%d,%d]\n",data[0],data[1]);		    
	    }
            send(conn_fd, data,len,0);//发送数据
        }
        else if(recv_len <= MAX_BUF_SIZE  && recv_len > MIN_BUF_SIZE){
            send(conn_fd,buf,MIN_BUF_SIZE,0);
            controlUnit.sendControlMessageToFpga(buf,recv_len);
        }
        else {
                //printf("receive message error len:%d\n",recv_len);
        }
       
        close(conn_fd);
    }

}

