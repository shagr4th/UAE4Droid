package org.ab.uae;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.util.DisplayMetrics;
import android.util.Log;

public class DemoRenderer implements GLSurfaceView.Renderer, Runnable {

	private ShortBuffer buffer;
	private GLBitmap bitmap;
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
	
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	if (buffer == null) {
	    	ByteBuffer bb = ByteBuffer.allocateDirect(512*256*2);
	    	buffer = bb.asShortBuffer();
	    	bitmap = new GLBitmap(0, 0, 1, bufferWidth, bufferHeight);
	    	bitmap.setBuffer(buffer);
	    	textureReInit = true;
	    	if (nativeThread == null) {
	    		nativeThread = new Thread(this);
	    		nativeThread.start();
	    	}
    	}
        gl.glDisable(GL10.GL_DITHER);
        gl.glDisable(GL10.GL_LIGHTING);
        gl.glDisable(GL10.GL_CULL_FACE);
        gl.glDisable(GL10.GL_MULTISAMPLE);
       
        //gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT, GL10.GL_FASTEST);
        gl.glClearColor(0, 0, 0, 0);
        //gl.glShadeModel(GL10.GL_SMOOTH);
        gl.glDisable(GL10.GL_DEPTH_TEST);
        //gl.glEnable(GL10.GL_TEXTURE_2D);
        
        gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
        gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
        
        textureReInit = true;
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
        //gl.glViewport(0, 0, w, h);
        nativeResize(w, h);
        gl.glViewport(0, 0, w, h);

        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glLoadIdentity();
        gl.glOrthof (0f, w, h, 0f, -1f,1f); 
        
        gl.glFrontFace(GL10.GL_CCW);
		
        coordsChanged = true;
        
		this.width = w;
		this.height = h;
		scaleX = (float) (width-pixels)/bufferWidth;
		scaleY = (float) height/bufferHeight;
		if (width < height) {
			scaleY = scaleX;
		}
		
		Log.i("UAE", "new onSurfaceChanged: " + scaleX + "-" + scaleY + "-" + pixels);
		
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
			if (bitmap != null)
				bitmap.changeCoords(pixels, 0, scaleX, scaleY);
		} else {
			pixels = 0;
			scaleX = (float) width/bufferWidth;
			scaleY = (float) height/bufferHeight;
			coordsChanged = true;
			if (bitmap != null)
				bitmap.changeCoords(0, 0, scaleX, scaleY);
		}
		Log.i("UAE", "new scale: " + scaleX + "-" + scaleY + "-" + pixels);
		if (width < height) {
			scaleY = scaleX;
		}
	}

    public void onDrawFrame(GL10 gl) {
    	
        if (coordsChanged) {
        	bitmap.changeCoords(scaleX, scaleY);
			coordsChanged = false;
			//textureReInit = true;
		}
    	bitmap.draw(gl, textureReInit, false);
    	textureReInit = false;
    	
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