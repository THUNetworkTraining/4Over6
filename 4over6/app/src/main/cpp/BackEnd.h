//
// Created by Administrator on 2017/3/19 0019.
//

#ifndef INC_4OVER6_BACKEND_H
#define INC_4OVER6_BACKEND_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "json/json.h"
#include "CLog.h"

class BackEnd {
public:
    static long long readFlow;
    static long long writeFlow;
    static long long lastHeartbeatTime;
private:
    int serverSocket;
    struct in6_addr* serverAddr;
    short serverPort;

    int tnu;

    std::string curPath;

    std::string IPPipeName,tnuPipeName,flowPipeName;

public:
    BackEnd();
    BackEnd(std::string curPath);
    void readSettings(const char* filename);
    void initializeSocket();
    void setCurPath(std::string curPath);
    void requireIP();                           //send 100 packet to server, wait for response, and write response via IPPipe
    void readTnu();
    void establishPipes();
    void setTimer();
};


#endif //INC_4OVER6_BACKEND_H
