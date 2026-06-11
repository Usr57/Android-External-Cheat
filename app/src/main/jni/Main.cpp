#include "Scarecrow/Executable.h"

std::string packageName = "com.mayoninjagames.zombiecubes2";
std::string appUrl = "https://t.me/User5577akaVlad";

bool editValues, editHex, editParameters, hookMethod, hookFields = false;
int points1, points2 = 10;


extern "C"
JNIEXPORT void JNICALL
Java_video_like_Ui_Views(JNIEnv *env, jobject thiz, jobject ctx, jobject title, jobject subtitle) {
    env->GetJavaVM(&globalVm); if (globalUiObj == nullptr) { globalUiObj = env->NewGlobalRef(ctx);  std::thread(LogReceiverThread).detach();}
    pmtSockets();
	
    Title(env, title, ("By User5577"));

    Category(env, ctx, ("Full external:"));
    Button(env, ctx, ("Values"), 1);
    Button(env, ctx, ("Hex patch"), 2);
    Button(env, ctx, ("Hook method"), 3);
    Button(env, ctx, ("Hook field"), 4);
    Category(env, ctx, ("Chams Menu:"));
    ButtonOnce(env, ctx, ("Inject chams lib"), 5);
    const char* chamsModes[] = {
        "Off",
        "Default",
        "Shading",
        "Wireframe",
        "Outline",
        "Vis Check Outline",
        "Vis Check Shading",
        "Vis Check Shading Outline"
    };
    ModeSelector(env, ctx, "Chams Mode", chamsModes, 8, 6);
    Seekbar(env, ctx, ("Max Points"), 10, 100000, 7);
    Seekbar(env, ctx, ("Min Points"), 10, 100000, 8);
    ShaderSelector(env, ctx, ("Shader name"), 9); 
}




extern "C"
JNIEXPORT void JNICALL
Java_video_like_Ui_Cases(JNIEnv *env, jobject thiz, jint feature, jint value, jobject ctx) {
    std::thread([feature, value]() {
        switch (feature) {
        case 1:
            editValues = !editValues;	
            if (editValues) 
				Command("ExampleOn");
            else 
				Command("ExampleOff");
            break;
			
        case 2:
            editHex = !editHex;	
            if (editHex) 
				Command("HexPatchOn"); 
            else 
				Command("HexPatchOff"); 
            break;
			
        
			
        case 3:
            hookMethod = !hookMethod;
            if (hookMethod) 
				Command("HookMethodOn"); 
            else 
				Command("HookMethodOff"); 
            break;
			
        case 4:
            hookFields = !hookFields;
            if (hookFields) 
				Command("HookFieldOn");
            else 
				Command("HookFieldOff");
            break;
			
        case 5:
            injectSO(packageName, "libmain.so"); 
            break;
			
        case 6:
            chamsWrite(std::to_string(value) + "\n");
            break;
			
        case 7:
            points1 = value;
            pointsWrite(points1, points2);
            break;
			
        case 8:
            points2 = value;
            pointsWrite(points1, points2);
            break;
			
        case 9:
            break;
        }
    }).detach();
}

