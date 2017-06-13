//
// Created by Administrator on 2017/4/16 0016.
//

#include <unistd.h>
#include "TnuReader.h"
#include "msg.h"
#include "BackEnd.h"
#include "v4Packet.h"

TnuReader::TnuReader(int tnu, int serverSocket,int allocedAddr) {
    this->tnu = tnu;
    this->serverSocket = serverSocket;
    this->allocedAddr = allocedAddr;
}

void TnuReader::readTnu() {
    char buffer[4096];
    int readLen = 0;
    msg m;
    m.type = NET_REQUEST;
    while((readLen = read(this->tnu, buffer, 4096)) <= 0) {
        /* wait */
    }
    //LOGD("read tnu packet size %d", readLen);
    memcpy(m.data, buffer, readLen);
    m.length = sizeof(m.type) + sizeof(m.length) + readLen;

    v4Packet *packet = (v4Packet*) buffer;
    in_addr srcAddr;
     packet->head.srcAddr = allocedAddr;
    /*srcAddr.s_addr = packet->head.srcAddr;
    LOGD("srcIP of packet: %s",inet_ntoa(srcAddr));*/
    srcAddr.s_addr = packet->head.dstAddr;
    //LOGD("dstIP of packet: %s",inet_ntoa(srcAddr));


    int writeLen = 0;

    writeLen = write(this->serverSocket, (void*)&m, m.length);
    /*LOGD("send server a packet size %d", writeLen);*/
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







