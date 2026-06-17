
#include <jni.h>
#include <sstream>
#include <string>

void Button(JNIEnv *env, jobject ctx, const char *name, int id){
    jclass BTN = env->GetObjectClass(ctx);
    jmethodID inter = env->GetMethodID(BTN,("addButton"),("(Ljava/lang/String;Lvideo/like/Ui$InterfaceBtn;)V"));
 
    jmethodID BTNS = env->GetMethodID(BTN,("BTN"),("(I)Lvideo/like/Ui$InterfaceBtn;"));

    env->CallVoidMethod(ctx, inter, env->NewStringUTF(name), env->CallObjectMethod(ctx, BTNS, id));
}

void ButtonOnce(JNIEnv *env, jobject ctx, const char *name, int id){
    jclass BTN = env->GetObjectClass(ctx);
    jmethodID inter = env->GetMethodID(BTN,("addButtonOnce"),("(Ljava/lang/String;Lvideo/like/Ui$InterfaceBtn;)V"));
 
    jmethodID BTNS = env->GetMethodID(BTN,("BTN"),("(I)Lvideo/like/Ui$InterfaceBtn;"));

    env->CallVoidMethod(ctx, inter, env->NewStringUTF(name), env->CallObjectMethod(ctx, BTNS, id));
}


void Seekbar (JNIEnv *env, jobject ctx, const char *name, int min, int max, int id){
    jclass Main = env->GetObjectClass(ctx);
    jmethodID skeed = env->GetMethodID(Main,("addSeekBar"),("(Ljava/lang/String;IILvideo/like/Ui$InterfaceInt;)V"));

    jmethodID INT = env->GetMethodID(Main,("INT"),("(I)Lvideo/like/Ui$InterfaceInt;"));

    env->CallVoidMethod(ctx, skeed, env->NewStringUTF(name),min,max, env->CallObjectMethod(ctx, INT, id));
}

void EditText(JNIEnv *env, jobject ctx, const char *name, int id) {
    jclass Main = env->GetObjectClass(ctx);
    jmethodID editTextMethod = env->GetMethodID(Main, ("addEditText"), ("(Ljava/lang/String;Lvideo/like/Ui$InterfaceStr;)V"));
    jmethodID STR = env->GetMethodID(Main, ("STR"), ("(I)Lvideo/like/Ui$InterfaceStr;"));

    env->CallVoidMethod(ctx, editTextMethod, env->NewStringUTF(name), env->CallObjectMethod(ctx, STR, id));
}

void ModeSelector(JNIEnv *env, jobject ctx, const char *name, const char **items, int itemCount, int id) {
    jclass Main = env->GetObjectClass(ctx);
    jmethodID method = env->GetMethodID(Main, ("addSelector"), ("(Ljava/lang/String;[Ljava/lang/String;Lvideo/like/Ui$InterfaceInt;)V"));
    jmethodID INT = env->GetMethodID(Main, ("INT"), ("(I)Lvideo/like/Ui$InterfaceInt;"));
    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray stringArray = env->NewObjectArray(itemCount, stringClass, env->NewStringUTF(""));
    for (int i = 0; i < itemCount; i++) {
        env->SetObjectArrayElement(stringArray, i, env->NewStringUTF(items[i]));
    }

    env->CallVoidMethod(ctx, method, env->NewStringUTF(name), stringArray, env->CallObjectMethod(ctx, INT, id));
}


void ShaderSelector(JNIEnv *env, jobject ctx, const char *name, int id) {
    jclass Main = env->GetObjectClass(ctx);
    jmethodID method = env->GetMethodID(Main, ("addShaderSelector"), ("(Ljava/lang/String;Lvideo/like/Ui$InterfaceStr;)V"));
    jmethodID STR = env->GetMethodID(Main, ("STR"), ("(I)Lvideo/like/Ui$InterfaceStr;"));

    env->CallVoidMethod(ctx, method, env->NewStringUTF(name), env->CallObjectMethod(ctx, STR, id));
}


void Category(JNIEnv *env, jobject ctx, const char *name){
    jclass Main = env->GetObjectClass(ctx);
    jmethodID AddCategory = env->GetMethodID(Main,("addCategory"),("(Ljava/lang/String;)V"));
    env->CallVoidMethod(ctx, AddCategory, env->NewStringUTF(name));
}

void Title(JNIEnv *env, jobject obj, const char* text){
    jclass textView = (*env).FindClass(("android/widget/TextView"));
    jmethodID setText = (*env).GetMethodID(textView, ("setText"), ("(Ljava/lang/CharSequence;)V"));

    jstring jstr = (*env).NewStringUTF(text);
    (*env).CallVoidMethod(obj, setText,  jstr);
}


void NewWindow(JNIEnv *env, jobject ctx, const char *name) {
    jclass Main = env->GetObjectClass(ctx);
    jmethodID method = env->GetMethodID(Main, ("beginSubWindow"), ("(Ljava/lang/String;)V"));
    env->CallVoidMethod(ctx, method, env->NewStringUTF(name));
}


void CloseNewWindow(JNIEnv *env, jobject ctx, const char *name) {
    jclass Main = env->GetObjectClass(ctx);
    jmethodID method = env->GetMethodID(Main, ("endSubWindow"), ("(Ljava/lang/String;)V"));
    env->CallVoidMethod(ctx, method, env->NewStringUTF(name));
}


