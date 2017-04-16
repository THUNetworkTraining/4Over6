#include <jni.h>
#include <string>
#include "BackEnd.h"
#include "CLog.h"
#include "json/json.h"
#include "test.h"

extern "C"
jstring
Java_thu409b_a4over6_MainActivity_runBackEnd(
        JNIEnv *env,
        jobject /* this */, jstring curDir) {
    std::string hello = "time to write a backEnd";
    const char* dir = (*env).GetStringUTFChars(curDir,0);
    //settingTest(dir);
    return env->NewStringUTF(hello.c_str());
}
