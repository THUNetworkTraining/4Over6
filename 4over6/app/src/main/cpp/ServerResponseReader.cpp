//
// Created by Administrator on 2017/4/16 0016.
//

#include "ServerResponseReader.h"
#include "msg.h"
#include "CLog.h"
#include "BackEnd.h"


void ServerResponseReader::readResponse() {
    LOGD("reading response");
    static char buffer[10000];
    static int bufferPtr = 0;

    //read header
    bufferPtr = 0;
    int readLen = 0;
    while(bufferPtr < MSG_HEADER_SIZE){
        readLen = read(this->serverSocket, buffer + bufferPtr, MSG_HEADER_SIZE - bufferPtr);
        if(readLen < 0){
            LOGE("Error reading msg header from server, %s\n",strerror(errno));
        }
        else{
            bufferPtr += readLen;
        }
    }
    // judging from type
    msg *m = (msg*) buffer;
    LOGD("receive a packet from server readLen type %d length %d",m->type,m->length);
    int dataToReadLen = 0;
    switch(m->type){
        case NET_RESPONSE :
            dataToReadLen = m->length - MSG_HEADER_SIZE;
            while(dataToReadLen > 0){
                readLen = read(this->serverSocket, buffer + bufferPtr, dataToReadLen);
                if(readLen < 0){
                     LOGE("Error reading msg data from server, %s\n",strerror(errno));
                 }
                 else{
                     bufferPtr += readLen;
                     dataToReadLen -= readLen;
                 }
            }
            processNetResponse(buffer + MSG_HEADER_SIZE, m->length - MSG_HEADER_SIZE);
            break;
        case IP_HEARTBEAT :
            LOGE("Heartbeat!");
            processHeartbeat();
            break;
        default :
            LOGE("Wrong type from server : %d", m->type);
    }
    /*int readLen = 0;
    static msg m;
    static char buffer[10000];
    static char lastData[20000];
    static int lastDataSize = 0;
    static int unReadLength = 0;
    static char* bufferPtr = buffer;
    while((readLen = read(this->serverSocket, bufferPtr, sizeof(msg))) <= 0){
    }
    bufferPtr = buffer;
    LOGD("receive a packet from server readLen %d",readLen);

    bool tooBig = 0;
    // process data until empty
    do{
    // last packet is split, need a part of current packet to complete it
    if(unReadLength > 0) {
            int sizeToCut = unReadLength < readLen ? unReadLength : readLen;
            LOGD("cut %d bytes from coming packet",sizeToCut);
            memcpy(lastData+lastDataSize, buffer, sizeToCut);
            bufferPtr += sizeToCut;
            lastDataSize += sizeToCut;
            readLen -= sizeToCut;
            unReadLength -= sizeToCut;
            if(unReadLength == 0){      // last packet is completed, send to tun
                  processNetResponse(lastData, lastDataSize);
            }
            LOGD("cut over remain %d",readLen);
        }
        if(readLen <= 0){
            LOGD("packet totally cut");
            return;
        }
    // try to treat buffer as a msg
    memcpy(&m,bufferPtr, readLen);

    // incoming data is shorter than a header, buffer it and wait for next packet
    if(readLen < 5) {
       bufferPtr = buffer;
       memcpy(bufferPtr, &m, readLen);
       bufferPtr += readLen;
       return;
    }
     LOGD("present packet readLen %d  type %d pLen %d", readLen, m.type, m.length);
    if(m.type == NET_RESPONSE){
        if(m.length < 5 || m.length > 5000){ // discard garbage packet
            return;
        }
        // last packet is unsent, send it to tun
        if(lastDataSize != 0) {
                    //LOGD("flush previous packet");
                    processNetResponse(lastData, lastDataSize);
                }
        // incoming packet is smaller than a full msg, buffer it until the msg is completed
        if(m.length >= readLen) {
            lastDataSize = readLen - sizeof(m.type) - sizeof((m.length));
            tooBig = 0;
            unReadLength = m.length - readLen;
            memcpy(lastData,m.data,lastDataSize);
            if(unReadLength > 0)
                LOGD("packet too small, need %d bytes from next packet",unReadLength);
        }
        // there is left data after processing a msg, maybe packets are stuck together
        else {
            LOGE("an oversized packet");
            lastDataSize = m.length - sizeof(m.type) - sizeof((m.length));
            memcpy(lastData,m.data,lastDataSize);
            processNetResponse(lastData, lastDataSize);
            bufferPtr += m.length;
            readLen -= m.length;
            tooBig = 1;
        }
    }
    else if(m.type == IP_HEARTBEAT) {
        if(m.length < 5 || m.length > 5000){ // discard garbage packet
            return;
        }
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
    bufferPtr = buffer;*/
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













