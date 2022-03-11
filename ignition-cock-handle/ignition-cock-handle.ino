// 전자저울
#include <SoftwareSerial.h>
#define SSR_ONE 5
#define SSR_TWO 7
#define SSR_FOUR 9
#define SSR_EIG 11

//블루투스의 Tx, Rx핀을 2번 3번핀으로 설정
SoftwareSerial mySerial(2, 3); 

void setup() {
  // 시리얼 통신의 속도를 9600으로 설정
  Serial.begin(9600);
  //시리얼통신이 연결되지 않았다면 코드 실행을 멈추고 무한 반복
  while (!Serial);
  Serial.println("connect complete");
  
  //블루투스와 아두이노의 통신속도를 9600으로 설정
  mySerial.begin(38400);

  // 4, 5, 6, 7 핀을 input으로 설정
  for(int i = 5;i<12;i+=2){
    pinMode(i, INPUT);
  }
}

int readSwitchValue(){
  int one = digitalRead(SSR_ONE);
  int two = digitalRead(SSR_TWO);
  int four = digitalRead(SSR_FOUR);
  int eig = digitalRead(SSR_EIG);
  Serial.print(one);
  Serial.print(" ");
  Serial.print(two);
  Serial.print(" ");
  Serial.print(four);
  Serial.print(" ");
  Serial.println(eig);
  Serial.println("----------------");
  return eig * 8 + four * 4 + two * 2 + one;
}

int tmp=-1;
int value=0;

void loop() {
  //블루투스에서 넘어온 데이터가 있다면
  if (mySerial.available()) { 
    //시리얼모니터에 데이터를 출력
    Serial.write(mySerial.read()); 
  }
  //시리얼모니터에 입력된 데이터가 있다면
  if (Serial.available()) {   
    //블루투스를 통해 입력된 데이터 전달 
    mySerial.write(Serial.read());  
  }

  value = readSwitchValue();
  delay(1000);
  if(value != tmp){
    tmp = value;
//    mySerial.write(tmp);
  }

}
