// 전자 점화콕 손잡이
#include <SoftwareSerial.h>
#define SSR_ONE 4
#define SSR_TWO 5
#define SSR_FOUR 6
#define SSR_EIG 7

//블루투스의 Tx, Rx핀을 2번 3번핀으로 설정
SoftwareSerial smartHandle(2, 3); 

void setup() {
  // 시리얼 통신의 속도를 9600으로 설정
//  Serial.begin(9600);
//  Serial.println("connect complete");
  
  //블루투스와 아두이노의 통신속도를 9600으로 설정
  smartHandle.begin(38400);

  // 4, 5, 6, 7 핀을 input으로 설정
  for(int i = 4;i<8;i++){
    pinMode(i, INPUT);
  }
}

int readSwitchValue(){
  int one = digitalRead(SSR_ONE);
  int two = digitalRead(SSR_TWO);
  int four = digitalRead(SSR_FOUR);
  int eig = digitalRead(SSR_EIG);
//  Serial.print(eig);
//  Serial.print(" ");
//  Serial.print(four);
//  Serial.print(" ");
//  Serial.print(two);
//  Serial.print(" ");
//  Serial.println(one);
//  Serial.println("===================");
  return eig * 8 + four * 4 + two * 2 + one;
}

int tmp=0;
int value=0;
unsigned long previousMillis = 0;
String inputString = "";

void loop() {
  // 현재 밀리초 저장
  unsigned long currentMillis = millis();
  
  // 블루투스에서 넘어온 데이터가 있다면
  while(smartHandle.available()) { 
    //시리얼모니터에 데이터를 출력
    // Serial.println(smartHandle.read());

    // 1바이트를 읽음
    char myChar = smartHandle.read();
    // 읽은 값이 \n과 \r이 아니면 inputString 에 붙여넣음
    if(myChar != '\n' && myChar != '\r') inputString += myChar;

  }
  // 스위치에서 값을 읽어옴 이 값은 0~15 사이의 값
  value = readSwitchValue();
  
  // value의 값이 0, 2, 4, 6 인 경우만 작동
  if(value < 7){
    
  // value가 tmp에 저장된 값과 다르다면 블루투스로 값 전송
    if(value/2 != tmp/2){
      // 단수를 바꾼지 1초 이상이 된다면 블루투스로 전송.
      if(currentMillis - previousMillis > 999){
        tmp = value;
        // Serial.println(tmp/2);
        smartHandle.write(tmp/2 +'0');
      }
      
    } else previousMillis = currentMillis;
  }else previousMillis = currentMillis;

  // 블루투스에 수신된 문자열이 있을경우
  if(inputString != ""){
    // Serial.println(inputString);
    // 그 문자열이 fire? 이면 현재 불의 단수를 전송
    if(inputString == "fire?"){
      smartHandle.write(tmp/2 +'0');

    // 그 문자열이 value? 이면 현재 스위치 값을 전송
    } else if(inputString == "value?"){
      // 0~9, :, ;, <, =, >, ? 의 값이 전송될 수 있음
      smartHandle.write(value +'0');
    }
    // 변수 초기화
    inputString = "";
  }
}
