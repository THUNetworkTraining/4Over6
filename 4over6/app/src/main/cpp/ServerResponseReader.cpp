//
// Created by Administrator on 2017/4/16 0016.
//

#include "ServerResponseReader.h"
#include "msg.h"
#include "CLog.h"
#include "BackEnd.h"


void ServerResponseReader::readResponse() {
    LOGD("reading response");
    int readLen = 0;
    static msg m;
    static char buffer[10000];
    static char lastData[20000];
    static int lastDataSize = 0;
    static int unReadLength = 0;
    static char* bufferPtr = buffer;
    while((readLen = read(this->serverSocket, bufferPtr, sizeof(msg))) <= 0){
        /* wait */
    }
    bufferPtr = buffer;
    LOGD("receive a packet from server readLen %d",readLen);

    bool tooBig = 0;
    do{
    if(unReadLength > 0) {
            int sizeToCut = unReadLength < readLen ? unReadLength : readLen;
            LOGD("cut %d bytes from coming packet",sizeToCut);
            memcpy(lastData+lastDataSize, buffer, sizeToCut);
            bufferPtr += sizeToCut;
            lastDataSize += sizeToCut;
            readLen -= sizeToCut;
            unReadLength -= sizeToCut;
            LOGD("cut over remain %d",readLen);
        }
        if(readLen <= 0){
            LOGD("packet totally cut");
            return;
        }
    memcpy(&m,bufferPtr, readLen);

    if(readLen < 5) {
       bufferPtr = buffer;
       memcpy(bufferPtr, &m, readLen);
       bufferPtr += readLen;
       return;
    }
     LOGD("present packet readLen %d  type %d pLen %d", readLen, m.type, m.length);
    if(m.type == NET_RESPONSE){
        if(m.length < 5 || m.length > 5000) // discard garbage packet
            return;
        if(lastDataSize != 0) {
                    LOGD("flush previous packet");
                    processNetResponse(lastData, lastDataSize);
                    lastDataSize = 0;
                }
        if(m.length >= readLen) {
            lastDataSize = readLen - sizeof(m.type) - sizeof((m.length));
            tooBig = 0;
            unReadLength = m.length - readLen;
            memcpy(lastData,m.data,lastDataSize);
            if(unReadLength > 0)
                LOGD("packet too small, need %d bytes from next packet",unReadLength);
        }
        else {
            LOGE("an oversized packet");
            lastDataSize = m.length - sizeof(m.type) - sizeof((m.length));
            memcpy(lastData,m.data,lastDataSize);
            processNetResponse(lastData, lastDataSize);
            lastDataSize=0;
            bufferPtr += m.length;
            readLen -= m.length;
            tooBig = 1;
        }
    }
    else if(m.type == IP_HEARTBEAT) {
        if(m.length < 5 || m.length > 5000) // discard garbage packet
            return;
        processHeartbeat();
        bufferPtr += m.length;
        readLen -= m.length;
        if(readLen > 0)
            tooBig = 1;
    }
    else {
        if(lastDataSize == 0) {
            LOGE("wrong msg type from server : %d",m.type);
            return;
        }
    }
    }
    while(tooBig);
    bufferPtr = buffer;
}

void ServerResponseReader::processNetResponse(char *data, int len) {
    LOGD("writing response to tun size %d",len);
    if(write(this->tnu, data, len) <= 0){
        LOGE("write net response to tnu failed %s",strerror(errno));
    }
    else{
        BackEnd::readFlow += len;
        BackEnd::readTimes ++;
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













