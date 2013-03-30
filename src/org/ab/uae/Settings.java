package org.ab.uae;

import org.ab.nativelayer.ImportView;
import org.ab.nativelayer.KeyPreference;

import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Build;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.view.KeyEvent;

public class Settings extends PreferenceActivity {
	
	 @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        setPreferenceScreen(createPreferenceHierarchy());
        
       
    }
	 
	 Preference romPref ;
	 Preference romKeyPref ;
	 Preference floppy1 ;
	 Preference floppy2 ;
	 Preference floppy3 ;
	 Preference floppy4 ;
	 
	 private PreferenceScreen createPreferenceHierarchy() {
	        // Root
	        PreferenceScreen root = getPreferenceManager().createPreferenceScreen(this);
	        root.setTitle(R.string.configure);
	        PreferenceCategory inlinePrefCat = new PreferenceCategory(this);
	        inlinePrefCat.setTitle(R.string.paths_section);
	        root.addPreference(inlinePrefCat);
	        
	        final SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(this);
	        
	        romPref = new Preference(this) {

				@Override
				protected void onClick() {
					Intent settingsIntent = new Intent();
					settingsIntent.setClass(getContext(), ImportView.class);
					settingsIntent.putExtra("import", new RomImportView("rom"));
	           		startActivityForResult(settingsIntent, Globals.PREFKEY_ROM_INT);
				}
	        	
	        };
	        romPref.setTitle(R.string.rom_location);
	        String rompath = sp.getString(Globals.PREFKEY_ROM, null);
	        if (rompath != null)
	        	romPref.setSummary(rompath);
	        inlinePrefCat.addPreference(romPref);    
	        
	        
	        romKeyPref = new Preference(this) {

				@Override
				protected void onClick() {
					Intent settingsIntent = new Intent();
					settingsIntent.setClass(getContext(), ImportView.class);
					settingsIntent.putExtra("import", new RomImportView("key"));
	           		startActivityForResult(settingsIntent, Globals.PREFKEY_ROMKEY_INT);
				}
	        	
	        };
	        romKeyPref.setTitle(R.string.romkey_location);
	        String romkeypath = sp.getString(Globals.PREFKEY_ROMKEY, null);
	        if (romkeypath != null)
	        	romKeyPref.setSummary(romkeypath);
	        inlinePrefCat.addPreference(romKeyPref); 
	        
	        floppy1 = new Preference(this) {

				@Override
				protected void onClick() {
					
					Intent settingsIntent = new Intent();
					settingsIntent.setClass(getContext(), ImportView.class);
					settingsIntent.putExtra("import", new FloppyImportView());
	           		startActivityForResult(settingsIntent, Globals.PREFKEY_F1_INT);
					
				}
	        	
	        };
	        rompath = sp.getString(Globals.PREFKEY_F1, null);
	        if (rompath != null)
	        	floppy1.setSummary(rompath);
	        
	         floppy1.setTitle(R.string.floppy1_location);
	        inlinePrefCat.addPreference(floppy1);
	        
	        if (rompath != null) {
	        	Preference rFloppy1 = new Preference(this) {
					@Override
					protected void onClick() {
						Editor e = sp.edit();
						e.remove(Globals.PREFKEY_F1);
						e.commit();
						floppy1.setSummary("");
					}
		        };
		        rFloppy1.setTitle(R.string.remove_floppy1);
		        inlinePrefCat.addPreference(rFloppy1);
	        }
	        
	        floppy2 = new Preference(this) {

				@Override
				protected void onClick() {
					Intent settingsIntent = new Intent();
					settingsIntent.setClass(getContext(), ImportView.class);
					settingsIntent.putExtra("import", new FloppyImportView());
	           		startActivityForResult(settingsIntent, Globals.PREFKEY_F2_INT);
				}
	        	
	        };
	        floppy2.setTitle(R.string.floppy2_location);
	        rompath = sp.getString(Globals.PREFKEY_F2, null);
	        if (rompath != null)
	        	floppy2.setSummary(rompath);
	        inlinePrefCat.addPreference(floppy2);   
	        
	        if (rompath != null) {
	        	Preference rFloppy2 = new Preference(this) {
					@Override
					protected void onClick() {
						Editor e = sp.edit();
						e.remove(Globals.PREFKEY_F2);
						e.commit();
						floppy2.setSummary("");
					}
		        };
		        rFloppy2.setTitle(R.string.remove_floppy2);
		        inlinePrefCat.addPreference(rFloppy2);
	        }
	        
	        floppy3 = new Preference(this) {

				@Override
				protected void onClick() {
					Intent settingsIntent = new Intent();
					settingsIntent.setClass(getContext(), ImportView.class);
					settingsIntent.putExtra("import", new FloppyImportView());
	           		startActivityForResult(settingsIntent, Globals.PREFKEY_F3_INT);
				}
	        	
	        };
	        floppy3.setTitle(R.string.floppy3_location);
	        rompath = sp.getString(Globals.PREFKEY_F3, null);
	        if (rompath != null)
	        	floppy3.setSummary(rompath);
	        inlinePrefCat.addPreference(floppy3);
	        
	        if (rompath != null) {
	        	Preference rFloppy3 = new Preference(this) {
					@Override
					protected void onClick() {
						Editor e = sp.edit();
						e.remove(Globals.PREFKEY_F3);
						e.commit();
						floppy3.setSummary("");
					}
		        };
		        rFloppy3.setTitle(R.string.remove_floppy3);
		        inlinePrefCat.addPreference(rFloppy3);
	        }
	        
	        floppy4 = new Preference(this) {

				@Override
				protected void onClick() {
					Intent settingsIntent = new Intent();
					settingsIntent.setClass(getContext(), ImportView.class);
					settingsIntent.putExtra("import", new FloppyImportView());
	           		startActivityForResult(settingsIntent, Globals.PREFKEY_F4_INT);
				}
	        	
	        };
	        floppy4.setTitle(R.string.floppy4_location);
	        rompath = sp.getString(Globals.PREFKEY_F4, null);
	        if (rompath != null)
	        	floppy4.setSummary(rompath);
	        inlinePrefCat.addPreference(floppy4);
	        
	        if (rompath != null) {
	        	Preference rFloppy4 = new Preference(this) {
					@Override
					protected void onClick() {
						Editor e = sp.edit();
						e.remove(Globals.PREFKEY_F4);
						e.commit();
						floppy4.setSummary("");
					}
		        };
		        rFloppy4.setTitle(R.string.remove_floppy4);
		        inlinePrefCat.addPreference(rFloppy4);
	        }
	        
	        PreferenceCategory perfPrefCat = new PreferenceCategory(this);
	        perfPrefCat.setTitle(R.string.perf_section);
	        root.addPreference(perfPrefCat);
	        
	        CheckBoxPreference toggleCyclonePref = new CheckBoxPreference(this);
	        toggleCyclonePref.setKey(Globals.PREFKEY_CYCLONE);
	        toggleCyclonePref.setTitle(R.string.cyclone_core);
	        toggleCyclonePref.setSummary(R.string.cyclone_desc);
	        toggleCyclonePref.setDefaultValue(false);
	        toggleCyclonePref.setEnabled(Build.CPU_ABI.toLowerCase().startsWith("arm"));
	        perfPrefCat.addPreference(toggleCyclonePref);
	        
	        CheckBoxPreference toggleSoundPref = new CheckBoxPreference(this);
	        toggleSoundPref.setKey(Globals.PREFKEY_SOUND);
	        toggleSoundPref.setTitle(R.string.sound);
	        toggleSoundPref.setDefaultValue(false);
	        perfPrefCat.addPreference(toggleSoundPref);
	        
	        CheckBoxPreference toggleAFSPref = new CheckBoxPreference(this);
	        toggleAFSPref.setKey(Globals.PREFKEY_AFS);
	        toggleAFSPref.setTitle(R.string.auto_frameskip);
	        toggleAFSPref.setDefaultValue(true);
	        perfPrefCat.addPreference(toggleAFSPref);
	        
	        ListPreference fs1Pref = new ListPreference(this);
	        fs1Pref.setEntries(R.array.fs_entries);
	        fs1Pref.setEntryValues(R.array.fs_entries);
	        fs1Pref.setDefaultValue("2");
	        fs1Pref.setDialogTitle(R.string.frameskip_value);
	        fs1Pref.setKey(Globals.PREFKEY_FS);
	        fs1Pref.setTitle(R.string.frameskip_value);
	        perfPrefCat.addPreference(fs1Pref);
	        
	        CheckBoxPreference toggleDSPref = new CheckBoxPreference(this);
	        toggleDSPref.setKey(Globals.PREFKEY_DRIVESTATUS);
	        toggleDSPref.setTitle(R.string.drivestatus);
	        toggleDSPref.setDefaultValue(false);
	        perfPrefCat.addPreference(toggleDSPref);
	        
	        CheckBoxPreference toggleNTSCPref = new CheckBoxPreference(this);
	        toggleNTSCPref.setKey(Globals.PREFKEY_NTSC);
	        toggleNTSCPref.setTitle(R.string.ntsc);
	        toggleNTSCPref.setDefaultValue(false);
	        perfPrefCat.addPreference(toggleNTSCPref);
	        
	        ListPreference sc1Pref = new ListPreference(this);
	        sc1Pref.setEntries(R.array.sc_entries_summary);
	        sc1Pref.setEntryValues(R.array.sc_entries);
	        sc1Pref.setDefaultValue("0");
	        sc1Pref.setDialogTitle(R.string.system_clock);
	        sc1Pref.setKey(Globals.PREFKEY_SC);
	        sc1Pref.setTitle(R.string.system_clock);
	        sc1Pref.setSummary(R.string.system_clock_summary);
	        perfPrefCat.addPreference(sc1Pref);
	        
	        ListPreference st1Pref = new ListPreference(this);
	        st1Pref.setEntries(R.array.st_entries_summary);
	        st1Pref.setEntryValues(R.array.st_entries);
	        st1Pref.setDefaultValue("0");
	        st1Pref.setDialogTitle(R.string.sync_threshold);
	        st1Pref.setKey(Globals.PREFKEY_ST);
	        st1Pref.setTitle(R.string.sync_threshold);
	        st1Pref.setSummary(R.string.system_clock_summary);
	        perfPrefCat.addPreference(st1Pref);
	        
	        PreferenceCategory portPrefCat = new PreferenceCategory(this);
	        portPrefCat.setTitle(R.string.mapping_settings);
	        root.addPreference(portPrefCat);
	        
	        ListPreference joyLayoutPref = new ListPreference(this);
	        joyLayoutPref.setEntries(R.array.joystick_layout_test);
	        joyLayoutPref.setEntryValues(R.array.joystick_layout);
	        joyLayoutPref.setDefaultValue("bottom_bottom");
	        joyLayoutPref.setDialogTitle(R.string.joystick_layout_test);
	        joyLayoutPref.setKey("vkeypadLayout");
	        joyLayoutPref.setTitle(R.string.joystick_layout_test);
	        portPrefCat.addPreference(joyLayoutPref);
	        
	        ListPreference joySizePref = new ListPreference(this);
	        joySizePref.setEntries(R.array.joystick_size);
	        joySizePref.setEntryValues(R.array.joystick_size);
	        joySizePref.setDefaultValue("medium");
	        joySizePref.setDialogTitle(R.string.joystick_layout_size);
	        joySizePref.setKey("vkeypadSize");
	        joySizePref.setTitle(R.string.joystick_layout_size);
	        portPrefCat.addPreference(joySizePref);
	        
	        ListPreference screenSizePref = new ListPreference(this);
	        screenSizePref.setEntries(R.array.screen_size);
	        screenSizePref.setEntryValues(R.array.screen_size);
	        screenSizePref.setDefaultValue("stretched");
	        screenSizePref.setDialogTitle(R.string.screen_size_text);
	        screenSizePref.setKey("scale");
	        screenSizePref.setTitle(R.string.screen_size_text);
	        portPrefCat.addPreference(screenSizePref);
	        
	        CheckBoxPreference toggleInputMethodPref = new CheckBoxPreference(this);
	        toggleInputMethodPref.setKey("useInputMethod");
	        toggleInputMethodPref.setTitle(R.string.useInputMethodTitle);
	        toggleInputMethodPref.setDefaultValue(false);
	        toggleInputMethodPref.setSummary(R.string.useInputMethod);
	        portPrefCat.addPreference(toggleInputMethodPref);
	        
	        CheckBoxPreference twoPlayers = new CheckBoxPreference(this);
	        twoPlayers.setKey("twoPlayers");
	        twoPlayers.setTitle(R.string.twoPlayersTitle);
	        twoPlayers.setDefaultValue(false);
	        twoPlayers.setSummary(R.string.twoPlayers);
	        portPrefCat.addPreference(twoPlayers);
	        
	        PreferenceScreen screenPref = getPreferenceManager().createPreferenceScreen(this);
	        screenPref.setKey("screen_preference");
	        screenPref.setTitle(R.string.custom_mappings);
	        portPrefCat.addPreference(screenPref);
	        
	        for(int i=0;i<DemoActivity.default_keycodes.length;i++) {
	        	screenPref.addPreference(new KeyPreference(this, null, DemoActivity.default_keycodes_string[i], DemoActivity.default_keycodes[i], new int [] { KeyEvent.KEYCODE_MENU }));
	        }
	        
	        setResult(RESULT_OK);
	        
	        return root;
	 }
	 
	
	 
	 @Override
	 protected void onActivityResult(final int requestCode, final int resultCode, final Intent extras) {
		 if (resultCode == RESULT_OK) {
			 String prefKey = null;
			 String path = null;
			 if (requestCode == Globals.PREFKEY_ROM_INT) {
				 prefKey = Globals.PREFKEY_ROM;
				 path = extras.getStringExtra("currentFile");
				 romPref.setSummary(path);
			 } else if (requestCode == Globals.PREFKEY_ROMKEY_INT) {
				 prefKey = Globals.PREFKEY_ROMKEY;
				 path = extras.getStringExtra("currentFile");
				 romKeyPref.setSummary(path);
			 } else if (requestCode == Globals.PREFKEY_F1_INT) {
				 prefKey = Globals.PREFKEY_F1;
				 path = extras.getStringExtra("currentFile");
				 floppy1.setSummary(path);
			 } else if (requestCode == Globals.PREFKEY_F2_INT) {
				 prefKey = Globals.PREFKEY_F2;
				 path = extras.getStringExtra("currentFile");
				 floppy2.setSummary(path);
			 } else if (requestCode == Globals.PREFKEY_F3_INT) {
				 prefKey = Globals.PREFKEY_F3;
				 path = extras.getStringExtra("currentFile");
				 floppy3.setSummary(path);
			 } else if (requestCode == Globals.PREFKEY_F4_INT) {
				 prefKey = Globals.PREFKEY_F4;
				 path = extras.getStringExtra("currentFile");
				 floppy4.setSummary(path);
			 }
			 if (prefKey != null) {
				 SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(this);
				Editor e = sp.edit();
				e.putString(prefKey, path);
				e.commit();
			 }
		 }
	 }

}
