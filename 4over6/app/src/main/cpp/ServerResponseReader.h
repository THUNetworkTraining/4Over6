//
// Created by Administrator on 2017/4/16 0016.
//

#ifndef INC_4OVER6_SERVERRESPONSEREADER_H
#define INC_4OVER6_SERVERRESPONSEREADER_H

extern  class BackEnd

class ServerResponseReader {
    int serverSocket;
    int tnu;

    void readResponse();
    void processNetResponse(char data[], int len);
    void processHeartbeat();

public:
    ServerResponseReader(int serverSocket, int tnu);
    void run();
    static void sRun(ServerResponseReader *ptr);
};


#endif //INC_4OVER6_SERVERRESPONSEREADER_H
