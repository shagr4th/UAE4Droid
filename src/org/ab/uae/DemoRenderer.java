package org.ab.uae;

import java.nio.ShortBuffer;

import android.util.DisplayMetrics;
import android.util.Log;

public class DemoRenderer implements Runnable {

	private ShortBuffer buffer;
	private DemoActivity surfaceView;
	private boolean textureReInit;
	protected float scaleX;
	protected float scaleY;
	protected boolean coordsChanged;
	protected int bufferWidth = 320;
	protected int bufferHeight = 240;
	protected int width;
	protected int height;
	protected int pixels;
	protected static Thread nativeThread;
	
	public DemoRenderer(DemoActivity surfaceView) {
		this.surfaceView = surfaceView;
	}
	
	public void run() {
		Log.i("Renderer", "nativeInit");
		nativeInit(surfaceView, buffer, 1);
	}
	
    
    
	public void shiftImage(int leftDPIs) {
		if (leftDPIs > 0) {
			DisplayMetrics metrics = new DisplayMetrics();
			surfaceView.getWindowManager().getDefaultDisplay().getMetrics(metrics);
	        pixels = (int) (leftDPIs * metrics.density + 0.5f);
	        Log.i("frodo64", "pixels: " + pixels);
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
	}

    public void exitApp() {
         nativeDone();
    };
    
   
    public native void nativePause( );
    public native void nativeResume( );

    public native void nativeInit(DemoActivity demo, ShortBuffer buffer, int directmode);
    public native void nativeResize(int w, int h);
    //private native void nativeRender();
    public native void nativeDone();

}