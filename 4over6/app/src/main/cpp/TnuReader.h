//
// Created by Administrator on 2017/4/16 0016.
//

#ifndef INC_4OVER6_TNUREADER_H
#define INC_4OVER6_TNUREADER_H

extern class BackEnd;

class TnuReader {
    int tnu;
    int serverSocket;
    int allocedAddr;

    void readTnu();

public:
    TnuReader(int tnu, int serverSocket, int allocedAddr);
    static void sRun(TnuReader* ptr);
    void run();
};


#endif //INC_4OVER6_TNUREADER_H
