package com.example.mdp_maze;

import android.os.Bundle;

import androidx.preference.PreferenceFragmentCompat;


public class PreferencesFragment extends PreferenceFragmentCompat {
    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.string_preferences);
    }
}
