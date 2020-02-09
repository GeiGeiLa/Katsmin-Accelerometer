package com.example.accalpha.DataDisplayTabs;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.fragment.app.Fragment;

import com.example.accalpha.R;

public class BroadcastRecieveActivity extends Fragment {
    //    @Override
//    public void onAttach(Activity activity)
//    {
//        super.onAttach(activity);
//
//    }
    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        Activity a;
        if (context instanceof Activity) {
            a = (Activity) context;
        }
    }


    @Override
    public View onCreateView(LayoutInflater inflator, ViewGroup container,
                             Bundle saveInstanceState) {

        View rootView = inflator.inflate(R.layout.activity_data_display, container, false);
        return rootView;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);

    }
}
