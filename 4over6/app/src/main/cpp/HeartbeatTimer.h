//
// Created by Administrator on 2017/4/13 0013.
//

#ifndef INC_4OVER6_HEARTBEATTIMER_H
#define INC_4OVER6_HEARTBEATTIMER_H


#include "msg.h"
#include <time.h>
#include "CLog.h"

extern class BackEnd;

class HeartbeatTimer{
    BackEnd* parent;
    int serverSocket;
    std::string flowPipeName;

    int secCounter;

public:
    HeartbeatTimer();
    HeartbeatTimer(BackEnd* parent, std::string flowPipeName, int serverSocket);
    void mainLoop();
    static void run(void* timerPtr);
private:
    void sendFlows();
    void sendHeartbeat();
};


#endif //INC_4OVER6_HEARTBEATTIMER_H
