//
// Created by Administrator on 2017/4/13 0013.
//

#include "HeartbeatTimer.h"
#include "BackEnd.h"

HeartbeatTimer::HeartbeatTimer(BackEnd *parent, std::string flowPipeName, int serverSocket) {
    this->parent = parent;
    this->flowPipeName = flowPipeName;
    this->secCounter = 0;
    this->serverSocket = serverSocket;
}

void HeartbeatTimer::sendFlows() {
    long long buffer[4];
    buffer[0] = parent->readFlow;
    buffer[1] = parent->writeFlow;
    buffer[2] = parent->readTimes;
    buffer[3] = parent->writeTimes;
    writePipe(this->flowPipeName, &buffer, sizeof(parent->readFlow)*4);
    //LOGE("flows %lld %lld %lld %lld",buffer[0],buffer[1],buffer[2],buffer[3]);
}

void HeartbeatTimer::mainLoop() {
    while(1){
        sendFlows();
        time_t t;
        time(&t);
        time_t heartbeatInterval = t - parent->lastHeartbeatTime;
        //LOGD("timer counts %d  interval  %d from %d",this->secCounter, heartbeatInterval, std::this_thread::get_id());
        if(heartbeatInterval > 60){
            LOGD("Heartbeat timeout : %d!",heartbeatInterval);
            return;
        }
        else{
            if(this->secCounter % 20 == 0){
                //LOGD("sending heartbeat");
                sendHeartbeat();
            }
        }
        this->secCounter ++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void HeartbeatTimer::sendHeartbeat() {
    msg ipmsg;
    ipmsg.type = IP_HEARTBEAT;
    ipmsg.length = sizeof(int) + sizeof(char);
    write(this->serverSocket, (char*) &ipmsg, ipmsg.length);
}

HeartbeatTimer::HeartbeatTimer() {

}

void HeartbeatTimer::run(void *timerPtr) {
    HeartbeatTimer* ptr = (HeartbeatTimer*) timerPtr;
    ptr->mainLoop();
    LOGD("timer exiting");
}











