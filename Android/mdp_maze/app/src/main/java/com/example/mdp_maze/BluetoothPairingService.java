package com.example.mdp_maze;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.lang.reflect.Method;
import java.util.ArrayList;

public class BluetoothPairingService extends Activity {

    private static final String TAG = "BluetoothPairingService";

    private BluetoothAdapter mBTAdapter;

    private ListView discoveredDevicesLV;
    private ListView pairedDeviceLV;

    private Button scanBtn;
    private Button unpairBtn;

    private ArrayList<BluetoothDevice> mBTDiscoveredDevices;
    private ArrayAdapter<String> mDiscoveredDevicesArrayAdapter;

    private ArrayList<BluetoothDevice> mBTPairedDevices;
    private ArrayAdapter<String> mPairedDevicesArrayAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        setContentView(R.layout.bluetooth_pairing_layout);

        mBTAdapter = BluetoothAdapter.getDefaultAdapter();

        setResult(Activity.RESULT_CANCELED);

        discoveredDevicesLV = (ListView) findViewById(R.id.new_devices);
        pairedDeviceLV = (ListView) findViewById(R.id.paired_devices);

        mDiscoveredDevicesArrayAdapter = new ArrayAdapter<String>(this, R.layout.device_name);
        mPairedDevicesArrayAdapter = new ArrayAdapter<String>(this, R.layout.device_name);

        discoveredDevicesLV.setAdapter(mDiscoveredDevicesArrayAdapter);
        pairedDeviceLV.setAdapter(mPairedDevicesArrayAdapter);

        discoveredDevicesLV.setOnItemClickListener(mDeviceClickListener);
        pairedDeviceLV.setOnItemClickListener(mDeviceClickListener);

        scanBtn = (Button) findViewById(R.id.button_scan);
        unpairBtn = (Button) findViewById(R.id.unpair_devices);

        scanBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                doDiscovery();
            }
        });

        unpairBtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                forgetPairedDevices();
            }
        });

        IntentFilter intentFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        this.registerReceiver(mBroadcastReceiver, intentFilter);

        intentFilter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        this.registerReceiver(mBroadcastReceiver, intentFilter);

        intentFilter = new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        this.registerReceiver(mBroadcastReceiver, intentFilter);

        mBTDiscoveredDevices = new ArrayList<BluetoothDevice>();
        getPairedDevices();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if(mBTAdapter != null)
            mBTAdapter.cancelDiscovery();

        this.unregisterReceiver(mBroadcastReceiver);
    }

    private void getPairedDevices(){

        mBTPairedDevices = new ArrayList<BluetoothDevice>(mBTAdapter.getBondedDevices());
        mPairedDevicesArrayAdapter.clear();

        if(mBTPairedDevices.size() > 0){
            findViewById(R.id.title_paired_devices).setVisibility(View.VISIBLE);

            for(BluetoothDevice device : mBTPairedDevices)
                mPairedDevicesArrayAdapter.add(device.getName() + "\n" + device.getAddress());
        } else {
            findViewById(R.id.title_paired_devices).setVisibility(View.INVISIBLE);
            String noPairedDevices = getResources().getText(R.string.none_paired).toString();
            mPairedDevicesArrayAdapter.add(noPairedDevices);
        }
    }

    private void doDiscovery(){
        Log.d(TAG, "doDiscovery()");

        setProgressBarIndeterminateVisibility(true);
        setTitle(R.string.scanning);

        findViewById(R.id.title_new_devices).setVisibility(View.VISIBLE);

        if(mBTAdapter.isDiscovering())
            mBTAdapter.cancelDiscovery();

        mBTAdapter.startDiscovery();
    }

    private AdapterView.OnItemClickListener mDeviceClickListener
            = new AdapterView.OnItemClickListener(){
        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int index, long l) {

            if(mBTAdapter.isDiscovering())
                mBTAdapter.cancelDiscovery();

            String deviceInfo = ((TextView) view).getText().toString();
            String deviceMACAddress = deviceInfo.substring(deviceInfo.length() - 17);

            Log.d(TAG, deviceInfo + " clicked");

            BluetoothDevice selectedDevice = mBTAdapter.getRemoteDevice(deviceMACAddress);

            if(mBTPairedDevices.contains(selectedDevice)){
                sendPairedDevice(selectedDevice);
            } else{
                selectedDevice.createBond();
            }
        }
    };

    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "mBroadcastReceiver: onReceive()");
            String action = intent.getAction();

            if(BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice bluetoothDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                if(mBTPairedDevices.contains((BluetoothDevice) bluetoothDevice) || mBTDiscoveredDevices.contains((BluetoothDevice) bluetoothDevice))
                    return;

                if(mDiscoveredDevicesArrayAdapter.getCount() == 0){
                    mDiscoveredDevicesArrayAdapter.clear();
                }
                mBTDiscoveredDevices.add(bluetoothDevice);
                Log.d(TAG, "onReceive: " + bluetoothDevice.getName() + ": " + bluetoothDevice.getAddress());
                mDiscoveredDevicesArrayAdapter.add(bluetoothDevice.getName() + "\n" + bluetoothDevice.getAddress());

            }

            if(BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)){
                setProgressBarIndeterminateVisibility(false);
                setTitle(R.string.select_device);

                if(mDiscoveredDevicesArrayAdapter.getCount() == 0){
                    String noNewDevices = getResources().getText(R.string.none_found).toString();
                    mDiscoveredDevicesArrayAdapter.add(noNewDevices);
                }
            }

            if(action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)){
                BluetoothDevice bluetoothDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                if (bluetoothDevice.getBondState() == BluetoothDevice.BOND_BONDED){
                    Log.d(TAG, "BroadcastReceiver: BOND_BONDED.");
                    sendPairedDevice(bluetoothDevice);
                }

                if (bluetoothDevice.getBondState() == BluetoothDevice.BOND_BONDING) {
                    Log.d(TAG, "BroadcastReceiver: BOND_BONDING.");
                }

                if (bluetoothDevice.getBondState() == BluetoothDevice.BOND_NONE) {
                    Log.d(TAG, "BroadcastReceiver: BOND_NONE.");
                }
                getPairedDevices();
            }
        }
    };

    private void sendPairedDevice(BluetoothDevice bluetoothDevice){
        Intent sendPairedDeviceIntent = new Intent();
        sendPairedDeviceIntent.putExtra(BluetoothDevice.EXTRA_DEVICE, bluetoothDevice);
        setResult(Activity.RESULT_OK, sendPairedDeviceIntent);
        finish();
    }

    private void forgetPairedDevices(){
        for(BluetoothDevice pairedDevice : mBTPairedDevices)
            unpairDevice(pairedDevice);
    }

    private void unpairDevice(BluetoothDevice bluetoothDevice){
        try{
            Method m = bluetoothDevice.getClass()
                    .getMethod("removeBond", (Class[]) null);
            m.invoke(bluetoothDevice, (Object[]) null);
        } catch (Exception e){
            Log.d(TAG, "Could not unpair: " + e.getMessage());
        }
    }
}
