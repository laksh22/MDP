package com.example.mdp_maze;

import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.nio.charset.Charset;
import java.util.UUID;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

public class BluetoothConnectionService {
    private static final String TAG = "BluetoothConnection";

    private static final String appName = "Mars Rover";

    private final BluetoothAdapter mBTAdapter;
    private BluetoothDevice mBTDevice;
    private UUID mBTDeviceUUID;
    private static final UUID MY_UUID_INSECURE =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private final Context mContext;

    private AcceptThread mInsecureAcceptThread;
    private ConnectThread mConnectThread;
    private ConnectedThread mConnectedThread;

    private ProgressDialog mProgressDialog;

    public BluetoothConnectionService(Context context) {
        mContext = context;
        mBTAdapter = BluetoothAdapter.getDefaultAdapter();
        start();
    }

    private class AcceptThread extends Thread {

        private final BluetoothServerSocket mmServerSocket;

        public AcceptThread(){
            BluetoothServerSocket tmp = null;

            try{
                tmp = mBTAdapter.listenUsingInsecureRfcommWithServiceRecord(appName, MY_UUID_INSECURE);
                Log.d(TAG, "AcceptThread: Setting up Server using: " + MY_UUID_INSECURE);
            }catch (IOException e){
                Log.e(TAG, "AcceptThread: IOException: " + e.getMessage() );
            }

            mmServerSocket = tmp;
        }

        public void run(){
            Log.d(TAG, "run: AcceptThread Running.");

            BluetoothSocket bluetoothSocket = null;

            try{
                Log.d(TAG, "run: RFCOM server socket start.....");

                bluetoothSocket = mmServerSocket.accept();

                Log.d(TAG, "run: RFCOM server socket accepted connection.");
            }catch (IOException e){
                Log.e(TAG, "AcceptThread: IOException: " + e.getMessage() );
            }

            if(bluetoothSocket != null){
                connectSocket(bluetoothSocket);
            }

            Log.i(TAG, "end: AcceptThread ");
        }

