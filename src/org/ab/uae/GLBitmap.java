package org.ab.uae;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.opengl.GLUtils;
import android.os.Build;
import android.util.Log;

public class GLBitmap {
	
	private int x;
	private int y;
	private int width, width_p2;
	private int height, height_p2;
	private float scaleX;
	private float scaleY;
	private int textureID;
	private ShortBuffer buffer;
	private boolean externalBuffer;
	public void setBuffer(ShortBuffer buffer) {
		this.buffer = buffer;
		externalBuffer = true;
	}

	private FloatBuffer mFVertexBuffer;
    private FloatBuffer mTexBuffer;
    private ShortBuffer mIndexBuffer;
    public boolean coordsInitialized;
    private int vbos [];
    private int vertex_object ;
    private int index_object ;
    private int texture_object ;
    private Bitmap bitmap;
    public Bitmap getBitmap() {
		return bitmap;
	}

	private int z;
    
    public GLBitmap(int x, int y, int z, Bitmap bitmap) {
    	this(x, y, z, bitmap.getWidth(), bitmap.getHeight());
    	this.bitmap = bitmap;
    }

	public GLBitmap(int x, int y, int z, int width, int height) {
		this.x = x;
		this.y = y;
		this.z = z;
		this.width = width;
		this.height = height;
		this.width_p2 = power2(width);
		this.height_p2 = power2(height);
		this.scaleX = 1.0f;
		this.scaleY = 1.0f;
	}
	
	public boolean changeCoords(float scaleX, float scaleY) {
		this.scaleX = scaleX;
		this.scaleY = scaleY;
		coordsInitialized = false;
		return false;
	}
	
	public boolean changeCoords(int x, int y, float scaleX, float scaleY) {
		this.x = x;
		this.y = y;
		this.scaleX = scaleX;
		this.scaleY = scaleY;
		coordsInitialized = false;
		return false;
	}
	
