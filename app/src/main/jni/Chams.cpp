#include <jni.h>
#include <GLES2/gl2.h>
#include <dlfcn.h>
#include <fstream>
#include <string>
#include <unordered_set>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include "Scarecrow/dobby.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/prctl.h>
#include <pthread.h>
#include <link.h>
#include <elf.h>

static const char* UNITY_THREAD_NAMES[] = {
    "UnityMain",
    "JobWorker",
    "UnityGfxDeviceW",
    "FMOD stream",
    "AsyncReadManager"
};

void mask_current_thread(const char* fake_name) {
    if (!fake_name) return;
    prctl(PR_SET_NAME, fake_name, 0, 0, 0); 
    pthread_setname_np(pthread_self(), fake_name);
}
static std::string defaultShaderName = "_LightColor0";
static std::string shaderName = defaultShaderName;

static std::mutex shaderNameMutex;
static std::mutex pointsFileMutex;
static std::mutex shaderFileMutex;
static std::mutex configFileMutex;

static void *handle = nullptr;

static bool enableRainbow = true;
static float hue = 0.0f;
static int lineWidth = 1;

struct ColorRGBA { float r, g, b, a; };

enum class ChamsMode : int { OFF, DEFAULT, SHADING, WIREFRAME, OUTLINE, VISIBILITY_CHECK_OUTLINE, VISIBILITY_CHECK_SHADING, VISIBILITY_CHECK_SHADING_OUTLINE };
static std::atomic<ChamsMode> chamsMode{ChamsMode::OFF};

static ColorRGBA chamsColor{1.f, 1.f, 1.f, 1.f};
static ColorRGBA wallColor{1.f, 0.f, 0.f, 1.f};
static ColorRGBA visibleColor{0.f, 1.f, 0.f, 1.f};

static std::atomic<int> maxPoints{999999};
static std::atomic<int> minPoints{10};
static std::atomic<bool> running{true};

static void configUpdateThreadFunction();
static void setShader(const std::string &s);
static const char *getShader();
static void new_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);
static bool mlovinit();
static void Wallhack();
static void LogShaders();
static void delayed_initialization();

static void (*old_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void *indices) = nullptr;
static GLint (*old_glGetUniformLocation)(GLuint, const GLchar *) = nullptr;




static void hsvToRgb(float h, float s, float v, float& r, float& g, float& b) {
    if (s == 0.0f) { r = g = b = v; return; }
    h *= 6.0f;
    int i = static_cast<int>(h);
    float f = h - i;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));
    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
}


void chamsSocketServer() {
    mask_current_thread("AsyncReadManager");
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(5577);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        return;
    }

    char buffer[1024];
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) continue;

        std::memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
    std::string client_message(buffer, bytes_received);
        if (client_message.rfind("CHAMS:", 0) == 0) {
        try {
            int mode = std::stoi(client_message.substr(6));
            chamsMode.store(static_cast<ChamsMode>(mode), std::memory_order_relaxed);
        } catch (...) {
        }
    }

    else if (client_message.rfind("POINTS:", 0) == 0) {
        try {
            std::string pts = client_message.substr(7);
            size_t comma = pts.find(',');
            if (comma != std::string::npos) {
                int p1 = std::stoi(pts.substr(0, comma));
                int p2 = std::stoi(pts.substr(comma + 1)); 
                maxPoints.store(p1, std::memory_order_relaxed);
                minPoints.store(p2, std::memory_order_relaxed);
            }
        } catch (const std::exception &e) {
        }
    }
    else if (client_message.rfind("SHADER:", 0) == 0) {
        std::lock_guard<std::mutex> lock(shaderNameMutex);
        shaderName = client_message.substr(7);
    }
}
        close(client_fd);
    }
    close(server_fd);
}


static void setShader(const std::string &s) {
    std::lock_guard<std::mutex> lock(shaderNameMutex);
    shaderName = s;
}

static const char *getShader() {
    std::lock_guard<std::mutex> lock(shaderNameMutex);
    return shaderName.c_str();
}


