//
// Created by Administrator on 2017/3/19 0019.
//

#include "msg.h"
#include "CLog.h"

void writePipe(std::string pipeName, void *data, int length) {
    int pipe = open(pipeName.c_str(), O_RDWR|O_CREAT|O_TRUNC);
    if(pipe <= 0) {
        LOGD("cannot write pipe %s",pipeName.c_str());
        return;
    }
    write(pipe, data, length);
    close(pipe);
}

int readPipe(std::string pipeName, void *data, int length) {
    int pipe = open(pipeName.c_str(), O_RDWR|O_CREAT);
    if(pipe <= 0) {
        LOGD("cannot read pipe %s",pipeName.c_str());
        return -1;
    }
    int size = read(pipe, data, length);
    close(pipe);
    return size;
}



