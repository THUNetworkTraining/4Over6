//
// Created by Administrator on 2017/4/16 0016.
//

#include <unistd.h>
#include "TnuReader.h"
#include "msg.h"
#include "BackEnd.h"

TnuReader::TnuReader(int tnu, int serverSocket) {
    this->tnu = tnu;
    this->serverSocket = serverSocket;
}

void TnuReader::readTnu() {
    char buffer[4096];
    size_t readLen = 0;
    msg m;
    m.type = NET_REQUEST;
    while((readLen = read(this->tnu, buffer, 4096)) <= 0) {
        /* wait */
    }
    memcpy(m.data, buffer, readLen);
    m.length = sizeof(m.type) + sizeof(m.length) + readLen;
    write(this->serverSocket, (void*)&m, m.length);
    BackEnd::writeFlow += readLen;
    BackEnd::writeTimes ++;
}

void TnuReader::run() {
    while(1){
        readTnu();
    }
}

void TnuReader::sRun(TnuReader *ptr) {
    ptr->run();
}