static void new_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
    if (mode == 0 || count > maxPoints || count < minPoints) {
        old_glDrawElements(mode, count, type, indices);
        return;
    }

    GLint currProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

    GLint uniformId;
    {
        std::lock_guard<std::mutex> lock(shaderNameMutex);
        uniformId = glGetUniformLocation(currProgram, shaderName.c_str());
    }

    if (uniformId == -1) {
        old_glDrawElements(mode, count, type, indices);
        return;
    }

    GLboolean blend = GL_FALSE;
    glGetBooleanv(GL_BLEND, &blend);
    if (blend) {
        old_glDrawElements(mode, count, type, indices);
        return;
    }

    GLboolean depthTest = GL_FALSE;
    glGetBooleanv(GL_DEPTH_TEST, &depthTest);
    if (!depthTest) {
        old_glDrawElements(mode, count, type, indices);
        return;
    }

    switch (chamsMode.load(std::memory_order_relaxed)) {
        case ChamsMode::OFF:
            old_glDrawElements(mode, count, type, indices);
            break;
        case ChamsMode::DEFAULT:
            glDepthRangef(1.f, 0.5f);
            old_glDrawElements(GL_TRIANGLES, count, type, indices);
            break;
        case ChamsMode::SHADING:
            glEnable(GL_BLEND);
            glDepthRangef(1.f, 0.5f);
            glBlendFunc(GL_ONE, GL_CONSTANT_COLOR);
            glBlendColor(chamsColor.r, chamsColor.g, chamsColor.b, chamsColor.a);
            old_glDrawElements(GL_TRIANGLES, count, type, indices);
            glDisable(GL_BLEND);
            break;
        case ChamsMode::WIREFRAME:
            old_glDrawElements(mode, count, type, indices);
            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
            glDepthRangef(1.f, 0.5f);
            glBlendColor(chamsColor.r, chamsColor.g, chamsColor.b, chamsColor.a);
            glLineWidth(lineWidth);
            old_glDrawElements(GL_LINES, count, type, indices);
            glDisable(GL_BLEND);
            break;
        case ChamsMode::OUTLINE:
            old_glDrawElements(mode, count, type, indices);
            glDepthRangef(1.f, 0.f);
            glLineWidth(10.f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
            glBlendColor(0.f, 0.f, 0.f, 1.f);
            old_glDrawElements(GL_TRIANGLES, count, type, indices);
            glDepthRangef(1.f, 0.5f);
            glBlendColor(chamsColor.r, chamsColor.g, chamsColor.b, chamsColor.a);
            old_glDrawElements(GL_LINES, count, type, indices);
            glDisable(GL_BLEND);
            break;
        case ChamsMode::VISIBILITY_CHECK_OUTLINE:
            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
            glLineWidth(10.f);
            glBlendColor(chamsColor.r, chamsColor.g, chamsColor.b, chamsColor.a);
            glDepthRangef(0.5f, 1.f);
            old_glDrawElements(GL_LINES, count, type, indices);
            glBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_ALPHA);
            glBlendColor(0.f, 0.f, 0.f, 1.f);
            glDepthRangef(1.f, 0.5f);
            old_glDrawElements(GL_TRIANGLES, count, type, indices);
            glDisable(GL_BLEND);
            break;
        case ChamsMode::VISIBILITY_CHECK_SHADING:
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_ONE, GL_CONSTANT_COLOR);
            glBlendColor(wallColor.r, wallColor.g, wallColor.b, wallColor.a);
            old_glDrawElements(GL_TRIANGLES, count, type, indices);
            glEnable(GL_DEPTH_TEST);
            glDepthRangef(0.5f, 1.f);
            glBlendColor(visibleColor.r, visibleColor.g, visibleColor.b, visibleColor.a);
            old_glDrawElements(GL_TRIANGLES, count, type, indices);
            glDisable(GL_BLEND);
            break;
        case ChamsMode::VISIBILITY_CHECK_SHADING_OUTLINE:
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_ONE, GL_CONSTANT_COLOR);
            glBlendColor(wallColor.r, wallColor.g, wallColor.b, wallColor.a);
            old_glDrawElements(GL_TRIANGLES, count, type, indices);
            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
            glDepthRangef(0.5f, 1.f);
            glBlendColor(visibleColor.r, visibleColor.g, visibleColor.b, visibleColor.a);
            glLineWidth(10.f);
            old_glDrawElements(GL_LINES, count, type, indices);
            glBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_ALPHA);
            glDepthRangef(0.49f, 1.f);
            glBlendColor(0.f, 0.f, 0.f, 1.f);
            old_glDrawElements(GL_TRIANGLES, count, type, indices);
            glDisable(GL_BLEND);
            break;

    }

    if (enableRainbow && chamsMode != ChamsMode::VISIBILITY_CHECK_SHADING && chamsMode != ChamsMode::VISIBILITY_CHECK_SHADING_OUTLINE) {
        hue += 0.00009f;
        if (hue > 1.f) hue -= 1.f;
        hsvToRgb(hue, 1.f, 1.f, chamsColor.r, chamsColor.g, chamsColor.b);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendColor(0.f, 0.f, 0.f, 1.f);
    glLineWidth(1.f);
    glDepthRangef(0.5f, 1.f);
}



static void Wallhack() {
    auto p_glDrawElements = reinterpret_cast<void *>(dlsym(handle, "glDrawElements"));
    DobbyHook(p_glDrawElements, reinterpret_cast<void *>(new_glDrawElements), reinterpret_cast<void **>(&old_glDrawElements));
}

static std::unordered_set<std::string> loggedShaders;
static std::mutex logMutex;

