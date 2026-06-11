#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <iomanip>
#include <string.h>
#include "JavaUI.h"
#include "Obfuscate.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

extern std::string packageName;
extern std::string appUrl;
std::string architecture;

std::string ownLibPath;
std::string filePath = "/data/data/video.like/files/";

void runCommandAsRoot(const std::string& cmd) {
    std::string fullCommand = "su -c \"" + cmd + "\"";
    std::system(fullCommand.c_str());
}

void executeMemoryLib() {
    std::string executerArm = filePath + "executer";
    std::string executerX64 = filePath + "executerX64";
    runCommandAsRoot("chmod 777 " + executerArm);
    runCommandAsRoot("chmod 777 " + executerX64);
    runCommandAsRoot(executerArm);
    runCommandAsRoot(executerX64);
}


void executeLibInjector(const std::string& packageName, const std::string& soPath) {
    std::string injectorArm = filePath + "injectorLib";
    std::string injectorX64 = filePath + "injectorLibX64";  
    runCommandAsRoot("chmod 777 " + injectorArm);
    runCommandAsRoot("chmod 777 " + injectorX64);
    std::string args = " --package " + packageName + " --libs " + soPath + " --memfd"; 
    runCommandAsRoot(injectorArm + args);
    runCommandAsRoot(injectorX64 + args);
}

void injectSO(const std::string& packageName, const std::string& soName) {
    std::string fullSoPath = ownLibPath + "/" + soName;
    executeLibInjector(packageName, fullSoPath);
}


bool Command(const std::string& command) {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) return false;

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5588);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;
    setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(client_fd);
        return false;
    }
    
    send(client_fd, command.c_str(), command.length(), 0);
    close(client_fd);
    return true;
}


extern "C" JNIEXPORT jstring JNICALL
Java_video_like_MainActivity_getPackageGame(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF(packageName.c_str());
}


extern "C" JNIEXPORT jstring JNICALL
Java_video_like_MainActivity_getUrl(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(appUrl.c_str());
}



extern "C" JNIEXPORT void JNICALL
Java_video_like_MainActivity_archName(JNIEnv *env, jobject, jstring jArchitecture) {
    const char *nativeString = env->GetStringUTFChars(jArchitecture, JNI_FALSE);
    architecture = std::string(nativeString);
    env->ReleaseStringUTFChars(jArchitecture, nativeString);
}


extern "C" JNIEXPORT void JNICALL
Java_video_like_MainActivity_nativeOwnPath(JNIEnv *env, jobject, jstring jOwnLibPath) {
    const char *nativeString = env->GetStringUTFChars(jOwnLibPath, JNI_FALSE);
    ownLibPath = std::string(nativeString);
    env->ReleaseStringUTFChars(jOwnLibPath, nativeString);
}




extern "C" JNIEXPORT void JNICALL
Java_video_like_MainActivity_procDestroy(JNIEnv *env, jobject) {
    Command("STOP_SERVER");
    runCommandAsRoot("pkill -f executer");
    runCommandAsRoot("pkill -f executerX64");
    runCommandAsRoot("pkill -f injectorLib");
    runCommandAsRoot("pkill -f injectorLibX64");
    usleep(200000); 
}



void pmtSockets() {
    std::thread([]() {
        executeMemoryLib();
    }).detach();
    std::thread([]() {
        usleep(800000);
        for (int i = 0; i < 20; ++i) {
            if (Command("SET_PKG:" + packageName)) {
                break;
            }
            usleep(300000);
        }
    }).detach();
}



bool chamsSockets(const std::string& data) {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) return false;

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5577);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;
    setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(client_fd);
        return false;
    }

    send(client_fd, data.c_str(), data.length(), 0);
    close(client_fd);
    return true;
}


bool chamsWrite(const std::string& content) {
    return chamsSockets("CHAMS:" + content);
}

bool pointsWrite(int p1, int p2) {
    std::string payload = "POINTS:" + std::to_string(p1) + "," + std::to_string(p2);
    return chamsSockets(payload);
}

bool shaderWrite(const std::string& name) {
    return chamsSockets("SHADER:" + name);
}

static JavaVM* globalVm = nullptr;
static jobject globalUiObj = nullptr;

void LogReceiverThread() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return;
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(5599);
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd);
        return;
    }
    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        return;
    }
    
    char buffer[256];
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd >= 0) {
            std::memset(buffer, 0, sizeof(buffer));
            int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes > 0) {
                std::string msg(buffer, bytes);
                if (msg.rfind("LOG:", 0) == 0) {
                    std::string shader = msg.substr(4);                 
                    if (globalVm && globalUiObj) {
                        JNIEnv* env;
                        if (globalVm->AttachCurrentThread(&env, nullptr) == JNI_OK) {
                            jclass uiClass = env->GetObjectClass(globalUiObj);
                            jmethodID updateMethod = env->GetMethodID(uiClass, "updateShaderList", "(Ljava/lang/String;)V");
                            if (updateMethod) {
                                jstring jShader = env->NewStringUTF(shader.c_str());
                                env->CallVoidMethod(globalUiObj, updateMethod, jShader);
                                env->DeleteLocalRef(jShader);
                            }
                            globalVm->DetachCurrentThread();
                        }
                    }
                }
            }
            close(client_fd);
        }
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_video_like_Ui_StringCases(JNIEnv *env, jobject thiz, jint id, jstring text) {
    if (text != nullptr) {
        const char *nativeString = env->GetStringUTFChars(text, JNI_FALSE);
        if (nativeString != nullptr) {
            std::string convertedText(nativeString);
            env->ReleaseStringUTFChars(text, nativeString);
            shaderWrite(convertedText);
        }
    }
}

