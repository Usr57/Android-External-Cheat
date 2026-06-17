#include "Scarecrow/Executable.h"

std::string packageName = "com.mayoninjagames.zombiecubes2";
std::string appUrl = "https://t.me/User5577akaVlad";

bool editValues, editValuesFreeze, editHex, editParameters, hookMethod, hookFields = false;
int points1, points2 = 10;


extern "C"
JNIEXPORT void JNICALL
Java_video_like_Ui_Views(JNIEnv *env, jobject thiz, jobject ctx, jobject title, jobject subtitle) {
    env->GetJavaVM(&globalVm); if (globalUiObj == nullptr) { globalUiObj = env->NewGlobalRef(ctx);  std::thread(LogReceiverThread).detach();}
    pmtSockets();
	
    Title(env, title, ("By User5577"));

    Category(env, ctx, ("Full external:"));
	
	NewWindow(env, ctx, ("Values"));
    Button(env, ctx, ("Edit values"), 1);
	Button(env, ctx, ("Freeze values"), 2);
	ButtonOnce(env, ctx, ("Add values"), 3);
	CloseNewWindow(env, ctx, ("Values"));
	
    Button(env, ctx, ("Hex patch"), 4);
    Button(env, ctx, ("Hook method"), 5);
    Button(env, ctx, ("Hook field"), 6);
    Category(env, ctx, ("Chams Menu:"));
    ButtonOnce(env, ctx, ("Inject chams lib"), 7);
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
    ModeSelector(env, ctx, "Chams Mode", chamsModes, 8, 8);
    Seekbar(env, ctx, ("Max Points"), 10, 100000, 9);
    Seekbar(env, ctx, ("Min Points"), 10, 100000, 10);
    ShaderSelector(env, ctx, ("Shader name"), 11); 
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
            editValuesFreeze = !editValuesFreeze;	
            if (editValuesFreeze) 
				Command("ExampleFreeze");
            else 
				Command("ExampleFreezeOff");
            break;
			
		case 3:          
				Command("ExampleAdd");
            break;
			
        case 4:
            editHex = !editHex;	
            if (editHex) 
				Command("HexPatchOn"); 
            else 
				Command("HexPatchOff"); 
            break;
			
        		
        case 5:
            hookMethod = !hookMethod;
            if (hookMethod) 
				Command("HookMethodOn"); 
            else 
				Command("HookMethodOff"); 
            break;
			
        case 6:
            hookFields = !hookFields;
            if (hookFields) 
				Command("HookFieldOn");
            else 
				Command("HookFieldOff");
            break;
			
        case 7:
            injectSO(packageName, "libmain.so"); 
            break;
			
        case 8:
            chamsWrite(std::to_string(value) + "\n");
            break;
			
        case 9:
            points1 = value;
            pointsWrite(points1, points2);
            break;
			
        case 10:
            points2 = value;
            pointsWrite(points1, points2);
            break;
			
        case 11:
            break;
        }
    }).detach();
}

