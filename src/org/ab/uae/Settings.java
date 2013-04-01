package org.ab.uae;

import org.ab.nativelayer.DirImportView;
import org.ab.nativelayer.ImportView;
import org.ab.nativelayer.KeyPreference;

import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.view.KeyEvent;

public class Settings extends PreferenceActivity implements OnPreferenceChangeListener {
	
	 @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        setPreferenceScreen(createPreferenceHierarchy());
        
       
    }
	 
	 Preference romPref ;
	 //Preference romKeyPref ;
	 Preference hddFile ;
	 Preference hdfFile ;
	 Preference floppy1 ;
	 Preference floppy2 ;
	 Preference floppy3 ;
	 Preference floppy4 ;
	 ListPreference fsCpuModelPref;
	 ListPreference fsChipMemPref;
	 ListPreference fsFastMemPref;
	 ListPreference fsSlowMemPref;
	 ListPreference fsChipSetPref;
	 ListPreference fsCpuSpeedPref;
	 ListPreference fsFloppySpeedPref;
	 
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
	        
	        
	        /*romKeyPref = new Preference(this) {

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
	        inlinePrefCat.addPreference(romKeyPref); */
	        
	        hdfFile = new Preference(this) {

				@Override
				protected void onClick() {
					
					Intent settingsIntent = new Intent();
					settingsIntent.setClass(getContext(), ImportView.class);
					settingsIntent.putExtra("import", new RomImportView("hdf"));
	           		startActivityForResult(settingsIntent, Globals.PREFKEY_HDF_INT);
					
				}
	        	
	        };
	        rompath = sp.getString(Globals.PREFKEY_HDF, null);
	        if (rompath != null)
	        	hdfFile.setSummary(rompath);
	        
	        hdfFile.setTitle(R.string.hdf_location);
	        inlinePrefCat.addPreference(hdfFile);
	        if (rompath != null) {
	        	Preference rFloppyHDF = new Preference(this) {
					@Override
					protected void onClick() {
						Editor e = sp.edit();
						e.remove(Globals.PREFKEY_HDF);
						e.commit();
						hdfFile.setSummary("");
					}
		        };
		        rFloppyHDF.setTitle(R.string.remove_hdf);
		        inlinePrefCat.addPreference(rFloppyHDF);
	        }
	        
	        hddFile = new Preference(this) {

				@Override
				protected void onClick() {
					
					Intent settingsIntent = new Intent();
					settingsIntent.setClass(getContext(), DirImportView.class);
					settingsIntent.putExtra("import", new RomImportView("dir"));
	           		startActivityForResult(settingsIntent, Globals.PREFKEY_HDD_INT);
					
				}
	        	
	        };
	        rompath = sp.getString(Globals.PREFKEY_HDD, null);
	        if (rompath != null)
	        	hddFile.setSummary(rompath);
	        
	        hddFile.setTitle(R.string.hdd_location);
	        inlinePrefCat.addPreference(hddFile);
	        if (rompath != null) {
	        	Preference rFloppyHDD = new Preference(this) {
					@Override
					protected void onClick() {
						Editor e = sp.edit();
						e.remove(Globals.PREFKEY_HDD);
						e.commit();
						hddFile.setSummary("");
					}
		        };
		        rFloppyHDD.setTitle(R.string.remove_hdd);
		        inlinePrefCat.addPreference(rFloppyHDD);
	        }
	        
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
	        
	        CheckBoxPreference toggleSoundPref = new CheckBoxPreference(this);
	        toggleSoundPref.setKey(Globals.PREFKEY_SOUND);
	        toggleSoundPref.setTitle(R.string.sound);
	        toggleSoundPref.setDefaultValue(false);
	        perfPrefCat.addPreference(toggleSoundPref);
	        
	        /*CheckBoxPreference toggleAFSPref = new CheckBoxPreference(this);
	        toggleAFSPref.setKey(Globals.PREFKEY_AFS);
	        toggleAFSPref.setTitle(R.string.auto_frameskip);
	        toggleAFSPref.setDefaultValue(true);
	        perfPrefCat.addPreference(toggleAFSPref);*/
	        
	        ListPreference fs1Pref = new ListPreference(this);
	        fs1Pref.setEntries(R.array.fs_entries);
	        fs1Pref.setEntryValues(R.array.fs_entries);
	        fs1Pref.setDefaultValue("1");
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
	        
	        fsCpuModelPref = new ListPreference(this);
	        fsCpuModelPref.setEntries(R.array.cpu_model_summary);
	        fsCpuModelPref.setEntryValues(R.array.cpu_model_entries);
	        fsCpuModelPref.setDefaultValue("0");
	        fsCpuModelPref.setDialogTitle(R.string.cpu_model);
	        fsCpuModelPref.setKey(Globals.PREF_CPU_MODEL);
	        fsCpuModelPref.setTitle(R.string.cpu_model);
	        fsCpuModelPref.setOnPreferenceChangeListener(this);
	        perfPrefCat.addPreference(fsCpuModelPref);
	        
	        fsChipMemPref = new ListPreference(this);
	        fsChipMemPref.setEntries(R.array.mem_summary);
	        fsChipMemPref.setEntryValues(R.array.mem_entries);
	        fsChipMemPref.setDefaultValue("1");
	        fsChipMemPref.setDialogTitle(R.string.chipmem);
	        fsChipMemPref.setKey(Globals.PREF_CHIP_MEM);
	        fsChipMemPref.setTitle(R.string.chipmem);
	        fsChipMemPref.setOnPreferenceChangeListener(this);
	        perfPrefCat.addPreference(fsChipMemPref);
	        
	        fsFastMemPref = new ListPreference(this);
	        fsFastMemPref.setEntries(R.array.mem2_summary);
	        fsFastMemPref.setEntryValues(R.array.mem2_entries);
	        fsFastMemPref.setDefaultValue("0");
	        fsFastMemPref.setDialogTitle(R.string.fastmem);
	        fsFastMemPref.setKey(Globals.PREF_FAST_MEM);
	        fsFastMemPref.setTitle(R.string.fastmem);
	        fsFastMemPref.setOnPreferenceChangeListener(this);
	        perfPrefCat.addPreference(fsFastMemPref);
	        
	        fsSlowMemPref = new ListPreference(this);
	        fsSlowMemPref.setEntries(R.array.mem2_summary);
	        fsSlowMemPref.setEntryValues(R.array.mem2_entries);
	        fsSlowMemPref.setDefaultValue("0");
	        fsSlowMemPref.setDialogTitle(R.string.slowmem);
	        fsSlowMemPref.setKey(Globals.PREF_SLOW_MEM);
	        fsSlowMemPref.setTitle(R.string.slowmem);
	        fsSlowMemPref.setOnPreferenceChangeListener(this);
	        perfPrefCat.addPreference(fsSlowMemPref);
	        
	        fsChipSetPref = new ListPreference(this);
	        fsChipSetPref.setEntries(R.array.chipset_summary);
	        fsChipSetPref.setEntryValues(R.array.chipset_entries);
	        fsChipSetPref.setDefaultValue("0");
	        fsChipSetPref.setDialogTitle(R.string.chipset);
	        fsChipSetPref.setKey(Globals.PREF_CHIPSET);
	        fsChipSetPref.setTitle(R.string.chipset);
	        fsChipSetPref.setOnPreferenceChangeListener(this);
	        perfPrefCat.addPreference(fsChipSetPref);
	        
	        fsCpuSpeedPref = new ListPreference(this);
	        fsCpuSpeedPref.setEntries(R.array.cpu_speed_summary);
	        fsCpuSpeedPref.setEntryValues(R.array.cpu_speed_entries);
	        fsCpuSpeedPref.setDefaultValue("0");
	        fsCpuSpeedPref.setDialogTitle(R.string.cpu_speed);
	        fsCpuSpeedPref.setKey(Globals.PREF_CPU_SPEED);
	        fsCpuSpeedPref.setTitle(R.string.cpu_speed);
	        fsCpuSpeedPref.setOnPreferenceChangeListener(this);
	        perfPrefCat.addPreference(fsCpuSpeedPref);
	        
	        fsFloppySpeedPref = new ListPreference(this);
	        fsFloppySpeedPref.setEntries(R.array.floppy_speed_entries);
	        fsFloppySpeedPref.setEntryValues(R.array.floppy_speed_entries);
	        fsFloppySpeedPref.setDefaultValue("100");
	        fsFloppySpeedPref.setDialogTitle(R.string.floppy_speed);
	        fsFloppySpeedPref.setKey(Globals.PREF_FLOPPY_SPEED);
	        fsFloppySpeedPref.setTitle(R.string.floppy_speed);
	        fsFloppySpeedPref.setOnPreferenceChangeListener(this);
	        perfPrefCat.addPreference(fsFloppySpeedPref);
	        
	        /*ListPreference sc1Pref = new ListPreference(this);
	        sc1Pref.setEntries(R.array.sc_entries_summary);
	        sc1Pref.setEntryValues(R.array.sc_entries);
	        sc1Pref.setDefaultValue("0");
	        sc1Pref.setDialogTitle(R.string.system_clock);
	        sc1Pref.setKey(Globals.PREFKEY_SC);
	        sc1Pref.setTitle(R.string.system_clock);
	        sc1Pref.setSummary(R.string.system_clock_summary);
	        perfPrefCat.addPreference(sc1Pref);*/
	        
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
	        
	        onPreferenceChange(fsCpuModelPref, sp.getString(Globals.PREF_CPU_MODEL, "0"));
	        onPreferenceChange(fsCpuSpeedPref, sp.getString(Globals.PREF_CPU_SPEED, "0"));
	        onPreferenceChange(fsChipMemPref, sp.getString(Globals.PREF_CHIP_MEM, "1"));
	        onPreferenceChange(fsFastMemPref, sp.getString(Globals.PREF_FAST_MEM, "0"));
	        onPreferenceChange(fsSlowMemPref, sp.getString(Globals.PREF_SLOW_MEM, "0"));
	        onPreferenceChange(fsChipSetPref, sp.getString(Globals.PREF_CHIPSET, "0"));
	        onPreferenceChange(fsFloppySpeedPref, sp.getString(Globals.PREF_FLOPPY_SPEED, "100"));
	        
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
			 } /*else if (requestCode == Globals.PREFKEY_ROMKEY_INT) {
				 prefKey = Globals.PREFKEY_ROMKEY;
				 path = extras.getStringExtra("currentFile");
				 romKeyPref.setSummary(path);
			 } */else if (requestCode == Globals.PREFKEY_HDD_INT) {
				 prefKey = Globals.PREFKEY_HDD;
				 path = extras.getStringExtra("currentFile");
				 hddFile.setSummary(path);
			 } else if (requestCode == Globals.PREFKEY_HDF_INT) {
				 prefKey = Globals.PREFKEY_HDF;
				 path = extras.getStringExtra("currentFile");
				 hdfFile.setSummary(path);
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



	public boolean onPreferenceChange(Preference preference, Object newValue) {
		if (Globals.PREF_CPU_MODEL.equals(preference.getKey())) {
			int nV = Integer.parseInt(newValue.toString());
			fsCpuModelPref.setSummary(getResources().getStringArray(R.array.cpu_model_summary)[nV]);
		} else if (Globals.PREF_CPU_SPEED.equals(preference.getKey())) {
			int nV = Integer.parseInt(newValue.toString());
			fsCpuSpeedPref.setSummary(getResources().getStringArray(R.array.cpu_speed_summary)[nV]);
		} else if (Globals.PREF_CHIP_MEM.equals(preference.getKey())) {
			int nV = Integer.parseInt(newValue.toString());
			fsChipMemPref.setSummary(getResources().getStringArray(R.array.mem_summary)[nV]);
		} else if (Globals.PREF_FAST_MEM.equals(preference.getKey())) {
			int nV = Integer.parseInt(newValue.toString());
			fsFastMemPref.setSummary(getResources().getStringArray(R.array.mem2_summary)[nV]);
		} else if (Globals.PREF_SLOW_MEM.equals(preference.getKey())) {
			int nV = Integer.parseInt(newValue.toString());
			fsSlowMemPref.setSummary(getResources().getStringArray(R.array.mem2_summary)[nV]);
		} else if (Globals.PREF_CHIPSET.equals(preference.getKey())) {
			int nV = Integer.parseInt(newValue.toString());
			fsChipSetPref.setSummary(getResources().getStringArray(R.array.chipset_summary)[nV]);
		} else if (Globals.PREF_FLOPPY_SPEED.equals(preference.getKey())) {
			fsFloppySpeedPref.setSummary(newValue.toString() + "%%");
		}
		return true;
	}

}
