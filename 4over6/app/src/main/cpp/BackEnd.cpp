//
// Created by Administrator on 2017/3/19 0019.
//

#include "BackEnd.h"
#include "msg.h"

 long long BackEnd::readFlow = 0;
 long long BackEnd::writeFlow = 0;
 long long BackEnd::readTimes = 0;
 long long BackEnd::writeTimes = 0;
time_t BackEnd::lastHeartbeatTime = 0;


bool BackEnd::initializeSocket() {
    this->serverSocket = socket(PF_INET6, SOCK_STREAM, 0);
    if(this->serverSocket == -1) {
        LOGF("cannot create socket! %s ",strerror(errno));
        return false;
    }
    sockaddr_in6 dest;
    dest.sin6_family = AF_INET6;
    dest.sin6_port = htons(this->serverPort);
    dest.sin6_addr = *(this->serverAddr);
    for(int i = 0; i < CONNECT_RETRY_TIME; i++){
        if(connect(this->serverSocket, (struct sockaddr*) &dest, sizeof(dest)) != 0) {
            LOGF("cannot connect to server! %s",strerror(errno));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
            return true;     // connect success
    }
    return false; // connect failed
}

void BackEnd::readSettings(const char *filename) {
    filename = (curPath + "/" + std::string(filename)).c_str();
    FILE* f = fopen(filename,"r+");
    LOGD("file %s opened %d %d %s", filename, f, errno, strerror(errno));
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
    this->tnu = 0;

    this->timerThread = 0;
    this->tnuReader = 0;
    this->tnuThread = 0;
    this->serverResponseReader = 0;
    this->serverResponseThread = 0;
    this->timer = 0;
}

BackEnd::BackEnd(std::string curPath) {
    this->serverAddr = new in6_addr();
    this->curPath = curPath;
    this->tnu = 0;

    this->timerThread = 0;
    this->tnuReader = 0;
    this->tnuThread = 0;
    this->serverResponseReader = 0;
    this->serverResponseThread = 0;
    this->timer = 0;
}

void BackEnd::setCurPath(std::string curPath) {
    this->curPath = curPath;
}

void BackEnd::requireIP() {
    msg ipmsg;
    ipmsg.type = IP_REQUEST;
    ipmsg.length = sizeof(int) + sizeof(char);
    write(this->serverSocket, (char*) &ipmsg, ipmsg.length);

    LOGD("requiring IP");
    char buffer[500] = {0};
    int readSize = 0;
    while((readSize = read(this->serverSocket, buffer, 500)) < 5 ) { }
    msg *ipres = (msg*) buffer;
    char data[1000];
    memset(data, 0, 1000);
    LOGD("readSize %d",readSize);
    memcpy(data,ipres->data,readSize-5);
    LOGD("IP strings %s",data);
    data[readSize-5] = '\0';
    int prevPtr = 0;
    int nextPtr = 0;
    int intAddrs[5];
    for(int i = 0; i < 5; i++) {        //split 5 ip strings according to " " or "\0"
        char addr[50];
        in_addr addrStruct;
        memset(addr, 0, 50);
        while(data[nextPtr] != ' ' && data[nextPtr] != '\0')        //find next " " or "\0"
            nextPtr++;
        strncpy(addr, data+prevPtr, nextPtr-prevPtr+1);               //copy substring between prevPtr and nextPtr
        addr[nextPtr-prevPtr] = '\0';
        for(int i = 0; i < 50; i++)
            if(addr[i] == '\n')
                addr[i] = '\0';
        prevPtr = ++nextPtr;
        if(inet_aton(addr,&addrStruct) == 0)
            LOGD("wrong addr transform %s!",addr);
        intAddrs[i] = addrStruct.s_addr;
    }
    IPResponse response;
    response.addr = intAddrs[0];
    response.route = intAddrs[1];
    response.DNS[0] = intAddrs[2];
    response.DNS[1] = intAddrs[3];
    response.DNS[2] = intAddrs[4];
    allocedAddr.s_addr = response.addr;
    in_addr addr;
    allocedAddr.s_addr = response.addr;
    addr.s_addr = response.addr;
    LOGD("IP from server: %s",inet_ntoa(addr));
    addr.s_addr = response.route;
    LOGD("route from server: %s",inet_ntoa(addr));
    addr.s_addr = response.DNS[0];
    LOGD("DNS1 from server: %s",inet_ntoa(addr));
    addr.s_addr = response.DNS[1];
    LOGD("DNS2 from server: %s",inet_ntoa(addr));
    addr.s_addr = response.DNS[2];
    LOGD("DNS3 from server: %s",inet_ntoa(addr));
    writePipe(this->IPPipeName, &response, sizeof(IPResponse));
}

void BackEnd::establishPipes() {
    this->IPPipeName = this->curPath + "/" +"IPPipe";
    mknod(this->IPPipeName.c_str(),S_IFIFO | 0666, 0);

    this->tnuPipeName = this->curPath + "/" + "tnuPipe";

    this->flowPipeName = this->curPath + "/" +"flowPipe";
    mknod(this->flowPipeName.c_str(),S_IFIFO | 0666, 0);
}

void BackEnd::getTnu() {
    LOGD("waiting for tnu from frontend");
    int readLen = 0;
    while((readLen = readPipe(this->tnuPipeName, &(this->tnu), sizeof(int))) < 4){
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
    LOGE("timeout exiting....");
    this->~BackEnd();
}

void BackEnd::run(char settingfile[]) {
    this->readSettings(settingfile);
    if(!this->initializeSocket())
        return;
    this->establishPipes();
    this->requireIP();
    this->setTimer();
    this->getTnu();
    this->createTnuThread();
    this->createServerResponseThread();

    this->checkAlive();

}

void BackEnd::createTnuThread() {
    this->tnuReader = new TnuReader(this->tnu, this->serverSocket, this->allocedAddr.s_addr);
    this->tnuThread = new std::thread(TnuReader::sRun, this->tnuReader);
}

void BackEnd::createServerResponseThread() {
    this->serverResponseReader = new ServerResponseReader(this->serverSocket, this->tnu);
    this->serverResponseThread = new std::thread(ServerResponseReader::sRun, this->serverResponseReader);
}

void BackEnd::checkAlive() {
    this->timerThread->join();
}

BackEnd::~BackEnd() {
}

























