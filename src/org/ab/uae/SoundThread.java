package org.ab.uae;

import java.nio.ByteBuffer;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

public class SoundThread implements Runnable
{

    private ByteBuffer bb;
    private AudioTrack at;
    private int total_buffer_len;
    private int block_buffer_len;
    private boolean playing;
    private Thread audio;
    

    public SoundThread(int freq, int bits, int block_buffer_len, int nb_blocks) {
        this.block_buffer_len = block_buffer_len;
        total_buffer_len = block_buffer_len*nb_blocks;
        at = new AudioTrack(AudioManager.STREAM_MUSIC, freq, AudioFormat.CHANNEL_CONFIGURATION_MONO, bits>=16?AudioFormat.ENCODING_PCM_16BIT:AudioFormat.ENCODING_PCM_8BIT, block_buffer_len * nb_blocks, AudioTrack.MODE_STREAM);
    }

    public ByteBuffer play() {
        if (audio != null)
        {
            playing = false;
            audio.interrupt();
        }
        bb = ByteBuffer.allocateDirect(total_buffer_len);
        playing = true;
        audio = new Thread(this);
        audio.start();
        return bb;
    }

    public synchronized void send() {
        notify();
    }

    public void stop() {
        playing = false;
    }

    public void run() {
        at.play();
        byte back_buffer [] = new byte [block_buffer_len];
        while (playing)
        {
            synchronized(this) {
                try {
					wait();
				} catch (InterruptedException e) {
					break;
				}
            }
            //Log.i("UAE", "playing: " + bb.position());
            bb.get(back_buffer);
            at.write(back_buffer, 0, block_buffer_len);
            if (bb.position() == total_buffer_len)
                bb.position(0);
        }
        at.stop();
    }
}
