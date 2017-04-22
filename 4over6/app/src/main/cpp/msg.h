//
// Created by Administrator on 2017/3/19 0019.
//

#ifndef INC_4OVER6_MSG_H
#define INC_4OVER6_MSG_H


#include <stdint.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define IP_REQUEST 100
#define IP_RESPONSE 101
#define NET_REQUEST 102
#define NET_RESPONSE 103
#define IP_HEARTBEAT 104

struct msg {
    int length;		//整个结构体的字节长度
    char type;		//类型
    char data[4096];	//数据段
};

struct IPResponse {
      uint32_t  addr;
      uint32_t route;
      uint32_t DNS[3];
  };

/*
 * pipenames:
 *      IPPipe  :  used to write IPResponse to frontend
 *      tnuPipe :  used to write tnu (a single int) to backend
 *      flowPipe : used to write readFlow writeFlow readTimes writeTimes ( 4 ints ) to frontEnd
 * */

void writePipe(std::string pipeName, void* data, int length);

int readPipe(std::string pipeName, void* data, int length);

#endif //INC_4OVER6_MSG_H
