package com.example.mdp_maze;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.GridView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.mdp_maze.Adapters.GridAxisAdapter;
import com.example.mdp_maze.Adapters.GridViewAdapter;

import org.json.JSONObject;

import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.UUID;

import androidx.appcompat.app.AppCompatActivity;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import androidx.preference.PreferenceManager;

public class MainActivity extends AppCompatActivity implements SharedPreferences.OnSharedPreferenceChangeListener {
    private static final String TAG = "MainActivity";

    private static final int NUM_ROWS = 20;
    private static final int NUM_COLS = 15;

    private static final int REQUEST_ENABLE_BT = 0;
    private static final int REQUEST_DEVICE_CONNECT_INSECURE = 1;
    public static final int REQUEST_COORDINATES = 2;

    private Handler mHandler;

    private GridView mGridView;
    private GridView mXAxis;
    private GridView mYAxis;

    private GridViewAdapter mGridViewAdapter;
    private GridAxisAdapter mXAxisAdapter;
    private GridAxisAdapter mYAxisAdapter;

    private Button mGridUpdateBtn;

    private String mF1String;
    private String mF2String;
    private String mWayPointXCoord;
    private String mWayPointYCoord;
    private String mStartCoordinateXCoord;
    private String mStartCoordinateYCoord;
    private String mDescriptorStringOne;
    private String mDescriptorStringTwo;
    private String mExplorationTime;
    private String mFastestPathTime;

    private TextView mConnDeviceTV;
    private TextView mWayPointXCoordTV;
    private TextView mWayPointYCoordTV;
    private TextView mStartCoordinateXCoordTV;
    private TextView mStartCoordinateYCoordTV;
    private TextView mExplorationTimeTV;
    private TextView mFastestPathTimeTV;
    //    private TextView mRobotStatusTV;

    private BluetoothAdapter mBTAdapter;
    private BluetoothDevice mBTDevice;
    private static final UUID MY_UUID_INSECURE = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private SensorManager sensorManager;
    private Sensor sensor;


    private Toast mToast;
    private Boolean isAuto = true;
    private TextView mConnectTV;
    private TextView mAutoTV;

    private ArrayList<Character> mMapDescriptor;
    private static final String DEFAULT_MAP_DESCRIPTOR_STRING = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000434000000000000444000000000000444000000000000";



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mMapDescriptor = Utils.getMapDescriptor(DEFAULT_MAP_DESCRIPTOR_STRING);

        mStartCoordinateXCoord = "0";
        mStartCoordinateYCoord = "19";
        mWayPointXCoord = "_";
        mWayPointYCoord = "_";

        mGridView = (GridView) findViewById(R.id.maze);
        mGridViewAdapter = new GridViewAdapter(MainActivity.this, NUM_ROWS, NUM_COLS, mMapDescriptor);

        mXAxis = (GridView) findViewById(R.id.x_axis);
        mYAxis = (GridView) findViewById(R.id.y_axis);

        mGridView.setNumColumns(NUM_COLS);
        mXAxis.setNumColumns(NUM_COLS);
        mYAxis.setNumColumns(1);

        mXAxisAdapter = new GridAxisAdapter(MainActivity.this, NUM_COLS);
        mYAxisAdapter = new GridAxisAdapter(MainActivity.this, NUM_ROWS);

        mGridView.setAdapter(mGridViewAdapter);
        mXAxis.setAdapter(mXAxisAdapter);
        mYAxis.setAdapter(mYAxisAdapter);

        mConnDeviceTV = (TextView) findViewById(R.id.connected_device);
        mWayPointXCoordTV = (TextView) findViewById(R.id.waypoint_x);
        mWayPointYCoordTV = (TextView) findViewById(R.id.waypoint_y);
        mStartCoordinateXCoordTV = (TextView) findViewById(R.id.start_coordinate_x);
        mStartCoordinateYCoordTV = (TextView) findViewById(R.id.start_coordinate_y);
        mExplorationTimeTV = (TextView) findViewById(R.id.exploration_time_tv);
        mFastestPathTimeTV = (TextView) findViewById(R.id.fastest_path_time_tv);
        //        mRobotStatusTV = (TextView) findViewById(R.id.robot_status);