	public void initTexture(GL10 gl, boolean textureReInit) {
		
		if (!coordsInitialized || textureReInit) {
			initVertexes(gl);
			coordsInitialized = true;
		}
		if (textureReInit) {
			int textures [] = new int [1];
			gl.glGenTextures(1, textures, 0);
			textureID = textures[0];
			Log.w("frodoc64", "Allocated texture: " + textureID);
		
		
			gl.glActiveTexture(GL10.GL_TEXTURE0);
			gl.glBindTexture(GL10.GL_TEXTURE_2D, textureID);
         
			gl.glPixelStorei(GL10.GL_UNPACK_ALIGNMENT, 1);

			gl.glTexParameterx(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S,GL10.GL_CLAMP_TO_EDGE);
			gl.glTexParameterx(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T,GL10.GL_CLAMP_TO_EDGE); // GL_LINEAR
        
        
	        gl.glTexParameterx(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER,GL10.GL_NEAREST);
	        gl.glTexParameterx(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER,GL10.GL_LINEAR); // GL_LINEAR
	        
	        gl.glTexEnvf(GL10.GL_TEXTURE_ENV, GL10.GL_TEXTURE_ENV_MODE,GL10.GL_MODULATE); // GL_REPLACE
	      //  ((GL11) gl).glTexParameteriv(GL10.GL_TEXTURE_2D, GL11Ext.GL_TEXTURE_CROP_RECT_OES, new int [] { 0, height, width, -height }, 0);
		}
        if (this.bitmap != null && textureReInit) {
        	Bitmap newBitmap = Bitmap.createBitmap(width_p2, height_p2, this.bitmap.getConfig());
    		Canvas c = new Canvas(newBitmap);
    		c.drawBitmap(this.bitmap, 0, 0, null);
    		GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, newBitmap, 0);
    		newBitmap.recycle();
        }
	}
	
	public void initVertexes(GL10 gl) {
		
		ByteBuffer vbb = ByteBuffer.allocateDirect(4 * 3 * 4);
        vbb.order(ByteOrder.nativeOrder());
        mFVertexBuffer = vbb.asFloatBuffer();

        ByteBuffer tbb = ByteBuffer.allocateDirect(4 * 2 * 4);
        tbb.order(ByteOrder.nativeOrder());
        mTexBuffer = tbb.asFloatBuffer();

        ByteBuffer ibb = ByteBuffer.allocateDirect(4 * 2);
        ibb.order(ByteOrder.nativeOrder());
        mIndexBuffer = ibb.asShortBuffer();
        
       // Log.i("frodoc64", "scaleX: " + scaleX);
        
        float zz = ((float)z-1)/1000;
        float[] coords = {
                // X, Y, Z
                
                 x + (int) ((float)width * scaleX),  y, zz,
                 x + (int) ((float)width * scaleX), y + (int) ((float)height*scaleY), zz,
                 x, y, zz,
                 x, y + (int) ((float)height*scaleY), zz
        };
        /*for(int i=0;i<coords.length;i++)
        	Log.i("frodoc64", "" + coords[i]);*/
        
      //Texture coords 
        float[] texturCoords = new float[] 
        { 
                     
                     1f / ((float)width_p2 / width) ,  0f, 0,
                     1f / ((float)width_p2 / width) ,      1f / ((float)height_p2 / height), 0,
               0f    ,  0f, 0,
               0f    ,     1f / ((float)height_p2 / height) ,0
        }; 

        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < 3; j++) {
                mFVertexBuffer.put(coords[i*3+j]);
            }
        }

        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < 2; j++) {
                mTexBuffer.put(texturCoords[i*3+j]);
            }
        }

        for(int i = 0; i < 4; i++) {
            mIndexBuffer.put((short) i);
        }

        mFVertexBuffer.position(0);
        mTexBuffer.position(0);
        mIndexBuffer.position(0);
        
        boolean cliq_bug = Build.MODEL != null && Build.MODEL.toUpperCase().contains("MB200");
		
		if (gl instanceof GL11 && !cliq_bug) {
			vbos = new int [3];
			GL11 gl11 = (GL11) gl;
			gl11.glGenBuffers(vbos.length, vbos, 0); 
	        vertex_object = vbos[0]; 
	        texture_object = vbos[1];
	        index_object = vbos[2]; 
	        
//	        ByteBuffer vbbF = ByteBuffer.allocateDirect(4 * 5 * 4);
//	        vbbF.order(ByteOrder.nativeOrder());
//	        FloatBuffer mFT = vbbF.asFloatBuffer();
//	        mFT.put(mFVertexBuffer);
//	        mFT.put(mTexBuffer);
//	        mFT.position(0);
//	        
//	        gl11.glBindBuffer(GL11.GL_ARRAY_BUFFER, vertex_object); 
//	        gl11.glBufferData(GL11.GL_ARRAY_BUFFER, 20 * 4, mFT, GL11.GL_STATIC_DRAW); 
	       
	        gl11.glBindBuffer(GL11.GL_ARRAY_BUFFER, vertex_object); 
	        gl11.glBufferData(GL11.GL_ARRAY_BUFFER, 12 * 4, mFVertexBuffer, GL11.GL_STATIC_DRAW); 
	        
	        gl11.glBindBuffer(GL11.GL_ARRAY_BUFFER, texture_object); 
	        gl11.glBufferData(GL11.GL_ARRAY_BUFFER, 8 * 4, mTexBuffer, GL11.GL_STATIC_DRAW); 
	        
	        gl11.glBindBuffer(GL11.GL_ARRAY_BUFFER, 0);
	        	
	        gl11.glBindBuffer(GL11.GL_ELEMENT_ARRAY_BUFFER, index_object); 
	        gl11.glBufferData(GL11.GL_ELEMENT_ARRAY_BUFFER, 4 * 2, mIndexBuffer, GL11.GL_STATIC_DRAW); 

	        gl11.glBindBuffer(GL11.GL_ELEMENT_ARRAY_BUFFER, 0); 
	        
	        
	        /* int error = gl11.glGetError(); 
            if (error != GL10.GL_NO_ERROR) 
            { 
                Log.e("frodoc64", "Generate vertex buffer GLError: " + error); 
            } */
            
		}
        
	}
	
	public void glFree(GL10 gl) {
		Log.i("frodoc64", "free GL bitmap: " + z);
		gl.glDeleteTextures(1, new int [] { textureID }, 0);
		if (gl instanceof GL11) 
        {
			// delete buffers
			// gl11.glDeleteBuffers(1, buffer, 0); 
        }
	}
	
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
	
	private static int power2(int s) { 		if (s > 0) { 			for(int i=0;i<10000;i++) { 				int inf = (int) Math.pow(2, i); 				int sup = (int) Math.pow(2, i+1); 				if (s <= sup && s >=inf) 					return sup; 			} 		} 		return s; 	}
	public void draw(GL10 gl, boolean textureReInit, boolean transparency) {
		
		checkFPS();
		
		
		initTexture(gl, textureReInit);
		
		
		if (bitmap == null) {
			if (buffer == null) {
				ByteBuffer bb = ByteBuffer.allocateDirect(width_p2*height_p2*2);
				buffer = bb.asShortBuffer();
				gl.glTexImage2D(GL10.GL_TEXTURE_2D, 0, GL10.GL_RGB, width_p2, height_p2, 0, GL10.GL_RGB, GL10.GL_UNSIGNED_SHORT_5_6_5, buffer);
			}
			
			if (externalBuffer) {
				gl.glTexImage2D(GL10.GL_TEXTURE_2D, 0, GL10.GL_RGB, width_p2, height_p2, 0, GL10.GL_RGB, GL10.GL_UNSIGNED_SHORT_5_6_5, buffer);
				externalBuffer = false;
			}
			//gl.glTexImage2D(GL10.GL_TEXTURE_2D, 0, GL10.GL_RGB, width_p2, height_p2, 0, GL10.GL_RGB, GL10.GL_UNSIGNED_SHORT_5_6_5, buffer);
			
			
			
			gl.glTexSubImage2D(GL10.GL_TEXTURE_2D, 0, 0, 0, width, height,  GL10.GL_RGB, GL10.GL_UNSIGNED_SHORT_5_6_5, buffer);
			
			
		}
		
		if (z >= 2 && transparency) {
			gl.glEnable(GL10.GL_BLEND);
			//gl.glBlendFunc (GL10.GL_ONE, GL10.GL_ONE);
			gl.glBlendFunc(GL10.GL_SRC_ALPHA, GL10.GL_ONE_MINUS_SRC_ALPHA);
			//gl.glColor4f(0.5f, 0.5f, 1f, 0.5f);
		}
	       
		if (vbos != null) {
			GL11 gl11 = (GL11) gl;
			//if (textureReInit) {
			gl11.glBindBuffer(GL11.GL_ARRAY_BUFFER, vertex_object); 
			gl11.glVertexPointer(3, GL11.GL_FLOAT, 12, 0);
			gl.glEnable(GL11.GL_TEXTURE_2D);
			gl11.glBindBuffer(GL11.GL_ARRAY_BUFFER, texture_object); 
			gl11.glTexCoordPointer(2, GL11.GL_FLOAT, 8, 0);
			gl11.glBindBuffer(GL11.GL_ELEMENT_ARRAY_BUFFER, index_object); 
			//}
			gl11.glDrawElements(GL11.GL_TRIANGLE_STRIP, 4, GL11.GL_UNSIGNED_SHORT, 0); 
			
		} else {
			
		
		 gl.glVertexPointer(3, GL10.GL_FLOAT, 0, mFVertexBuffer);
        gl.glEnable(GL10.GL_TEXTURE_2D);
        gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, mTexBuffer);
        gl.glDrawElements(GL10.GL_TRIANGLE_STRIP, 4, GL10.GL_UNSIGNED_SHORT, mIndexBuffer);
		}
		
		if (z == 2) {
			gl.glBlendFunc(GL10.GL_ONE, GL10.GL_ONE_MINUS_SRC_ALPHA);
		}
        
	}

	public int getWidth() {
		return width;
	}

	public int getHeight() {
		return height;
	}

}
