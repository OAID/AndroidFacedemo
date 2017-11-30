package com.openailab.speechrecognition;

/**
 * Created by Lenovo on 2017/11/8.
 */

public class SpeechRecognitionJNI {


    public SpeechRecognitionJNI() {}

    public static void ASRCallback(String str){

        System.out.println("ASR result "+str);
    }

    public static native int ASRInit(int card, int device, int channels, int rate, int vad_threshold, double mic_gain);

    public static native void ASRStart();

    public static native void ASRStop();

}