        updateStartCoordinatesTV();
        updateWayPointTV();

        mGridUpdateBtn = (Button) findViewById(R.id.maze_update);

//        mConnectTV = (TextView) findViewById(R.id.tv_connect_btn);
//        mAutoTV = (TextView) findViewById(R.id.tv_auto_btn);

        mGridUpdateBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mGridViewAdapter.refreshMap(mMapDescriptor);
            }
        });

        setupPreferenceStrings();

        LocalBroadcastManager.getInstance(this)
                .registerReceiver(mIncomingMessageReceiver, new IntentFilter("incomingMessage"));

        IntentFilter intentFilter = new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        this.registerReceiver(mBroadcastReceiver, intentFilter);

    }

    private BroadcastReceiver mIncomingMessageReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String incomingMessage = intent.getStringExtra("theMessage");
            incomingMessage = incomingMessage.substring(4, incomingMessage.length());

            JSONObject messageJSON = Utils.getJSONObject(incomingMessage);
            String mazeString = Utils.getJSONString(messageJSON, "m");

            if(!Utils.getJSONString(messageJSON, "d").equals(""))
                mDescriptorStringOne = Utils.getJSONString(messageJSON, "d");
            if(!Utils.getJSONString(messageJSON, "e").equals(""))
                mDescriptorStringTwo = Utils.getJSONString(messageJSON, "e");

            mMapDescriptor = Utils.getMapDescriptor(mazeString);
            if(isAuto && mMapDescriptor != null){
                mGridViewAdapter.refreshMap(mMapDescriptor);
            }
        }
    };

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)) {
                BluetoothDevice bluetoothDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                if (bluetoothDevice.getBondState() == BluetoothDevice.BOND_BONDED) {
                    Log.d(TAG, "BroadcastReceiver: BOND_BONDED.");
                    mBTDevice = bluetoothDevice;
                    updateConnTV();
                }

                if (bluetoothDevice.getBondState() == BluetoothDevice.BOND_BONDING) {
                    Log.d(TAG, "BroadcastReceiver: BOND_BONDING.");
                }

                if (bluetoothDevice.getBondState() == BluetoothDevice.BOND_NONE) {
                    Log.d(TAG, "BroadcastReceiver: BOND_NONE.");
                    if (bluetoothDevice.equals(mBTDevice)) {
                        mBTDevice = null;
                        updateConnTV();
                    }
                }
            }
        }
    };

    private void setupPreferenceStrings() {

        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);

        mF1String = sharedPreferences.getString(getString(R.string.string_f1_key), getString(R.string.defaultValue_f1));
        mF2String = sharedPreferences.getString(getString(R.string.string_f2_key), getString(R.string.defaultValue_f2));

        sharedPreferences.registerOnSharedPreferenceChangeListener(this);
    }

    private void updateWayPointTV() {
        mWayPointXCoordTV.setText("X: " + mWayPointXCoord);
        mWayPointYCoordTV.setText("Y: " + mWayPointYCoord);
    }

    private void updateStartCoordinatesTV() {
        mStartCoordinateXCoordTV.setText("X: " + mStartCoordinateXCoord);
        mStartCoordinateYCoordTV.setText("Y: " + mStartCoordinateYCoord);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater= getMenuInflater();
        inflater.inflate(R.menu.example_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case R.id.item1:
                if (mBTDevice == null) {
                    Intent bluetoothConnectIntent = new Intent(MainActivity.this, BluetoothPairingService.class);
                    startActivityForResult(bluetoothConnectIntent, REQUEST_DEVICE_CONNECT_INSECURE);
                } else {
                    stopBTConnection();
                }
                return true;


            case R.id.make_discoverable:
                Toast.makeText(this, "Make Discoverable is selected", Toast.LENGTH_SHORT).show();
                return true;

            case R.id.set_robot_position:
                Toast.makeText(this, "set robot position", Toast.LENGTH_SHORT).show();
                return true;

            case R.id.set_strings:
                Intent preferenceIntent = new Intent(this, PreferencesActivity.class);
                startActivity(preferenceIntent);
                return true;


            default: return super.onOptionsItemSelected(item);
        }

    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        if (key.equals(getString(R.string.string_f1_key))) {
            mF1String = sharedPreferences.getString(getString(R.string.string_f1_key), getString(R.string.defaultValue_f1));
        }
        if (key.equals(getString(R.string.string_f2_key))) {
            mF2String = sharedPreferences.getString(getString(R.string.string_f2_key), getString(R.string.defaultValue_f2));
        }
    }

    public void stopBTConnection() {
        if (mBTDevice == null) {
            Toast.makeText(this, "No Bluetooth Device Paired", Toast.LENGTH_SHORT);
        } else {
            ((BluetoothDelegate) this.getApplicationContext()).appBluetoothConnectionService.disconnectConn();
        }
    }

    private void writeOnOutputStream(String message) {
        if (mBTDevice == null) {
            Toast.makeText(this, "No Bluetooth Device Connected", Toast.LENGTH_SHORT).show();
            return;
        } else {
            byte[] bytes = message.toString().getBytes(Charset.defaultCharset());
            ((BluetoothDelegate) this.getApplicationContext()).appBluetoothConnectionService.write(bytes);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();

//        mBTAdapter = BluetoothAdapter.getDefaultAdapter();
//
//        if (!mBTAdapter.isEnabled()) {
//            Intent enableBTIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
//            startActivityForResult(enableBTIntent, REQUEST_ENABLE_BT);
//        }
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent receivedIntent) {
        super.onActivityResult(requestCode, resultCode, receivedIntent);
        switch (requestCode) {
            case REQUEST_ENABLE_BT: {
                if (resultCode != Activity.RESULT_OK) {
                    Toast.makeText(this, "Could not enable Bluetooth.", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(this, "Bluetooth enabled.", Toast.LENGTH_SHORT).show();
                }

                break;
            }

            case REQUEST_DEVICE_CONNECT_INSECURE: {
                if (resultCode == Activity.RESULT_OK) {
//                    mConnectTV.setText(R.string.disconnect);
                    initializeNewBTDevice(receivedIntent);
                } else {
//                    mConnectTV.setText(R.string.connect);
                }
                break;
            }

            case REQUEST_COORDINATES: {
                if (resultCode == Activity.RESULT_OK) {
                    int xCoord = receivedIntent.getIntExtra("X", 0);
                    int yCoord = receivedIntent.getIntExtra("Y", 0);
                    String type = receivedIntent.getStringExtra("TYPE");

                    Log.d(TAG, "X: " + xCoord + ", Y: " + yCoord + ", Type: " + type);

                    if (type.equals("wayPoint")) {
                        mWayPointXCoord = Integer.toString(xCoord);
                        mWayPointYCoord = Integer.toString(yCoord);

                        sendWayPoint(xCoord, yCoord);
                        updateWayPointTV();
                    } else {
                        mStartCoordinateXCoord = Integer.toString(xCoord);
                        mStartCoordinateYCoord = Integer.toString(yCoord);
                        sendStartCoordinates(xCoord, yCoord);
                        updateStartCoordinatesTV();
                    }
                }
            }
        }
    }

    private void sendStartCoordinates(int x, int y) {
        writeOnOutputStream("d(" + Integer.toString(x) + ", " + Integer.toString(y) + ")");
    }

    private void sendWayPoint(int x, int y) {
        writeOnOutputStream("c(" + Integer.toString(x) + ", " + Integer.toString(y) + ")");
    }




    private void initializeNewBTDevice(Intent intent) {
        mBTDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        startBTConnection(mBTDevice);
        updateConnTV();
    }

    private void updateConnTV() {
        if (mBTDevice != null) {
            mConnDeviceTV.setText("Connected Device: " + mBTDevice.getName().toString());
            mConnDeviceTV.setVisibility(View.VISIBLE);
        } else {
            mConnDeviceTV.setText("Connected Device:");
            mConnDeviceTV.setVisibility(View.INVISIBLE);
        }
    }

    public void startBTConnection(BluetoothDevice bluetoothDevice){
        Log.d(TAG, "startBTConnection: Initializing RFCOM Bluetooth Connection.");
        ((BluetoothDelegate)this.getApplicationContext()).appBluetoothConnectionService
                .startClient(bluetoothDevice, MY_UUID_INSECURE, this);
    }




}
