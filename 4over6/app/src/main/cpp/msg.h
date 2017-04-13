//
// Created by Administrator on 2017/3/19 0019.
//

#ifndef INC_4OVER6_MSG_H
#define INC_4OVER6_MSG_H


#include <stdint.h>
#include <string>

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

void writePipe(std::string pipeName, void* data, int length);

int readPipe(std::string pipeName, void* data, int length);

#endif //INC_4OVER6_MSG_H