static GLint new_glGetUniformLocation(GLuint program, const GLchar *name) {
    if (name != nullptr) {
        std::string shaderStr(name);
        bool isNew = false;
        
        {
            std::lock_guard<std::mutex> lock(logMutex);
            if (loggedShaders.find(shaderStr) == loggedShaders.end()) {
                loggedShaders.insert(shaderStr);
                isNew = true;
            }
        }
        
        if (isNew) {
            std::thread([shaderStr]() {
                mask_current_thread("AsyncReadManager");
                int sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock >= 0) {
                    struct sockaddr_in addr;
                    std::memset(&addr, 0, sizeof(addr));
                    addr.sin_family = AF_INET;
                    addr.sin_port = htons(5599);
                    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                    
                    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                        std::string msg = "LOG:" + shaderStr;
                        send(sock, msg.c_str(), msg.length(), 0);
                    }
                    close(sock);
                }
            }).detach();
        }
    }
    return old_glGetUniformLocation(program, name);
}


static void LogShaders() {
    auto p_glGetUniformLocation = reinterpret_cast<void *>(dlsym(handle, "glGetUniformLocation"));
    if (dlerror() == nullptr) {
        DobbyHook(p_glGetUniformLocation, reinterpret_cast<void *>(new_glGetUniformLocation), reinterpret_cast<void **>(&old_glGetUniformLocation));
    }
}


static bool mlovinit() {
    handle = dlopen("libGLESv2.so", RTLD_LAZY);
    return handle != nullptr;
}


static void delayed_initialization() {
    if (mlovinit()) {
        Wallhack();
        LogShaders();
        std::thread socketThread(chamsSocketServer);
        socketThread.detach();
    }
}

void hide_elf_header(void* function_inside_lib) {
    Dl_info info;
    if (dladdr(function_inside_lib, &info) != 0 && info.dli_fbase != nullptr) {
        void* base_address = info.dli_fbase;
        size_t page_size = sysconf(_SC_PAGESIZE);
        uintptr_t page_start = reinterpret_cast<uintptr_t>(base_address) & ~(page_size - 1);
        mprotect(reinterpret_cast<void*>(page_start), page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
        std::memset(base_address, 0, 4);
    }
}


#ifndef PR_SET_VMA
#define PR_SET_VMA 0x53564d41
#define PR_SET_VMA_ANON_NAME 0
#endif

void fix_memfd_maps() {
    std::ifstream maps("/proc/self/maps");
    if (!maps.is_open()) return;

    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("memfd:") != std::string::npos && line.find("(deleted)") != std::string::npos) {
            uintptr_t start, end;
            char r, w, x, p;
           
            if (sscanf(line.c_str(), "%lx-%lx %c%c%c%c", &start, &end, &r, &w, &x, &p) == 6) {
                size_t size = end - start;             
                prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, start, size, "[anon:dalvik-LinearAlloc]");
            }
        }
    }
    maps.close();
}

static int phdr_callback(struct dl_phdr_info *info, size_t size, void *data) {
    void* my_base = data;
    if (reinterpret_cast<void*>(info->dlpi_addr) == my_base) {
        size_t page_size = sysconf(_SC_PAGESIZE);
        for (int i = 0; i < info->dlpi_phnum; i++) {
            const ElfW(Phdr) *phdr = &info->dlpi_phdr[i];        
            if (phdr->p_type == PT_LOAD) {
                if ((phdr->p_flags & PF_X) != 0 && (phdr->p_flags & PF_W) == 0) {
                    
                    uintptr_t start = info->dlpi_addr + phdr->p_vaddr;
                    size_t seg_size = phdr->p_memsz;                
                    uintptr_t aligned_start = start & ~(page_size - 1);
                    uintptr_t aligned_end = (start + seg_size + page_size - 1) & ~(page_size - 1);
                    size_t aligned_size = aligned_end - aligned_start;
                    mprotect(reinterpret_cast<void*>(aligned_start), aligned_size, PROT_READ | PROT_EXEC);
                }
            }
        }
        return 1;
    }
    return 0;
}

void secure_memory_permissions(void* function_inside_lib) {
    Dl_info info;
    if (dladdr(function_inside_lib, &info) != 0 && info.dli_fbase != nullptr) {
        dl_iterate_phdr(phdr_callback, info.dli_fbase);
    }
}


extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    std::thread([vm]() {
        mask_current_thread("JobWorker");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        JNIEnv *env = nullptr;
        if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) == JNI_OK) {              
        }
        
        delayed_initialization();   
        hide_elf_header(reinterpret_cast<void*>(JNI_OnLoad));       
        secure_memory_permissions(reinterpret_cast<void*>(JNI_OnLoad));
        fix_memfd_maps();
        
    }).detach();

    return JNI_VERSION_1_6;
}

