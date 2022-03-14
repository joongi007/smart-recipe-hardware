package com.example.bluetest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemClock;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {
    // gui
    Button mBtnBluetoothOn;
    Button mBtnBluetoothOff;
    TextView mTvBluetoothStatus;
    TextView mTvReceiveData;
    TextView mTvSendData;
    Button mBtnConnect;
    Button mBtnSendData;
    TextView mTvReceiveData2;
    TextView mTvSendData2;
    Button mBtnConnect2;
    Button mBtnSendData2;

    BluetoothAdapter mBluetoothAdapter;
    Set<BluetoothDevice> mPairedDevices;
    List<String> mListPairedDevices;

    Handler mBluetoothHandler;
    Handler mBluetoothHandler2;
    ConnectedBluetoothThread mThreadConnectedBluetooth;
    ConnectedBluetoothThread mThreadConnectedBluetooth2;
    BluetoothDevice mBluetoothDevice;
    BluetoothSocket mBluetoothSocket;
    BluetoothSocket mBluetoothSocket2;

    final static int BT_MESSAGE_READ = 2;
    final static int BT_CONNECTING_STATUS = 3;
    final static UUID BT_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTvBluetoothStatus = findViewById(R.id.tvBluetoothStatus);
        mBtnBluetoothOn = findViewById(R.id.btnBluetoothOn);
        mBtnBluetoothOff = findViewById(R.id.btnBluetoothOff);

        mTvReceiveData = findViewById(R.id.tvReceiveData);
        mTvSendData = findViewById(R.id.tvSendData);
        mBtnConnect = findViewById(R.id.btnConnect);
        mBtnSendData = findViewById(R.id.btnSendData);

        mBtnConnect2 = findViewById(R.id.btnConnect2);
        mBtnSendData2 = findViewById(R.id.btnSendData2);
        mTvReceiveData2 = findViewById(R.id.tvReceiveData2);
        mTvSendData2 = findViewById(R.id.tvSendData2);

        // 장치가 블루투스 기능을 지원하는지 알아오기
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // 버튼이 클릭했을때의 이벤트 정의
        mBtnBluetoothOn.setOnClickListener(view -> bluetoothOn());
        mBtnBluetoothOff.setOnClickListener(view -> bluetoothOff());
        mBtnConnect.setOnClickListener(view -> listPairedDevices(1));
        mBtnSendData.setOnClickListener(view -> {
            if(mThreadConnectedBluetooth != null) {
                mThreadConnectedBluetooth.write(mTvSendData.getText().toString());
                mTvSendData.setText("");
            }
        });

        // 블루투스 핸들러로 블루투스 연결 뒤 수신된 데이터를 읽어와 ReceiveData 텍스트 뷰에 표시
        mBluetoothHandler = new Handler(Looper.getMainLooper()){
            public void handleMessage(android.os.Message msg){
                if(msg.what == BT_MESSAGE_READ){
                    String readMessage;
                    readMessage = new String((byte[]) msg.obj, StandardCharsets.UTF_8);
                    mTvReceiveData.setText(readMessage);
                }
            }
        };
        mBtnConnect2.setOnClickListener(view -> listPairedDevices(2));
        mBtnSendData2.setOnClickListener(view -> {
            if(mThreadConnectedBluetooth2 != null) {
                mThreadConnectedBluetooth2.write(mTvSendData2.getText().toString());
                mTvSendData2.setText("");
            }
        });

        // 블루투스 핸들러로 블루투스 연결 뒤 수신된 데이터를 읽어와 ReceiveData 텍스트 뷰에 표시
        mBluetoothHandler2 = new Handler(Looper.getMainLooper()){
            public void handleMessage(android.os.Message msg){
                if(msg.what == BT_MESSAGE_READ){
                    String readMessage;
                    readMessage = new String((byte[]) msg.obj, StandardCharsets.UTF_8);
                    mTvReceiveData2.setText(readMessage);
                }
            }
        };
    }

    // 블루투스 활성화 메서드
    void bluetoothOn() {
        if(mBluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(), "블루투스를 지원하지 않는 기기입니다.", Toast.LENGTH_LONG).show();
        }
        else {
            if (mBluetoothAdapter.isEnabled()) {
                Toast.makeText(getApplicationContext(), "블루투스가 이미 활성화 되어 있습니다.", Toast.LENGTH_LONG).show();
                mTvBluetoothStatus.setText("활성화");
            }
            else {
                Toast.makeText(getApplicationContext(), "블루투스가 활성화 되어 있지 않습니다.", Toast.LENGTH_LONG).show();
                Intent intentBluetoothEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityResult.launch(intentBluetoothEnable);
            }
        }
    }
    ActivityResultLauncher<Intent> startActivityResult = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (result.getResultCode() == Activity.RESULT_OK) {
                    // 블루투스가 활성화가 된다면 선택창 열기
                    Toast.makeText(getApplicationContext(), "블루투스 활성화", Toast.LENGTH_LONG).show();
                    mTvBluetoothStatus.setText("활성화");
                }else if (result.getResultCode() == Activity.RESULT_CANCELED){
                    // 블루투스가 비활성화상태
                    Toast.makeText(getApplicationContext(), "취소", Toast.LENGTH_LONG).show();
                    mTvBluetoothStatus.setText("비활성화");
                }
            });

    // 블루투스 비활성화 메소드
    @SuppressLint("MissingPermission")
    void bluetoothOff() {
        if (mBluetoothAdapter.isEnabled()) {
            mBluetoothAdapter.disable();
            Toast.makeText(getApplicationContext(), "블루투스가 비활성화 되었습니다.", Toast.LENGTH_SHORT).show();
            mTvBluetoothStatus.setText("비활성화");
        }
        else {
            Toast.makeText(getApplicationContext(), "블루투스가 이미 비활성화 되어 있습니다.", Toast.LENGTH_SHORT).show();
        }
    }

    // 블루투스 페어링 장치 목록 가져오는 메소드
    @SuppressLint("MissingPermission")
    void listPairedDevices(int type) {
        // 블루투스가 활성화 상태라면
        if (mBluetoothAdapter.isEnabled()) {
            // 페어링된 블루투스 장치 목록을 가져옴
            mPairedDevices = mBluetoothAdapter.getBondedDevices();

            // 페어링된 장치가 있다면
            if (mPairedDevices.size() > 0) {
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setTitle("장치 선택");

                // 페어링된 장치 목록
                mListPairedDevices = new ArrayList<String>();
                for (BluetoothDevice device : mPairedDevices) {
                    mListPairedDevices.add(device.getName());
                    //mListPairedDevices.add(device.getName() + "\n" + device.getAddress());
                }
                final CharSequence[] items = mListPairedDevices.toArray(new CharSequence[mListPairedDevices.size()]);
                mListPairedDevices.toArray(new CharSequence[mListPairedDevices.size()]);

                // item 선택에 따른 이벤트 정의
                builder.setItems(items, (dialog, item) -> connectSelectedDevice(items[item].toString(), type));
                AlertDialog alert = builder.create();
                alert.show();
            } else {
                Toast.makeText(getApplicationContext(), "페어링된 장치가 없습니다.", Toast.LENGTH_LONG).show();
            }
        }
        else {
            Toast.makeText(getApplicationContext(), "블루투스가 비활성화 되어 있습니다.", Toast.LENGTH_SHORT).show();
        }
    }
    // 블루투스 연결하는 메서드
    @SuppressLint("MissingPermission")
    void connectSelectedDevice(String selectedDeviceName, int type) {
        // 선택된 장치의 주소를 불러옴
        for(BluetoothDevice tempDevice : mPairedDevices) {
            if (selectedDeviceName.equals(tempDevice.getName())) {
                mBluetoothDevice = tempDevice;
                break;
            }
        }
        try {
            if(type == 1){
                // 블루투스 소캣 생성, 연결 및 쓰레드로 통신
                    mBluetoothSocket = mBluetoothDevice.createRfcommSocketToServiceRecord(BT_UUID);
                    mBluetoothSocket.connect();
                    mThreadConnectedBluetooth = new ConnectedBluetoothThread(mBluetoothSocket, mBluetoothHandler);
                    mThreadConnectedBluetooth.start();
                    mBluetoothHandler.obtainMessage(BT_CONNECTING_STATUS, 1, -1).sendToTarget();

            }else if(type == 2){
                // 블루투스 소캣 생성, 연결 및 쓰레드로 통신
                    mBluetoothSocket2 = mBluetoothDevice.createRfcommSocketToServiceRecord(BT_UUID);
                    mBluetoothSocket2.connect();
                    mThreadConnectedBluetooth2 = new ConnectedBluetoothThread(mBluetoothSocket2, mBluetoothHandler2);
                    mThreadConnectedBluetooth2.start();
                    mBluetoothHandler2.obtainMessage(BT_CONNECTING_STATUS, 1, -1).sendToTarget();
            }
        } catch (IOException e) {
            Toast.makeText(getApplicationContext(), "블루투스 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
        }

    }


    private class ConnectedBluetoothThread extends Thread {
        private final Handler mmhandler;
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        // 데이터 송수신 스트림 생성
        public ConnectedBluetoothThread(BluetoothSocket socket, Handler handler) {
            mmhandler = handler;
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "소켓 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        // 수신 처리
        public void run() {
            byte[] buffer = new byte[1024];
            int bytes;

            // 수신된 데이터가 존재한다면 데이터 처리
            while (true) {
                try {
                    bytes = mmInStream.available();
                    if (bytes != 0) {
                        SystemClock.sleep(100);
                        bytes = mmInStream.available();
                        bytes = mmInStream.read(buffer, 0, bytes);
                        mmhandler.obtainMessage(BT_MESSAGE_READ, bytes, -1, buffer).sendToTarget();
                    }
                } catch (IOException e) {
                    break;
                }
            }
        }

        // 데이터 전송을 위한 메서드
        public void write(String str) {
            byte[] bytes = str.getBytes();
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "데이터 전송 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }
        }
        // 데이터 연결 취소
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "소켓 해제 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }
        }
    }
}