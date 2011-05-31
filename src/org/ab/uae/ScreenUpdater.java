package org.ab.uae;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.util.Log;

public class ScreenUpdater implements Runnable {
	
	MainSurfaceView view;
	boolean running;
	Thread thread;
	
	public ScreenUpdater(MainSurfaceView view) {
		this.view = view;
	}
	
	public void start() {
		running = true;
		thread = new Thread(this);
		thread.start();
	}
	
	public void stop() {
		running = false;
		thread.interrupt();
	}
	
	long t = System.currentTimeMillis();
	int frames;
	public void checkFPS() {

        frames++;
        
        if (frames % 20 == 0) {
        	long t2 = System.currentTimeMillis();
        	Log.i("uae", "UFPS: " + 20000 / (t2 - t));
        	t = t2;
        }
        
	}
	
	public void render() {
		synchronized (this) {
			notify();
		}
	}
	
	 PaintFlagsDrawFilter setfil = new PaintFlagsDrawFilter(0, 
			 Paint.FILTER_BITMAP_FLAG); 
	 

	public void run() {
		while (running) {
			
			try {
				synchronized(this) {
					wait();
				}
			} catch (InterruptedException e) {
				break;
			}
			
			Canvas c = null;
	        try {
	            c = view.mSurfaceHolder.lockCanvas();
	            view.mainScreen.copyPixelsFromBuffer(view.buffer);
	
	            if (c != null) {
	            	
		            if (view.aliased)
	           		 c.setDrawFilter(setfil);
		            c.drawBitmap(view.mainScreen, view.matrixScreen, null);
		            if (view.mParent.vKeyPad != null && view.mParent.touch && view.mParent.currentKeyboardLayout == 0)
		            	view.mParent.vKeyPad.draw(c);
	            }
           	
	        } catch (Exception e) {
	        	e.printStackTrace();
			} finally {
	            if (c != null) {
	            	view.mSurfaceHolder.unlockCanvasAndPost(c);
	            }
	        }
	        
	   
		}
	}

}
