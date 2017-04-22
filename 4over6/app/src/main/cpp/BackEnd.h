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
#include <time.h>
#include <thread>
#include "HeartbeatTimer.h"
#include "TnuReader.h"
#include "ServerResponseReader.h"

class BackEnd {
public:
    static long long readFlow;
    static long long writeFlow;
    static long long readTimes;
    static long long writeTimes;
    static time_t lastHeartbeatTime;
private:
    int serverSocket;
    struct in6_addr* serverAddr;
    struct in_addr allocedAddr;
    short serverPort;

    int tnu;
    int timerPid;

    std::string curPath;

    HeartbeatTimer* timer;
    TnuReader* tnuReader;
    ServerResponseReader* serverResponseReader;

    std::thread *timerThread;
    std::thread *tnuThread;
    std::thread *serverResponseThread;

    std::string IPPipeName,tnuPipeName,flowPipeName;

public:
    BackEnd();
    BackEnd(std::string curPath);
    ~BackEnd();

    void run(char settingfile[]);

    void readSettings(const char* filename);
    void initializeSocket();
    void setCurPath(std::string curPath);
    void establishPipes();
    void setTimer();
    void requireIP();                           //send 100 packet to server, wait for response, and write response via IPPipe
    void getTnu();
    void createTnuThread();
    void createServerResponseThread();

    void heartbeatTimeout();
    void checkAlive();
};


#endif //INC_4OVER6_BACKEND_H
