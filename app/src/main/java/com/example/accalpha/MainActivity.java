package com.example.accalpha;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.material.snackbar.Snackbar;

import java.util.ArrayList;

import static android.content.ContentValues.TAG;

public class MainActivity extends AppCompatActivity  {
    boolean hasPermission = false;
    Button permissionButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_info);
        final Button btn_sendNotification = findViewById(R.id.btn_notify);
        btn_sendNotification.setOnClickListener( new Button.OnClickListener()
        {
            @Override
            public void onClick(final View v)
            {
                Log.i(TAG,"Clicked button");
                Snackbar.make(v,"7秒後會顯示通知",Snackbar.LENGTH_LONG).show();
                btn_sendNotification.setEnabled(false);
                new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        final String CHANNEL_ID = "testNotify";
                        NotificationChannel notifyChannel = new NotificationChannel(
                                CHANNEL_ID,"channelName", NotificationManager.IMPORTANCE_HIGH);
                        notifyChannel.setDescription("摔倒通知");
                        notifyChannel.enableLights(true);
                        notifyChannel.enableVibration(true);
                        NotificationManager notificationManager = getSystemService(NotificationManager.class);
                        notificationManager.createNotificationChannel(notifyChannel);


                        NotificationCompat.Builder builder = new NotificationCompat.Builder(v.getContext(), CHANNEL_ID)
                                .setSmallIcon(R.drawable.protobelt)
                                .setContentTitle("噯呀！")
                                .setContentText("你似乎摔倒了！")
                                .setPriority(NotificationCompat.PRIORITY_HIGH)
                                .setVisibility(NotificationCompat.VISIBILITY_PUBLIC);
                        NotificationManagerCompat notificationManagerCompat = NotificationManagerCompat.from(v.getContext());

                        // notificationId is a unique int for each notification that you must define
                        notificationManagerCompat.notify(1, builder.build());
                        Button btn = findViewById(R.id.btn_notify);
                        btn.setEnabled(true);
                    }
                }, 7000);
            }
        });
        // getActionBar return null on newer android os
        getSupportActionBar().setTitle("Finding ACC devices");
        askForPermission();
        init();
    }

    boolean askForPermission()
    {
        String[] permissions = {Manifest.permission.BLUETOOTH,Manifest.permission.BLUETOOTH_ADMIN,
                Manifest.permission.ACCESS_COARSE_LOCATION,Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE};
        for(String permission:permissions)
        {

            if(ContextCompat.checkSelfPermission(this,permission) != PackageManager.PERMISSION_GRANTED)
            {
                Log.e(TAG, "Permission error"+permission);
                requestPermissions(permissions, 1);
            }
        }
        return true;
    }

    private BluetoothLeScanner bluetoothLeScanner;
    ArrayList<String> totalListViewData = new ArrayList<String>();
    ArrayAdapter listAdapter;
    private LeDeviceListAdapter mLeDeviceListAdapter;
    private BluetoothAdapter mBluetoothAdapter;
    Handler mHandler;
    boolean mScanning;

    /**
     * 列舉裝置
     * @param device
     */
    void findDevice(BluetoothDevice device)
    {
        if(device.getName() != null)
        {
            //if(true)
            if(device.getName().startsWith("ACC"))
            {
                mBluetoothAdapter.stopLeScan(mLeScanCallback);
                final Intent intent = new Intent(this, DeviceControlActivity.class);
                intent.putExtra(DeviceControlActivity.EXTRAS_DEVICE_NAME, device.getName());
                intent.putExtra(DeviceControlActivity.EXTRAS_DEVICE_ADDRESS, device.getAddress());
                startActivity(intent);
            }
        }
    }
    BluetoothManager bluetoothManager;

    void init()
    {
        mHandler = new Handler();
        mLeDeviceListAdapter = new LeDeviceListAdapter();

        // FIXME:
        //setListAdapter(mLeDeviceListAdapter);
        bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            Log.e("","bad is null");
            finish();
            return;
        }
        scanLeDevice(true);

    }
    private BluetoothAdapter.LeScanCallback mLeScanCallback =
            new BluetoothAdapter.LeScanCallback() {

                @Override
                public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
//                            findDevice(device);
                            mLeDeviceListAdapter.addDevice(device);
                            mLeDeviceListAdapter.notifyDataSetChanged();
                        }
                    });
                }
            };


    ArrayList<String> GetDevices()
    {
        ArrayList<java.lang.String> allListViewData = new ArrayList<java.lang.String>();
        // TODO:
        allListViewData.add(null);

        return allListViewData;
    }

    private void scanLeDevice(final boolean enable) {
        if (enable) {
            // Stops scanning after a pre-defined scan period.
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mScanning = false;
                    mBluetoothAdapter.stopLeScan(mLeScanCallback);
                    invalidateOptionsMenu();
                }
            }, 1000);

            mScanning = true;
            if(mBluetoothAdapter == null)
            {
                Log.e("","Oh no");
                return;
            }
            mBluetoothAdapter.startLeScan(mLeScanCallback);
        } else {
            mScanning = false;
            mBluetoothAdapter.stopLeScan(mLeScanCallback);
        }
        invalidateOptionsMenu();
    }

    // Adapter for holding devices found through scanning.
    private class LeDeviceListAdapter extends BaseAdapter {
        private ArrayList<BluetoothDevice> mLeDevices;
        private LayoutInflater mInflator;
        public LeDeviceListAdapter() {
            super();
            mLeDevices = new ArrayList<BluetoothDevice>();
        }
        // Device scan callback.
        private BluetoothAdapter.LeScanCallback mLeScanCallback =
                new BluetoothAdapter.LeScanCallback() {

                    @Override
                    public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mLeDeviceListAdapter.addDevice(device);
                                mLeDeviceListAdapter.notifyDataSetChanged();
                            }
                        });
                    }
                };
        // TODO:
        // Filter device and stop scan
        public void addDevice(BluetoothDevice device) {
            if(device.getName() != null )
            {
                bluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();
                if(!mLeDevices.contains(device))
//                if(device.getName().startsWith("ACC") && !mLeDevices.contains(device))
                {
                    mLeDevices.add(device);
//                    // 下面是直接進入ACC
                    mScanning = false;
                    mBluetoothAdapter.stopLeScan(mLeScanCallback);
                    final Intent intent = new Intent(MainActivity.this, DeviceControlActivity.class);
                    intent.putExtra(DeviceControlActivity.EXTRAS_DEVICE_NAME, device.getName());
                    intent.putExtra(DeviceControlActivity.EXTRAS_DEVICE_ADDRESS, device.getAddress());
                    if (mScanning) {
                        mBluetoothAdapter.stopLeScan(mLeScanCallback);
                        mScanning = false;
                    }
                    Log.d(TAG,"Starting");
                  startActivity(intent);
                }
            }
        }

        public BluetoothDevice getDevice(int position) {
            return mLeDevices.get(position);
        }

        public void clear() {
            mLeDevices.clear();
        }

        @Override
        public int getCount() {
            return mLeDevices.size();
        }

        @Override
        public Object getItem(int i) {
            return mLeDevices.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            DeviceInfo.ViewHolder viewHolder;
            // General ListView optimization code.
            if (view == null) {
                view = mInflator.inflate(R.layout.listitem_device, null);
                viewHolder = new DeviceInfo.ViewHolder();
                viewHolder.deviceAddress = (TextView) view.findViewById(R.id.device_address);
                viewHolder.deviceName = (TextView) view.findViewById(R.id.device_name);
                view.setTag(viewHolder);
            } else {
                viewHolder = (DeviceInfo.ViewHolder) view.getTag();
            }

            BluetoothDevice device = mLeDevices.get(i);
            final String deviceName = device.getName();
            if (deviceName != null && deviceName.length() > 0)
                viewHolder.deviceName.setText(deviceName);
            else
                viewHolder.deviceName.setText(R.string.unknown_device);
            viewHolder.deviceAddress.setText(device.getAddress());

            return view;
        }
    }



    static class ViewHolder {
        TextView deviceName;
        TextView deviceAddress;
    }
}
