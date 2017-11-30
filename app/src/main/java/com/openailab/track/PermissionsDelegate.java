package com.openailab.track;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.widget.Toast;

class PermissionsDelegate {

    private static final int REQUEST_CODE = 10;
    private final Activity activity;

    PermissionsDelegate(Activity activity) {
        this.activity = activity;
    }

    boolean hasCameraPermission() {
        int permissionCheckResult = ContextCompat.checkSelfPermission(
                activity,
                Manifest.permission.CAMERA
        );
        return permissionCheckResult == PackageManager.PERMISSION_GRANTED;
    }

    void requestCameraPermission() {
        ActivityCompat.requestPermissions(
                activity,
                new String[]{Manifest.permission.CAMERA},
                REQUEST_CODE
        );
    }
    boolean hasExtSDPermission() {
        int permissionCheckResult = ContextCompat.checkSelfPermission(
                activity,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
        );
        return permissionCheckResult == PackageManager.PERMISSION_GRANTED;
    }
    void requestExtSDPermission(){
        ActivityCompat.requestPermissions(
                activity,
                new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                REQUEST_CODE
        );
    }
    boolean hasaudioPermission() {
        int permissionCheckResult = ContextCompat.checkSelfPermission(
                activity,
                Manifest.permission.RECORD_AUDIO
        );
        return permissionCheckResult == PackageManager.PERMISSION_GRANTED;
    }
    void requestaudioPermission(){
        ActivityCompat.requestPermissions(
                activity,
                new String[]{Manifest.permission.RECORD_AUDIO},
                REQUEST_CODE
        );
    }
    boolean resultGranted(int requestCode,
                          String[] permissions,
                          int[] grantResults) {

        if (requestCode != REQUEST_CODE) {
            return false;
        }

        if (grantResults.length < 1) {
            return false;
        }
        if (!(permissions[0].equals(Manifest.permission.CAMERA))) {
            return false;
        }

       // View noPermissionView = activity.findViewById(R.id.no_permission);
       Toast.makeText(activity, "not support camera", Toast.LENGTH_LONG).show();
        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
           // noPermissionView.setVisibility(View.GONE);
            return true;
        }

        requestCameraPermission();
        requestExtSDPermission();
        requestaudioPermission();
        //noPermissionView.setVisibility(View.VISIBLE);
        return false;
    }
}
