#include <jni.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <android/log.h>
#include <pthread.h>
#include <time.h>
#include <Math.h>
#include <opencv/cv.h>


#include <android/log.h>
#include "face_demo.hpp"
#define  LOG_TAG    "JNI_PART"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG, __VA_ARGS__)




using namespace cv;
using namespace std;
//extern int init_tracker();

extern "C" {

Rect res;
FaceDemo mFace_demo;


void DrawText(cv::Mat &img, std::string text, int x, int y, cv::Scalar color) {
    cv::putText(img, text.c_str(), cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, 0.8, color, 2, 1);
}

JNIEXPORT void JNICALL
Java_com_openailab_track_MainActivity_FaceDemoInit(JNIEnv *env, jobject instance,
                                                   jdouble threshold_p, jdouble threshold_r,
                                                   jdouble threshold_o, jdouble factor,
                                                   jint mim_size) {
   int ret = mFace_demo.Init(threshold_p, threshold_r, threshold_o, factor, mim_size);
   
   LOGD("[%s] mFace_demo.Init %d.\n", __FUNCTION__, ret);
    // TODO

}

JNIEXPORT jint JNICALL
Java_com_openailab_track_MainActivity_FaceDemoRegister(JNIEnv *env, jobject instance, jint face_id,
                                                       jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);

    // TODO

    env->ReleaseStringUTFChars(name_, name);
}

JNIEXPORT jstring JNICALL
Java_com_openailab_track_MainActivity_FaceDemoRecognize(JNIEnv *env, jobject instance,
                                                        jlong matAddrframe) {

    // TODO
      Mat &mRgb = *(Mat *) matAddrframe;
      cvtColor(mRgb,mRgb,COLOR_RGBA2BGR);//opencv default work on BGR order without alpha
      flip(mRgb,mRgb,1);//miro mode
     std::string ss = mFace_demo.Recognize(mRgb);
	 LOGD("[%s] FaceDemo Recognize %s\n", __FUNCTION__, ss.c_str());
     cvtColor(mRgb,mRgb,COLOR_BGR2RGBA);//change back to android mode
    return env->NewStringUTF("string");
}

}