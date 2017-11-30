package com.openailab.track;

import org.opencv.android.BaseLoaderCallback;

import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;

import org.opencv.android.LoaderCallbackInterface;

import org.opencv.android.OpenCVLoader;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.core.Rect;
import org.opencv.core.Size;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.net.Uri;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.opengl.GLSurfaceView;
import android.widget.ImageView;


import com.google.android.gms.appindexing.Action;
import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.appindexing.Thing;
import com.google.android.gms.common.api.GoogleApiClient;
import com.openailab.speechrecognition.SpeechRecognitionJNI;

import java.io.IOException;


public class MainActivity extends AppCompatActivity implements CvCameraViewListener2 {

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    private static final String TAG = "Opencv::Activity";
    private Mat                    mRgba;
    private Mat                    mGray;
    private GoogleApiClient client;
    private CameraBridgeViewBase mOpenCvCameraView;
    private final PermissionsDelegate permissionsDelegate = new PermissionsDelegate(this);
    private boolean hasCameraPermission;
    private boolean hasExtSDPermission;
    private boolean hasaudioPermission;
    private int[] ret;
    public static boolean firstFrame=true;
   // GLSurfaceView mView;
    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override

        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");
                    mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
       setContentView(R.layout.activity_main);
      /*  mView = new GLSurfaceView(getApplication()){
            @Override
            public void onPause() {
                // TODO Auto-generated method stub
                super.onPause();
                Native.releaseCamera();
            }
        };
        Size size= new Size(800,480);
        mView.setRenderer(new CameraRenderer(this,size));
        setContentView(mView);*/
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.java_camera_view);
        hasCameraPermission = permissionsDelegate.hasCameraPermission();
        if(hasCameraPermission){
           mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        }
        else{
            permissionsDelegate.requestCameraPermission();
        }
        hasExtSDPermission = permissionsDelegate.hasExtSDPermission();
        if(hasExtSDPermission){
            //mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        }
        else{
            permissionsDelegate.requestExtSDPermission();
        }
        hasaudioPermission = permissionsDelegate.hasaudioPermission();
        if(hasaudioPermission){
            //mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        }
        else{
            permissionsDelegate.requestaudioPermission();
        }
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.setMaxFrameSize(320,240);
        try {
            Runtime.getRuntime().exec("export  BYPASSACL = 0x14c ");
            Runtime.getRuntime().exec(" export OPENBLAS_NUM_THREADS = 1 ");
        } catch (IOException e) {
            e.printStackTrace();
        }
        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    //public native String stringFromJNI();

    /**
     * @return
     */
    //public native String fromNewJNI();

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    public Action getIndexApiAction() {
        Thing object = new Thing.Builder()
                .setName("Main Page") // TODO: Define a title for the content shown.
                // TODO: Make sure this auto-generated URL is correct.
                .setUrl(Uri.parse("http://[ENTER-YOUR-URL-HERE]"))
                .build();
        return new Action.Builder(Action.TYPE_VIEW)
                .setObject(object)
                .setActionStatus(Action.STATUS_TYPE_COMPLETED)
                .build();
    }

    @Override
    public void onStart() {
        super.onStart();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client.connect();
        AppIndex.AppIndexApi.start(client, getIndexApiAction());
    }

    @Override
    public void onStop() {
        super.onStop();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        AppIndex.AppIndexApi.end(client, getIndexApiAction());
        client.disconnect();
    }

    @Override

    public void onPause()

    {
        super.onPause();
       // mView.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }



    @Override

    public void onResume()
    {
        super.onResume();
       // mView.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }

    }



    public void onDestroy() {
        //SpeechRecognitionJNI.ASRStop();
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }
    @Override
    public void onCameraViewStarted(int width, int height) {
        mRgba = new Mat(height, width, CvType.CV_8UC4);
        mGray = new Mat(height, width, CvType.CV_8UC1);

    }

    @Override
    public void onCameraViewStopped() {
        mRgba.release();
        mGray.release();

    }

    @Override
    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
        mRgba = inputFrame.rgba();
        mGray = inputFrame.gray();
        if(firstFrame){
            Thread thread=new Thread(new Runnable()
            {
                @Override
                public void run()
                {
                    Log.d("11111111", "111111111");
                    long startTime = System.currentTimeMillis();
                    // TODO Auto-generated method stub
                    FaceDemoInit(0.9,0.9,0.9,0.6,64);
                    FaceDemoRecognize(mRgba.getNativeObjAddr());
                    long estimatedTime = System.currentTimeMillis() - startTime;
                    Log.d("1111111111","estmated time =="+estimatedTime);
                }
            });

            thread.start();


            firstFrame=false;
        }



         return mRgba;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (permissionsDelegate.resultGranted(requestCode, permissions, grantResults)) {

            mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        }
    }

    public native void FaceDemoInit(double threshold_p, double threshold_r, double threshold_o, double factor, int mim_size);
    public native int FaceDemoRegister(int face_id, String name);
    public native String FaceDemoRecognize(long matAddrframe);
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("opencv_java3");
        System.loadLibrary("OpenCL");
        System.loadLibrary("caffe");
        System.loadLibrary("caffe_jni");
        System.loadLibrary("native-lib");
    }
}
