package olmoprieto.pong_dx;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
//import android.opengl.EGLConfig;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    private GLSurfaceView glSurfaceView;
    private RendererWrapper rendererWrapper;
    private boolean rendererSet;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        if(getSupportActionBar() != null) {
            getSupportActionBar().hide();
        }

        ActivityManager activityManager = (ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();

        final boolean supportsES2 = configurationInfo.reqGlEsVersion >= 0x20000;// || isEmulator();
        if (supportsES2) {
            glSurfaceView = new GLSurfaceView(this);

            glSurfaceView.setEGLContextClientVersion(2);
            glSurfaceView.setPreserveEGLContextOnPause(true);

            Display display = getWindowManager().getDefaultDisplay();
            rendererWrapper = new RendererWrapper(display.getWidth(), display.getHeight());
            glSurfaceView.setRenderer(rendererWrapper);

            rendererSet = true;
            setContentView(glSurfaceView);
        }
        else {
            // Should never happen in production
            Toast.makeText(this, "This device does not support OpenGLES2", Toast.LENGTH_LONG).show();

            return;
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent me) {
        switch (me.getAction()) {
            case MotionEvent.ACTION_DOWN : {

                rendererWrapper.onTouchEvent(me.getX(), me.getY());

                return true;
                //break;
            }
        }

        return false;
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (rendererSet == true) {
            glSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (rendererSet == true) {
            glSurfaceView.onResume();
        }
    }

    // The code below hides status and navigation bar
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            hideSystemUI();
        }
    }

    private void hideSystemUI() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN);
    }
}
