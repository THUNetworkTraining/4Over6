//
// Created by Administrator on 2017/4/13 0013.
//

#include "HeartbeatTimer.h"

HeartbeatTimer::HeartbeatTimer(BackEnd *parent, std::string flowPipeName) {
    this->parent = parent;
    this->flowPipeName = flowPipeName;
    this->secCounter = 0;
}

void HeartbeatTimer::sendFlows() {
    writePipe(this->flowPipeName, &BackEnd::readFlow, sizeof(BackEnd::readFlow));
    writePipe(this->flowPipeName, &BackEnd::writeFlow, sizeof(BackEnd::writeFlow));
}

void HeartbeatTimer::mainLoop() {
    sendFlows();
    
}





