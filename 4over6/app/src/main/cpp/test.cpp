//
// Created by Administrator on 2017/3/19 0019.
//

#include "test.h"


void settingTest(std::string curDir) {
    BackEnd backEnd;
    backEnd.setCurPath(curDir);

    char addr[] = "2402:f000:1:4417::900";
    Json::Value root;
    root["addr"] = addr;
    root["port"] = 5678;
    std::string out = root.toStyledString();
    LOGD("%s",out.c_str());
    std::string filename = "settings.json";
    filename = curDir + "/" + filename;
    LOGE("curDir: %s", curDir.c_str());
    FILE* file = fopen(filename.c_str(), "w+");
    if(!file) {
        LOGD("file %s cannot be opened  %d",filename.c_str(),errno);
        return;
    }
    fwrite(out.c_str(), out.size(), 1, file);
    fclose(file);
    LOGD("settings written");

    backEnd.readSettings(filename.c_str());
    backEnd.establishPipes();
    connectTest(backEnd);
    timerTest(backEnd);
}

void connectTest(BackEnd &backEnd) {
    backEnd.initializeSocket();
    backEnd.requireIP();
}

void timerTest(BackEnd &backEnd) {
    backEnd.setTimer();
    backEnd.checkAlive();
    LOGD("timeTestEnd");
}



