//
// Created by Administrator on 2017/4/13 0013.
//

#ifndef INC_4OVER6_HEARTBEATTIMER_H
#define INC_4OVER6_HEARTBEATTIMER_H

#include "BackEnd.h"
#include "msg.h"

class HeartbeatTimer {
    BackEnd* parent;

    std::string flowPipeName;
    int secCounter;

    HeartbeatTimer(BackEnd* parent, std::string flowPipeName);
    void sendFlows();
    void mainLoop();
};


#endif //INC_4OVER6_HEARTBEATTIMER_H
