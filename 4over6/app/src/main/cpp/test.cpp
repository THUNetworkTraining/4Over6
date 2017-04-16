//
// Created by Administrator on 2017/3/19 0019.
//

#include "test.h"


void settingTest(std::string curDir) {
    BackEnd backEnd;
    backEnd.setCurPath(curDir);

    char addr[] = "::1";
    Json::Value root;
    root["addr"] = addr;
    root["port"] = 1530;
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
    //connectTest(backEnd);
    backEnd.establishPipes();
    timerTest(backEnd);
}

void connectTest(BackEnd &backEnd) {
    backEnd.initializeSocket();
}

void timerTest(BackEnd &backEnd) {
    backEnd.setTimer();
    LOGD("timeTestEnd");
}