        public void cancel() {
            Log.d(TAG, "cancel: Canceling AcceptThread.");
            try {
                mmServerSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "cancel: Close of AcceptThread ServerSocket failed. " + e.getMessage() );
            }
        }
    }

    private class ConnectThread extends Thread {

        private BluetoothSocket mmSocket;

        public ConnectThread(BluetoothDevice device, UUID uuid) {
            Log.d(TAG, "ConnectThread: started.");
            mBTDevice = device;
            mBTDeviceUUID = uuid;
        }

        public void run(){
            Log.i(TAG, "run: ConnectThread ");

            BluetoothSocket tmp = null;

            try {
                Log.d(TAG, "ConnectThread: Trying to create InsecureRfcommSocket using UUID: "
                        + MY_UUID_INSECURE );
                tmp = mBTDevice.createRfcommSocketToServiceRecord(mBTDeviceUUID);
            } catch (IOException e) {
                Log.e(TAG, "ConnectThread: Could not create InsecureRfcommSocket " + e.getMessage());
            }

            mmSocket = tmp;

            mBTAdapter.cancelDiscovery();

            try {
                mmSocket.connect();
                Log.d(TAG, "run: ConnectThread connectSocket.");
            } catch (IOException e) {
                try {
                    mmSocket.close();
                    Log.d(TAG, "run: ConnectThread: Closed Socket.");
                } catch (IOException e1) {
                    Log.e(TAG, "run: ConnectThread: Unable to close connection in socket " + e1.getMessage());
                }
                Log.d(TAG, "run: ConnectThread: Could not connect to UUID: " + MY_UUID_INSECURE );
            }

            if(mmSocket != null){
                connectSocket(mmSocket);
            }
        }
        public void cancel() {
            try {
                Log.d(TAG, "cancel: Closing Client Socket.");
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "cancel: close() of mmSocket in Connectthread failed. " + e.getMessage());
            }
        }
    }

    public synchronized void start() {
        Log.d(TAG, "start: BluetoothConnectionService");

        if (mConnectThread != null) {
            mConnectThread.cancel();
            mConnectThread = null;
        }

        if (mInsecureAcceptThread == null) {
            mInsecureAcceptThread = new AcceptThread();
            mInsecureAcceptThread.start();
        }
    }

    public void startClient(BluetoothDevice device,UUID uuid, Context context){
        Log.d(TAG, "startClient: Started.");

        mProgressDialog = ProgressDialog.show(context,"Connecting Bluetooth"
                ,"Please Wait...",true);

        mConnectThread = new ConnectThread(device, uuid);
        mConnectThread.start();
    }

    private class ConnectedThread extends Thread {
        private BluetoothSocket mmSocket;

        private InputStream mmInStream;
        private OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket) {
            Log.d(TAG, "ConnectedThread: Starting.");

            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try{
                mProgressDialog.dismiss();
            }catch (NullPointerException e){
                Log.e(TAG, e.getMessage());
            }

            try {
                tmpIn = mmSocket.getInputStream();
                tmpOut = mmSocket.getOutputStream();
            } catch (IOException e) {
                Log.e(TAG, "start: ConnectedThread: Unable to open I/O streams.");
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run(){
            byte[] buffer = new byte[2048];
            int bytes;

            while (true) {
                try {
                    bytes = mmInStream.read(buffer);
                    String incomingMessage = new String(buffer, 0, bytes);
                    Log.d(TAG, "InputStream: " + incomingMessage);

                    Intent incomingMessageIntent = new Intent("incomingMessage");
                    incomingMessageIntent.putExtra("theMessage", incomingMessage);
                    LocalBroadcastManager.getInstance(mContext).sendBroadcast(incomingMessageIntent);
                } catch (IOException e) {
                    Log.e(TAG, "read: Error reading Input Stream. " + e.getMessage() );
                    break;
                }
            }
        }

        public void write(byte[] bytes) {
            String text = new String(bytes, Charset.defaultCharset());
            Log.d(TAG, "write: Writing to OutputStream: " + text);
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) {
                Log.e(TAG, "write: Error writing to output stream: " + e.getMessage() );
            }
        }

        public void cancel() {
            if (mmInStream != null) {
                try {
                    mmInStream.close();
                    Log.d(TAG, "cancel: InputStream closed");
                } catch (IOException e) {
                    Log.d(TAG, "cancel: Unable to close InputStream: " + e.getMessage());
                }
                mmInStream = null;
            }

            if(mmOutStream != null){
                try{
                    mmOutStream.close();
                    Log.d(TAG, "cancel: OutputStream closed");
                } catch (IOException e) {
                    Log.d(TAG, "cancel: Unable to close OutputStream: " + e.getMessage());
                }
                mmOutStream = null;
            }

            if(mmSocket != null){
                try{
                    mmSocket.close();
                    Log.d(TAG, "cancel: Connected Thread  Bluetooth Socket closed");
                } catch (IOException e) {
                    Log.d(TAG, "cancel: Unable to close Bluetooth Socket for Connected Thread: " + e.getMessage());
                }
                mmSocket = null;
            }
        }
    }

    private void connectSocket(BluetoothSocket bluetoothSocket) {
        Log.d(TAG, "connectSocket: Starting.");

        mConnectedThread = new ConnectedThread(bluetoothSocket);
        mConnectedThread.start();
    }

    public synchronized void write(byte[] out) {
        Log.d(TAG, "write: Write Called.");

        mConnectedThread.write(out);
    }

    public synchronized void disconnectConn(){
        Log.d(TAG, "disconnectConn: Closing ConnectThread and ConnectedThread");

        if (mConnectThread != null) {
            mConnectThread.cancel();
            mConnectThread = null;
        }

        unpairBTDevice();

        if (mConnectedThread != null) {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }
    }

    private void unpairBTDevice(){
        try{
            Method m = mBTDevice.getClass()
                    .getMethod("removeBond", (Class[]) null);
            m.invoke(mBTDevice, (Object[]) null);
        } catch (Exception e){
            Log.d(TAG, "Could not unpair: " + e.getMessage());
        }
    }

    public BluetoothDevice getBTDevice() {
        return mBTDevice;
    }
}