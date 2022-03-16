// 전자저울
#include <stdio.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

//블루투스의 Tx, Rx핀을 2번 3번핀으로 설정
SoftwareSerial mySerial(2, 3); 
LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale(4, 5);
void setLcdText(String text, int row);

void setup() {
  // 시리얼 통신의 속도를 9600으로 설정
  //시리얼통신이 연결되지 않았다면 코드 실행을 멈추고 무한 반복
  Serial.begin(9600);
  while (!Serial);
  Serial.println("connect complete");
  
  //블루투스와 아두이노의 통신속도를 9600으로 설정
  mySerial.begin(38400);

  // LCD 사용 시작
  lcd.begin();
  setLcdText(" * smart scale *",0);

  scale.set_scale(-226.0);
  scale.tare(10);

  pinMode(6, INPUT_PULLUP); // 영점조절 버튼
  pinMode(7, INPUT_PULLUP); // 전송 버튼
}

String inputString = "";
unsigned long previousMillis = 0;
char buf[16] = "";
float value=0.0;

void loop() {
  unsigned long currentMillis = millis();
  
  //블루투스에서 넘어온 데이터가 있다면
  while(mySerial.available()) { 
    //시리얼모니터에 데이터를 출력
    // Serial.println(smartHandle.read());

    // 1바이트를 읽음
    char myChar = mySerial.read();
    // 읽은 값이 \n과 \r이 아니면 inputString 에 붙여넣음
    if(myChar != '\n' && myChar != '\r') inputString += myChar;

  }
  //시리얼모니터에 입력된 데이터가 있다면
  if (Serial.available()) {   
    //블루투스를 통해 입력된 데이터 전달 
    mySerial.write(Serial.read());  
  }
  
  //  * 453.592
  value = scale.get_units();
  if(currentMillis - previousMillis > 999){
    sprintf(buf, "%14s g", String(value, 1).c_str());
    setLcdText(String(buf), 1);
    previousMillis = currentMillis;
  }
  

  // 영점 조절 버튼
  if(digitalRead(6) == 0){
    setLcdText("zero set starting", 0);
    scale.tare(10);
    setLcdText("zero set success ", 0);
  }

  // 전송 버튼
  if(digitalRead(7) == 0){
    setLcdText(" send: " + String(value, 1) +" g", 0);
    setLcdText(String(buf), 1);;
    mySerial.write(String(value, 1).c_str());
  }
  
  // 블루투스에 수신된 문자열이 있을경우
  if(inputString != ""){
    // Serial.println(inputString);
    // 그 문자열이 weight? 이면 현재 무게를 전송(g단위)
    if(inputString == "weight?"){
      mySerial.write(String(value, 1).c_str());

    // 그 문자열이 offset? 이면 무게센서의 현재 offset 값을 전송
    } else if(inputString == "offset?"){
      mySerial.write(String(scale.get_offset()).c_str());

    // 그 문자열이 disconnected 라면 현재 상태를 전달
    } else if (inputString == "disconnected"){
      setLcdText("disconnected", 0);
    }
    // 변수 초기화
    inputString = "";
  }
 }

 String firstLine = "";
 String otherLine = "";

 // lcd 컨트롤 함수
 // text: 기입할 텍스트
 // row: 0일땐 첫번째줄 이외의 숫자는 두번째줄
 void setLcdText(String text, int row){
  lcd.clear();
  if(row == 0){
    firstLine = text;
  } else {
    otherLine = text; 
  }
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(otherLine);
//  Serial.println(firstLine + "\n" + otherLine);
}
