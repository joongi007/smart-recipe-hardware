package com.example.bluetest;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {
    BluetoothAdapter bluetoothAdapter;
    Set<BluetoothDevice> pairedDevices;
    int pairedDeviceCount;
    BluetoothDevice mRemoteDevice;
    BluetoothSocket mSocket;
    InputStream mInputStream;
    Thread mThread;
    int readBufferPositon;      //버퍼 내 수신 문자 저장 위치
    byte[] readBuffer;      //수신 버퍼
    byte mDelimiter = 10;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button btn= findViewById(R.id.button_001);
        btn.setOnClickListener(view->finish());
        btn = findViewById(R.id.button_002);
        btn.setOnClickListener(view -> checkBluetooth());
    }
    @SuppressLint("MissingPermission")
    public void checkBluetooth() {
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            // 장치가 블루투스 지원하지 않는 경우
            Toast.makeText(this, "이 장치에서는 블루투스 연결이 불가능합니다.", Toast.LENGTH_LONG).show();
        } else {
            // 장치가 블루투스 지원하는 경우
            if (!bluetoothAdapter.isEnabled()) {
                // 블루투스를 지원하지만 비활성 상태인 경우
                // 블루투스를 활성 상태로 바꾸기 위해 사용자 동의 요첨
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityResult.launch(enableBtIntent);
            } else {
                selectDevice();
            }
        }
    }
    ActivityResultLauncher<Intent> startActivityResult = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (result.getResultCode() == Activity.RESULT_OK) {
                    // 블루투스가 활성화가 된다면 선택창 열기
                    selectDevice();
                }else if (result.getResultCode() == Activity.RESULT_CANCELED){
                    // 블루투스가 비활성화상태
                    Toast.makeText(this, "블루투스에 대한 권한이 없습니다.", Toast.LENGTH_LONG).show();
                }
            });



    @SuppressLint("MissingPermission")
    public void selectDevice(){
        pairedDevices = bluetoothAdapter.getBondedDevices();
        pairedDeviceCount = pairedDevices.size();

        if (pairedDevices.size() > 0) {
            // 페어링된 장치가 있는 경우
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("블루투스 장치 선택");

            // 페어링 된 블루투스 장치의 이름 목록 작성
            List<String> listItems = new ArrayList<String>();
            for(BluetoothDevice device : pairedDevices) {
                listItems.add(device.getName());
            }
            listItems.add("취소");    // 취소 항목 추가

            final CharSequence[] items = listItems.toArray(new CharSequence[listItems.size()]);
            builder.setItems(items, (dialog, item) -> {
                if (item == listItems.size() - 1) {
                    // 연결할 장치를 선택하지 않고 '취소'를 누른 경우

                } else {
                    // 연결할 장치를 선택한 경우
                    // 선택한 장치와 연결을 시도함
                    connectToSelectedDevices(items[item].toString());
                }
            });
            AlertDialog alert = builder.create();
            alert.show();
        } else{
            // 페어링된 장치가 없는 경우
            Toast.makeText(this, "현재 연결할 수 있는 장치가 존재하지 않습니다.", Toast.LENGTH_LONG).show();
        }
    }

    @SuppressLint("MissingPermission")
    BluetoothDevice getDeviceFromBondedList(String name) {
        BluetoothDevice selectedDevice = null;
        for(BluetoothDevice device : pairedDevices) {
            if(name.equals(device.getName())) {
                selectedDevice = device;
                break;
            }
        }
        Log.d("도달", "장치이름: " + selectedDevice.getName() + " 장치선택 성공");
        return selectedDevice;
    }

    @SuppressLint("MissingPermission")
    void connectToSelectedDevices(String selectedDeviceName) {
        mRemoteDevice = getDeviceFromBondedList(selectedDeviceName);
        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
        try {
            // 소켓 생성
            mSocket = mRemoteDevice.createRfcommSocketToServiceRecord(uuid);
            Log.d("도달", "소캣 생성 성공");

            // RFCOMM 채널을 통한 연결
            mSocket.connect();
            Log.d("도달", "소캣 연결 성공");

            // 데이터 송수신을 위한 스트림 열기
            mInputStream = mSocket.getInputStream();
            Log.d("도달", "스크림 열기 성공");

            // 데이터 수신 준비
            beginListenForData();
            Log.d("도달", "데이터 수신 준비 성공");
        }catch(Exception e) {
            // 블루투스 연결 중 오류 발생
            Log.d("도달", "블루투스연결 중 오류발생");
        }
    }

    protected void onDestory() {
        try {
            mThread.interrupt();   // 데이터 수신 쓰레드 종료
            mInputStream.close();
            mSocket.close();
        } catch(Exception e) {
            e.printStackTrace();
        }
        Log.d("도달", "활동종료 도달");
        super.onDestroy();
    }

    void beginListenForData()
    {
        final Handler handler = new Handler();

        readBuffer = new byte[1024] ;  //  수신 버퍼
        readBufferPositon = 0;        //   버퍼 내 수신 문자 저장 위치
        Log.d("도달", "beginListenForData 실행 성공");
        // 문자열 수신 쓰레드
        mThread = new Thread(() -> {
            while(!Thread.currentThread().isInterrupted()){

                try {
                    int bytesAvailable = mInputStream.available();    // 수신 데이터 확인
                    if(bytesAvailable > 0) {                     // 데이터가 수신된 경우
                        byte[] packetBytes = new byte[bytesAvailable];
                        mInputStream.read(packetBytes);
                        for(int i=0 ; i<bytesAvailable; i++) {
                            byte b = packetBytes[i];
                            if(b == mDelimiter) {
                                byte[] encodedBytes = new byte[readBufferPositon];
                                System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                final String data = new String(encodedBytes, StandardCharsets.US_ASCII);
                                readBufferPositon = 0;
                                handler.post(new Runnable() {
                                    public void run() {
                                        // 수신된 문자열 데이터에 대한 처리 작업

                                    }
                                });
                            }
                            else {
                                readBuffer[readBufferPositon++] = b;
                            }
                        }
                    }
                }
                catch (IOException e) {
                    // 데이터 수신 중 오류 발생.
                    Log.d("도달", "beginListenForData의 catch에서 에러발생");
                    e.printStackTrace();
                }
            }
        });
        Log.d("도달", "수신 쓰래드 정의 성공");
        mThread.start();
        Log.d("도달", "수신스레드 성공적으로 작동중");
    }
}