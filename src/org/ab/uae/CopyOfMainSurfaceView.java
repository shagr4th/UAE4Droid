package org.ab.uae;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class CopyOfMainSurfaceView extends GLSurfaceView {
    public CopyOfMainSurfaceView(Context context, AttributeSet set) {
        super(context, set);
        mParent = (DemoActivity)context;
        mRenderer = new DemoRenderer(mParent);
        setRenderer(mRenderer);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public boolean onTouchEvent(final MotionEvent event) 
    {
        // TODO: add multitouch support (added in Android 2.0 SDK)
        int action = -1;
        /*if( event.getAction() == MotionEvent.ACTION_DOWN )
        	action = 0;
        if( event.getAction() == MotionEvent.ACTION_UP )
        	action = 1;*/
        if( event.getAction() == MotionEvent.ACTION_MOVE )
        	action = 2;
        if (  action >= 0 ) {
        	
        	int x = (int) ((event.getX() / (float) mRenderer.width)* (float) mRenderer.bufferWidth);
        	int y = (int) ((event.getY() / (float) mRenderer.height)* (float) mRenderer.bufferHeight);
            nativeMouse(x , y, action, 0 );
        }
        return true;
    }
    
    private float oldX = 0;
    private float oldY = 0;
    private static final float FACTOR = 15;
    
    @Override
	public boolean onTrackballEvent(MotionEvent event) {
    	int action = -1;
        if( event.getAction() == MotionEvent.ACTION_DOWN )
        	action = 0;
        if( event.getAction() == MotionEvent.ACTION_UP )
        	action = 1;
        if( event.getAction() == MotionEvent.ACTION_MOVE )
        	action = 2;
        //Log.i("UAE", "" + event.getX() + "/" + event.getY());
        if (  action >= 0 ) {
           /* oldX = oldX + FACTOR*event.getX();
            oldY = oldY + FACTOR*event.getY();
            if (oldX < 0)
            	oldX = 0;
            if (oldY < 0)
            	oldY = 0;
            if (mRenderer != null && oldX > mRenderer.bufferWidth)
            	oldX = mRenderer.bufferWidth;
            if (mRenderer != null && oldY > mRenderer.bufferHeight)
            	oldY = mRenderer.bufferHeight;
            nativeMouse( (int)(oldX), (int)(oldY), action );*/
        	
        	nativeMouse( (int) (FACTOR*event.getX()), (int) (FACTOR*event.getY()), action, 1);
        }
        return true;
	}

     public void exitApp() {
         mRenderer.exitApp();
     };
     
     public void actionKey(boolean down, int keyCode) {
    	 if (down)
    		 keyDown(keyCode);
    	 else
    		 keyUp(keyCode);
     }
    
	public boolean keyDown(int keyCode) {
		if (keyCode == KeyEvent.KEYCODE_BACK || keyCode == KeyEvent.KEYCODE_MENU)
			return false;
		
		if (keyCode >= 2000) {
			nativeKey( keyCode-2000, 1, 1 );
			return true;
		} else if (keyCode >= 1000) {
			nativeKey( keyCode-1000, 1, 0 );
			return true;
		}
		
		int h [] = mParent.getRealKeyCode(keyCode);
		keyCode = h[0];
	
		
		if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN || keyCode == KeyEvent.KEYCODE_VOLUME_UP)
			return false;
		
		nativeKey( keyCode, 1, mParent.joystick );
         return true;
     }
	
	public boolean keyUp(int keyCode) {
		if (keyCode == KeyEvent.KEYCODE_BACK || keyCode == KeyEvent.KEYCODE_MENU)
			return false;
		
		if (keyCode >= 2000) {
			nativeKey( keyCode-2000, 0, 1 );
			return true;
		} else if (keyCode >= 1000) {
			nativeKey( keyCode-1000, 0, 0 );
			return true;
		}
		
		int h [] = mParent.getRealKeyCode(keyCode);
		keyCode = h[0];
		
		if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN || keyCode == KeyEvent.KEYCODE_VOLUME_UP)
			return false;
		
		nativeKey( keyCode, 0, mParent.joystick );
         return true;
     }

    DemoRenderer mRenderer;
    DemoActivity mParent;

    public static native void nativeMouse( int x, int y, int action, int relative );
    public static native void nativeKey( int keyCode, int down, int joystick );
     
	@Override
	public void onPause() {
		if (mRenderer != null)
			mRenderer.nativePause();
		super.onPause();
	}

	@Override
	public void onResume() {
		if (mRenderer != null)
			mRenderer.nativeResume();
		super.onResume();
	}

	public void shiftImage(int leftDPIs) {
		if (mRenderer != null)
			mRenderer.shiftImage(leftDPIs);
	}

	/*protected boolean readyToRenderAgain = true;
    protected Runnable renderRunnable = new Runnable() {
        public void run() {
            readyToRenderAgain = true;
        }
    };

    public void requestRender() {
        if (readyToRenderAgain) {
            super.requestRender();
            queueEvent(renderRunnable);
        }
        readyToRenderAgain = false;
    }*/

}