void setDialog(jobject ctx, JNIEnv *env, const char *title, const char *msg){
    jclass Alert = env->FindClass(("android/app/AlertDialog$Builder"));
    jmethodID AlertCons = env->GetMethodID(Alert, ("<init>"), ("(Landroid/content/Context;)V"));

    jobject MainAlert = env->NewObject(Alert, AlertCons, ctx);

    jmethodID setTitle = env->GetMethodID(Alert, ("setTitle"), ("(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;"));
    env->CallObjectMethod(MainAlert, setTitle, env->NewStringUTF(title));

    jmethodID setMsg = env->GetMethodID(Alert, ("setMessage"), ("(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;"));
    env->CallObjectMethod(MainAlert, setMsg, env->NewStringUTF(msg));

    jmethodID setCa = env->GetMethodID(Alert, ("setCancelable"), ("(Z)Landroid/app/AlertDialog$Builder;"));
    env->CallObjectMethod(MainAlert, setCa, false);

    jmethodID setPB = env->GetMethodID(Alert, ("setPositiveButton"), ("(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;"));
    env->CallObjectMethod(MainAlert, setPB, env->NewStringUTF("Ok"), static_cast<jobject>(NULL));

    jmethodID create = env->GetMethodID(Alert, ("create"), ("()Landroid/app/AlertDialog;"));
    jobject creaetob = env->CallObjectMethod(MainAlert, create);

    jclass AlertN = env->FindClass(("android/app/AlertDialog"));

    jmethodID show = env->GetMethodID(AlertN, ("show"), ("()V"));
    env->CallVoidMethod(creaetob, show);
}

void Toast(JNIEnv *env, jobject thiz, const char *text, int length) {
    jstring jstr = env->NewStringUTF(text);
    jclass toast = env->FindClass(("android/widget/Toast"));
    jmethodID methodMakeText =env->GetStaticMethodID(toast,("makeText"),("(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;"));
    jobject toastobj = env->CallStaticObjectMethod(toast, methodMakeText,thiz, jstr, length);
    jmethodID methodShow = env->GetMethodID(toast, ("show"), ("()V"));
    env->CallVoidMethod(toastobj, methodShow);
}

void startActivityPermisson(JNIEnv *env, jobject ctx){
    jclass native_context = env->GetObjectClass(ctx);
    jmethodID startActivity = env->GetMethodID(native_context, ("startActivity"),("(Landroid/content/Intent;)V"));

    jmethodID pack = env->GetMethodID(native_context, ("getPackageName"),("()Ljava/lang/String;"));
    jstring packetname = static_cast<jstring>(env->CallObjectMethod(ctx, pack));

    const char *pkg = env->GetStringUTFChars(packetname, 0);

    std::stringstream pkgg;
    pkgg << ("package:");
    pkgg << pkg;
    std::string pakg = pkgg.str();

    jclass Uri = env->FindClass(("android/net/Uri"));
    jmethodID Parce = env->GetStaticMethodID(Uri, ("parse"), ("(Ljava/lang/String;)Landroid/net/Uri;"));
    jobject UriMethod = env->CallStaticObjectMethod(Uri, Parce, env->NewStringUTF(pakg.c_str()));

    jclass intentclass = env->FindClass(("android/content/Intent"));
    jmethodID newIntent = env->GetMethodID(intentclass, ("<init>"), ("(Ljava/lang/String;Landroid/net/Uri;)V"));
    jobject intent = env->NewObject(intentclass,newIntent,env->NewStringUTF(("android.settings.action.MANAGE_OVERLAY_PERMISSION")), UriMethod);

    env->CallVoidMethod(ctx, startActivity, intent);
}

void startInject(JNIEnv *env, jobject ctx){
    jclass native_context = env->GetObjectClass(ctx);
    jclass intentClass = env->FindClass(("android/content/Intent"));
    jclass actionString = env->FindClass(("video/like/Ui"));
    jmethodID newIntent = env->GetMethodID(intentClass, ("<init>"), ("(Landroid/content/Context;Ljava/lang/Class;)V"));
    jobject intent = env->NewObject(intentClass,newIntent,ctx,actionString);
    jmethodID startActivityMethodId = env->GetMethodID(native_context, ("startService"), ("(Landroid/content/Intent;)Landroid/content/ComponentName;"));
    env->CallObjectMethod(ctx, startActivityMethodId, intent);
}

void CheckFloatingPermison(JNIEnv *env, jobject ctx){
    jclass Settigs = env->FindClass(("android/provider/Settings"));
    jmethodID canDraw =env->GetStaticMethodID(Settigs,("canDrawOverlays"),("(Landroid/content/Context;)Z"));
    if (!env->CallStaticBooleanMethod(Settigs, canDraw, ctx)){
        Toast(env,ctx,("Overlay permission is required in order to show injector menu. Restart the cheat after you allow permission"),1);
        Toast(env,ctx,("Overlay permission is required in order to show injector menu. Restart the cheat after you allow permission"),1);
        startActivityPermisson(env, ctx);
        return;
    }
    startInject(env, ctx);
}

extern "C"
JNIEXPORT void JNICALL
Java_video_like_MainActivity_checkFloating(JNIEnv *env, jobject thiz, jobject ctx) {
    CheckFloatingPermison(env, ctx);
}
