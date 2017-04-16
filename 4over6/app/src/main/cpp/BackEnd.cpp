//
// Created by Administrator on 2017/3/19 0019.
//

#include "BackEnd.h"
#include "msg.h"

 time_t BackEnd::readFlow = 0;
 time_t BackEnd::writeFlow = 0;
 int BackEnd::readTimes = 0;
 int BackEnd::writeTimes = 0;
time_t BackEnd::lastHeartbeatTime = 0;


void BackEnd::initializeSocket() {
    this->serverSocket = socket(PF_INET6, SOCK_STREAM, 0);
    if(this->serverSocket == -1) {
        LOGF("cannot create socket! %s ",strerror(errno));
        return;
    }
    sockaddr_in6 dest;
    dest.sin6_family = AF_INET6;
    dest.sin6_port = htons(this->serverPort);
    dest.sin6_addr = *(this->serverAddr);
    if(connect(this->serverSocket, (struct sockaddr*) &dest, sizeof(dest)) != 0) {
        LOGF("cannot connect to server! %s",strerror(errno));
        return;
    }
}

void BackEnd::readSettings(const char *filename) {
    FILE* f = fopen(filename,"r+");
    LOGD("file %s opened %d %d", filename, f, errno);
    char buffer[4096];
    fread(buffer,1,4096,f);
    Json::Reader reader;
    Json::Value root;
    if(reader.parse(buffer, root)) {
        const char* addr = root["addr"].asCString();
        inet_pton(AF_INET6, addr, this->serverAddr);
        this->serverPort = root["port"].asInt();
        LOGD("serverAddr: %s  ",addr);
        LOGD("port: %d", this->serverPort);
    }
    else {
        LOGD("cannot open %s", filename);
    }
}

BackEnd::BackEnd() {
    this->serverAddr = new in6_addr();
}

BackEnd::BackEnd(std::string curPath) {
    this->serverAddr = new in6_addr();
    this->curPath = curPath;
}

void BackEnd::setCurPath(std::string curPath) {
    this->curPath = curPath;
}

void BackEnd::requireIP() {
    msg ipmsg;
    ipmsg.type = IP_REQUEST;
    ipmsg.length = sizeof(int) + sizeof(char);
    write(this->serverSocket, (char*) &ipmsg, ipmsg.length);

    char buffer[500] = {0};
    int readSize = 0;
    while(readSize = read(this->serverSocket, buffer, 500) < 5 ) { }
    msg *ipres = (msg*) buffer;
    IPResponse *response = (IPResponse*)ipres->data;
    in_addr addr;
    addr.s_addr = response->addr;
    LOGD("IP from server: %s",inet_ntoa(addr));
    writePipe(this->IPPipeName, response, sizeof(IPResponse));
}

void BackEnd::establishPipes() {
    this->IPPipeName = this->curPath + "/" +"IPPipe";
    mknod(this->IPPipeName.c_str(),S_IFIFO | 0666, 0);

    this->tnuPipeName = this->curPath + "/" + "tnuPipe";

    this->flowPipeName = this->curPath + "/" +"flowPipe";
    mknod(this->flowPipeName.c_str(),S_IFIFO | 0666, 0);
}

void BackEnd::getTnu() {
    while(readPipe(this->tnuPipeName, &this->tnu, sizeof(int) < 4)){
        /* wait */
    }
    LOGD("tnu from frontend : %d",tnu);
}

void BackEnd::setTimer() {
    timer = new HeartbeatTimer(this, this->flowPipeName, this->serverSocket);
    timerThread = new std::thread(HeartbeatTimer::run, (void*)timer);
    time(&this->lastHeartbeatTime);
    LOGD("timer set");
}

void BackEnd::heartbeatTimeout() {

}

void BackEnd::run(char settingfile[]) {
    this->readSettings(settingfile);
    this->initializeSocket();
    this->establishPipes();
    this->setTimer();
    this->getTnu();
    this->createTnuThread();

}

void BackEnd::createTnuThread() {
    this->tnuReader = new TnuReader(this->tnu, this->serverSocket);
    this->tnuThread = new std::thread(TnuReader::sRun, this->tnuReader);
}



















