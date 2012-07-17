package org.ab.uae;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MainSurfaceView  extends SurfaceView implements SurfaceHolder.Callback, Runnable {
	
	DemoRenderer mRenderer;
	 SurfaceHolder mSurfaceHolder;
    public MainSurfaceView(Context context, AttributeSet set) {
        super(context, set);
        mParent = (DemoActivity)context;
        mRenderer = new DemoRenderer(mParent);
        
         mSurfaceHolder = getHolder();
         mSurfaceHolder.addCallback(this);
    }

    @Override
    public boolean onTouchEvent(final MotionEvent event) 
    {
    	
    	if (((DemoActivity) getContext()).vKeyPad != null && ((DemoActivity) getContext()).touch && DemoActivity.currentKeyboardLayout == 0) {
			boolean b = ((DemoActivity) getContext()).vKeyPad.onTouch(event, false);
			
			if (b)
				return true;
		}
    	
        // TODO: add multitouch support (added in Android 2.0 SDK)
        int action = -1;
        /*if( event.getAction() == MotionEvent.ACTION_DOWN )
        	action = 0;
        if( event.getAction() == MotionEvent.ACTION_UP )
        	action = 1;*/
        if( event.getAction() == MotionEvent.ACTION_MOVE )
        	action = 2;
        if (  action >= 0 ) {
        	
        	int x = (int) ((event.getX() / (float) width)* (float) bufferWidth);
        	int y = (int) ((event.getY() / (float) height)* (float) bufferHeight);
            nativeMouse(x , y, action, 0 );
        }
        return true;
    }
    

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
           nativeMouse( (int) (FACTOR*event.getX()), (int) (FACTOR*event.getY()), action, 1);
           actionKey(action == 0, KeyEvent.KEYCODE_DPAD_CENTER);
        }
        return true;
	}

     public void exitApp() {
    	 if (mRenderer != null)
    		 mRenderer.nativeDone();
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
		
		int joystick_nr = 1;
		
		if (keyCode >= 2000) {
			nativeKey( keyCode-2000, 1, 1, joystick_nr );
			try { Thread.sleep(50); } catch (InterruptedException e) {}
			return true;
		} else if (keyCode >= 1000) {
			nativeKey( keyCode-1000, 1, 0, joystick_nr );
			try { Thread.sleep(50); } catch (InterruptedException e) {}
			return true;
		}
		
		int h [] = mParent.getRealKeyCode(keyCode);
		keyCode = h[0];
		joystick_nr = h[1];
		
		if (keyCode == KeyEvent.KEYCODE_O) {
			mParent.setRightMouse(0);
			mParent.mouse_button = 0;
			nativeMouse(0, 0, 0, 1);
			return true;
		}
		
		if (keyCode == KeyEvent.KEYCODE_L) {
			mParent.setRightMouse(1);
			mParent.mouse_button = 1;
			nativeMouse(0, 0, 0, 1);
			//mParent.setRightMouse(0);
			return true;
		}
		
		/*if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN || keyCode == KeyEvent.KEYCODE_VOLUME_UP)
			return false;*/
		
		nativeKey( keyCode, 1, mParent.joystick, joystick_nr );
         return true;
     }
	
	public boolean keyUp(int keyCode) {
		if (keyCode == KeyEvent.KEYCODE_BACK || keyCode == KeyEvent.KEYCODE_MENU)
			return false;
		
		int joystick_nr = 1;
		
		if (keyCode >= 2000) {
			nativeKey( keyCode-2000, 0, 1, joystick_nr );
			try { Thread.sleep(50); } catch (InterruptedException e) {}
			return true;
		} else if (keyCode >= 1000) {
			nativeKey( keyCode-1000, 0, 0, joystick_nr );
			try { Thread.sleep(50); } catch (InterruptedException e) {}
			return true;
		}
		
		int h [] = mParent.getRealKeyCode(keyCode);
		keyCode = h[0];
		joystick_nr = h[1];
		
		if (keyCode == KeyEvent.KEYCODE_O) {
			mParent.setRightMouse(0);
			mParent.mouse_button = 0;
			nativeMouse(0, 0, 1, 1);
			return true;
		}
		
		if (keyCode == KeyEvent.KEYCODE_L) {
			mParent.setRightMouse(1);
			mParent.mouse_button = 1;
			nativeMouse(0, 0, 1, 1);
			//mParent.setRightMouse(0);
			return true;
		}
		
		if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN || keyCode == KeyEvent.KEYCODE_VOLUME_UP)
			return false;
		
		nativeKey( keyCode, 0, mParent.joystick, joystick_nr );
         return true;
     }

    DemoActivity mParent;

    public static native void nativeMouse( int x, int y, int action, int relative );
    public static native void nativeKey( int keyCode, int down, int joystick, int joystick_nr );
    public static native void setNumJoysticks(int numJoysticks);
  
	public void shiftImage(int leftDPIs) {
		if (leftDPIs > 0) {
			DisplayMetrics metrics = new DisplayMetrics();
			mParent.getWindowManager().getDefaultDisplay().getMetrics(metrics);
	        pixels = (int) (leftDPIs * metrics.density + 0.5f);
	        Log.i("uae", "pixels: " + pixels);
	        scaleX = (float) (width-pixels)/bufferWidth;
			scaleY = (float) height/bufferHeight;
			coordsChanged = true;
			
		} else {
			pixels = 0;
			scaleX = (float) width/bufferWidth;
			scaleY = (float) height/bufferHeight;
			coordsChanged = true;
			
		}
		Log.i("UAE", "new scale: " + scaleX + "-" + scaleY + "-" + pixels);
		if (width < height) {
			scaleY = scaleX;
		}
		
		initMatrix();
    	
	}
	
	public void run() {
		Log.i("Renderer", "nativeInit");
		if (mRenderer != null)
   		 mRenderer.nativeInit(mParent, buffer, 1);
	}

	public void surfaceChanged(SurfaceHolder holder, int format, int w,
			int h) {
		if (mRenderer != null)
   		 mRenderer.nativeResize(w, h);
       
        coordsChanged = true;
        
		this.width = w;
		this.height = h;
		scaleX = (float) (width-pixels)/bufferWidth;
		scaleY = (float) height/bufferHeight;
		if (width < height) {
			scaleY = scaleX;
		}
		
		initMatrix();
    	
		if (updater != null)
			updater.start();
		
		Log.i("UAE", "new onSurfaceChanged: " + scaleX + "-" + scaleY + "-" + pixels);
		
		if (((DemoActivity) getContext()).vKeyPad != null)
			((DemoActivity) getContext()).vKeyPad.resize(width, height);
		
	}
	
	public void initMatrix() {
		/* matrixScreen = new Matrix();
		 matrixScreen.setScale(scaleX, scaleY);
		 matrixScreen.postTranslate(pixels, 0);
    	 */
		
		if (width < height) {
			scaleY = scaleX;
		} else {
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
		 String scale = prefs.getString("scale", "stretched");
		 pixels = 0;
		 pixelsH = 0;
		 if ("scaled".equals(scale)) {
			 scaleX = scaleY;
			 pixels = (int) ((width - bufferWidth*scaleX) / 2);
		 } else if ("1x".equals(scale)) {
			 scaleX = 1.0f;
			 scaleY = 1.0f;
			 pixels = (int) ((width - bufferWidth*scaleX) / 2);
			 pixelsH = (int) ((height - bufferHeight*scaleX) / 2);
		 } else if ("2x".equals(scale)) {
			 scaleX = 2.0f;
			 scaleY = 2.0f;
			 pixels = (int) ((width - bufferWidth*scaleX) / 2);
			 pixelsH = (int) ((height - bufferHeight*scaleX) / 2);
		 }
		}
		 matrixScreen = new Matrix();
		 matrixScreen.setScale(scaleX, scaleY);
		 matrixScreen.postTranslate(pixels, pixelsH);
	}
	
	protected float scaleX;
	protected float scaleY;
	protected boolean coordsChanged;
	protected int bufferWidth = 320;
	protected int bufferHeight = 240;
	protected int width;
	protected int height;
	protected int pixels;
	protected int pixelsH;
	 Matrix matrixScreen;
	 
	ShortBuffer buffer;
	ScreenUpdater updater;
	
	 PaintFlagsDrawFilter setfil = new PaintFlagsDrawFilter(0, 
			 Paint.FILTER_BITMAP_FLAG); 
	 
	 
	 boolean aliased = false;
	
	public void surfaceCreated(SurfaceHolder holder) {
		if (buffer == null) {
			Log.i("UAE", "surfaceCreated");
	    	ByteBuffer bb = ByteBuffer.allocateDirect(320*240*2);
	    	buffer = bb.asShortBuffer();
	    	 mainScreen = Bitmap.createBitmap(320, 240, Bitmap.Config.RGB_565);
	    	 
	    	 //updater =new ScreenUpdater(this);
	    	 //updater.start();
	    	 
	    	if (DemoActivity.nativeThread == null || !DemoActivity.nativeThread.isAlive()) {
	    		DemoActivity.nativeThread = new Thread(this);
	    		DemoActivity.nativeThread.start();
	    	}
    	}
	}
	
	public Bitmap mainScreen;
	
	long t = System.currentTimeMillis();
	int frames;
	public void checkFPS() {

        frames++;
        
        if (frames % 20 == 0) {
        	long t2 = System.currentTimeMillis();
        	Log.i("uae", "FPS: " + 20000 / (t2 - t));
        	t = t2;
        }
        
	}
	public void requestRender() {
		
        //checkFPS();
		if (updater != null)
			updater.render();
		else {
			Canvas c = null;
	        try {
	            c = mSurfaceHolder.lockCanvas(null);
	            synchronized (mSurfaceHolder) {
	            	 mainScreen.copyPixelsFromBuffer(buffer);
	            	 if (c != null && matrixScreen != null) {
	 	            	if (aliased)
	            		 c.setDrawFilter(setfil);
	 	            	c.drawBitmap(mainScreen, matrixScreen, null);
	 	            	if (mParent.vKeyPad != null && mParent.touch && DemoActivity.currentKeyboardLayout == 0)
	 	            		mParent.vKeyPad.draw(c);
	            	 
	            	}
	            	 
	            }
	        } finally {
	            // do this in a finally so that if an exception is thrown
	            // during the above, we don't leave the Surface in an
	            // inconsistent state
	            if (c != null) {
	                mSurfaceHolder.unlockCanvasAndPost(c);
	            }
	        }
		}
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		if (updater != null)
			updater.stop();
	}
	
	public void onPause() {
		if (mRenderer != null)
   		 mRenderer.nativePause();
	}

	public void onResume() {
		if (mRenderer != null)
   		 mRenderer.nativeResume();
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

