//
// Created by Olmo on 10/06/2018.
//

#include "../../../../../Project/include/game.h"
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_olmoprieto_pong_1dx_GameLibJNIWrapper_onSurfaceCreated
        (JNIEnv *env, jclass clss, jint width, jint height) {

    Game::setRenderSize(width, height);
    Game::Instance();
}

JNIEXPORT void JNICALL Java_olmoprieto_pong_1dx_GameLibJNIWrapper_onSurfaceChanged
        (JNIEnv *env, jclass clss, jint width, jint height) {

}

JNIEXPORT void JNICALL Java_olmoprieto_pong_1dx_GameLibJNIWrapper_onDrawFrame
        (JNIEnv *env, jclass clss) {

    Game::Instance()->draw();
}

JNIEXPORT void JNICALL Java_olmoprieto_pong_1dx_GameLibJNIWrapper_onTouchEvent
        (JNIEnv *env, jclass clss, jfloat x, jfloat y) {

    //Game::Instance()->registerEvent(x, y);
}

#ifdef __cplusplus
};
#endif