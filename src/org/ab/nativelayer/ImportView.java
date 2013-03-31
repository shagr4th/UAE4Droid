package org.ab.nativelayer;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.ab.uae.R;

import android.app.ListActivity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

public class ImportView extends ListActivity implements OnItemLongClickListener {
	
	/**
     * text we use for the parent directory
     */
    private final static String PARENT_DIR = "..";
    /**
     * Currently displayed files
     */
    private final List<String> currentFiles = new ArrayList<String>();
    /**
     * Currently displayed directory
     */
    private File currentDir = null;
	private ImportFileView importView;

    @Override
    public void onCreate(final Bundle icicle) {
        super.onCreate(icicle);

        // go to the root directory
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(this);
        String last_dir = sp.getString("last_dir", "/sdcard");
        importView = (ImportFileView) getIntent().getSerializableExtra("import");
        String specificDir = (String) getIntent().getStringExtra("specificDir");
        if (specificDir != null && new File(specificDir).exists()) {
        	showDirectory(specificDir);
        } else
        	showDirectory(last_dir);
        
        getListView().setOnItemLongClickListener(this);
    }
    
    private File currentSelectedFile;

    @Override
    protected void onListItemClick(final ListView l, final View v, final int position, final long id) {
        if (position == 0 && PARENT_DIR.equals(this.currentFiles.get(0))) {
        	if (currentSelectedFile != null) {
        		currentSelectedFile =null;
        		showDirectory(this.currentDir.getAbsolutePath());
        	} else
        		showDirectory(this.currentDir.getParent());
        } else {
        	
        	if (currentSelectedFile != null) {
        		String extra = this.currentFiles.get(position);
        		if (importView != null) {
	        		String extra2 = importView.getExtra2(position);
	        		if (extra2 != null)
	        			extra = extra2;
        		}
        		selectFile(currentSelectedFile, extra, position);
        	}
        	
            final File file = new File(this.currentFiles.get(position));

            if (file.isDirectory()) {
            	currentSelectedFile = null;
                showDirectory(file.getAbsolutePath());
            } else {
            	if (importView != null && importView.virtualDir) {
            		currentSelectedFile = file;
            		this.currentFiles.clear();
            		this.currentFiles.add(PARENT_DIR);
            		ArrayList<String> list = importView.getExtendedList( getApplication(), currentSelectedFile);
            		if (list!= null)
            			this.currentFiles.addAll(list);
            		showList();
            	} else if (file.exists())
            		selectFile(file, null, 0);
            }
           
        }
    }
    
    private void selectFile(File file, String extra1, int extra2) {
    	final Intent extras = new Intent();
    	extras.putExtra("currentFile", file.getAbsolutePath());
    	if (extra1 != null)
    		extras.putExtra("extra1", extra1);
    	extras.putExtra("extra2", extra2);
        setResult(RESULT_OK, extras);
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(this);
        Editor e = sp.edit();
        e.putString("last_dir", file.getParent());
        e.commit();
        finish();
    }
    
   
    /**
     * Show the contents of a given directory as a selectable list
     *
     * @param path      the directory to display
     */
    private void showDirectory(final String path) {
        // we clear any old content and add an entry to get up one level
        this.currentFiles.clear();
        this.currentDir = new File(path);
        if (this.currentDir.getParentFile() != null) {
            this.currentFiles.add(PARENT_DIR);
        }

        // get all directories and relevant files in the given path
        final File[] files = this.currentDir.listFiles();
        final ArrayList<String> sorted = new ArrayList<String>();
        if (files != null) {
	        for (final File file : files) {
	            final String name = file.getAbsolutePath();
	
	            if (file.isDirectory()) {
	                sorted.add(name);
	            } else {
	            	//if (name.toLowerCase().endsWith(".d64") || name.toLowerCase().endsWith(".prg"))
	            	if (importView != null) {
	            		String ext = null;
	            		if (name.length() > 3)
	            			ext = name.substring(name.length()-3).toLowerCase();
	            		if (importView.getExtensions() == null || importView.getExtensions().contains(ext))
	            			sorted.add(name);
	            	} else
	            		sorted.add(name);
	            }
	        }
        }
        Collections.sort(sorted, String.CASE_INSENSITIVE_ORDER);
        this.currentFiles.addAll(sorted);

        showList();
    }
    
    private void showList() {
    	// display these images
        final Context context = this;

        ArrayAdapter<String> filenamesAdapter = new ArrayAdapter<String>(this, R.layout.file_row, this.currentFiles) {

            @Override
            public View getView(final int position, final View convertView, final ViewGroup parent) {
                return new IconifiedTextLayout(context, getItem(position), position);
            }
        };

        setListAdapter(filenamesAdapter);
    }

    // new layout displaying a text and an associated image
    class IconifiedTextLayout extends LinearLayout {

        public IconifiedTextLayout(final Context context, final String path, final int position) {
            super(context);

            setOrientation(HORIZONTAL);

            // determine icon to display
            final ImageView imageView = new ImageView(context);
            final File file = new File(path);

            if (position == 0 && PARENT_DIR.equals(path)) {
                imageView.setImageResource(R.drawable.folder);
            } else {
            	if (file.exists()) {
	                if (file.isDirectory()) {
	                    imageView.setImageResource(R.drawable.folder);
	                } else {
	                    imageView.setImageResource(R.drawable.icon32);
	                }
            	} else {
            		int r = R.drawable.icon;
            		
            		if (importView != null)
            			r = importView.getIcon(position);
            		imageView.setImageResource(r);
            	}
            }
            imageView.setPadding(0, 3, 5, 3);

            // create view for the directory name
            final TextView textView = new TextView(context);

            textView.setText(file.getName());
            textView.setTextSize(18);

            addView(imageView, new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
            addView(textView, new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
        }
    }

	public boolean onItemLongClick(AdapterView<?> arg0, View arg1, int position, long id) {
		if (importView.getExtensions() != null && importView.getExtensions().contains("dir")) {
			if (position == 0 && PARENT_DIR.equals(this.currentFiles.get(0))) {
	        	if (currentSelectedFile != null) {
	        		currentSelectedFile =null;
	        		showDirectory(this.currentDir.getAbsolutePath());
	        	} else
	        		showDirectory(this.currentDir.getParent());
	        	return true;
	        } else {
	        	
	        	final File file = new File(this.currentFiles.get(position));
	        	if (file.isDirectory() && file.exists()) {
	            	selectFile(file, null, 0);
	            	return true;
	        	}
	        	
	        }
		}
		return false;
	}

    

}
