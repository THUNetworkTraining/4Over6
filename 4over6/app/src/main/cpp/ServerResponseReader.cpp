//
// Created by Administrator on 2017/4/16 0016.
//

#include "ServerResponseReader.h"
#include "msg.h"
#include "CLog.h"
#include "BackEnd.h"


void ServerResponseReader::readResponse() {
    int readLen = 0;
    static msg m;
    while((readLen = read(this->serverSocket, &m, sizeof(m))) <= 0){
        /* wait */
    }
    if(m.type == NET_RESPONSE){
        processNetResponse(m.data, readLen - sizeof(m.type) - sizeof((m.length)));
    }
    else if(m.type == IP_HEARTBEAT) {
        processHeartbeat();
    }
    else {
        LOGE("wrong msg type from server : %d",m.type);
    }
}

void ServerResponseReader::processNetResponse(char *data, int len) {
    if(write(this->tnu, data, len) <= 0){
        LOGE("write net response to tnu failed");
    }
}

ServerResponseReader::ServerResponseReader(int serverSocket, int tnu) {
    this->serverSocket = serverSocket;
    this->tnu = tnu;
}

void ServerResponseReader::processHeartbeat() {
    time(&BackEnd::lastHeartbeatTime);
}

void ServerResponseReader::run() {
    while(1) {
        this->readResponse();
    }
}

void ServerResponseReader::sRun(ServerResponseReader *ptr) {
    ptr->run();
}













