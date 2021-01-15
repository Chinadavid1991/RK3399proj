#include "MonitorUnit.h"
#include "ControlUnit.h"
#include "SocketTransfer.h"
#include "Serial.h"
#include <iostream>
#include <string>
using namespace std;

//收发接口使用
void demo(){
    MonitorUnit monitorUnit;
    ControlUnit controlUnit;
    monitorUnit.updateMessages();
    controlUnit.updateMessages();

    unsigned char buf[32] = {0};
    unsigned char len = 0;
    while(true){
   	 for(int ix = 0;ix < 32;++ix){
        	monitorUnit.getMonitorData(0,ix+1,buf,len);
		printf("[cmd is:%d]\n",ix);
        	for(int i =0;i < len;++i){
            		printf("[%d]",buf[i]);
        	}
		printf("\n");
    	}
    }
}

void testSocket(){
    MonitorUnit monitorUnit;
    ControlUnit controlUnit;
    Serial serial;
    SocketTransfer socketTransfer(monitorUnit,controlUnit,serial);
    monitorUnit.updateMessages();
    controlUnit.updateMessages();
    socketTransfer.connectAndRespons();
}

int main() {
    //demo();
    testSocket();
    return 0;
}
