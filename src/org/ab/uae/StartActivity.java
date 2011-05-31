package org.ab.uae;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class StartActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.start);
		
		findViewById(R.id.configure).setOnClickListener(new View.OnClickListener() {
		    public void onClick(View v) {
		    	configure_amiga(v);
		    }
		});
		
		findViewById(R.id.start).setOnClickListener(new View.OnClickListener() {
		    public void onClick(View v) {
		    	start_amiga(v);
		    }
		});
	}
	
	public void configure_amiga(View v) {
		Intent settingsIntent = new Intent();
   		settingsIntent.setClass(this, Settings.class);
   		startActivityForResult(settingsIntent, 0);
	}
	
	public void start_amiga(View v) {
		Intent settingsIntent = new Intent();
   		settingsIntent.setClass(this, DemoActivity.class);
   		startActivityForResult(settingsIntent, 20);
	}

}
