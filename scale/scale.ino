// 전자저울
#include <stdio.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// 가중치 값
#define calibration_factor 521.052
// 오차 범위(g)
#define error_range 0.2
// 표기할 소수점 이하 자리수
#define decimal_places 1
// LCD 첫번째줄 갱신 주기
#define first_line_lower_limit 2999
// LCD 두번째줄 갱신 주기
#define second_line_update_cycle 499

//블루투스의 Tx, Rx핀을 2번 3번핀으로 설정
SoftwareSerial mySerial(2, 3); 
LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale(4, 5);
void setLcdText(String text, int row);

void setup() {
  // 시리얼 통신의 속도를 9600으로 설정
  //시리얼통신이 연결되지 않았다면 코드 실행을 멈추고 무한 반복
//  Serial.begin(9600);
//  while (!Serial);
//  Serial.println("connect complete");
  
  //블루투스와 아두이노의 통신속도를 9600으로 설정
  mySerial.begin(38400);

  // LCD 사용 시작
  lcd.begin();
  setLcdText(" * smart scale *",0);
  setLcdText("           0.0 g",1);

  // scale 가중치조절
  scale.set_scale(calibration_factor);
  // scale 영점 조절
  scale.tare(10);

  pinMode(6, INPUT_PULLUP); // 영점조절 버튼
  pinMode(7, INPUT_PULLUP); // 전송 버튼
}

// 블루투스로 통신된 데이터
String inputString = "";
// LCD에 1초마다 현재 g수를 표시하기위함
unsigned long previousMillis[2] = {0, 0};
// 형식적인 문자열을 보여주기위함
char buf[16] = "";
// 이전에 기록된 센서의 값
float previousValue = 0.0;
// 현재 센서로부터 들어오는 값
float value=0.0;
// value의 텍스트 값
String valueText= "";
// 구문 통과 여부 결정 변수
bool pass = false;

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
  
  // 무게센서로 부터 값을 가져옴
  value = scale.get_units();
  // 센서가 +-0.2g의 오차를 가지므로 소프트웨어적으로 가변성 제거
  if((previousValue > value - error_range) && (previousValue < value + error_range)){
    if(value > -error_range && value < error_range) {
      value = 0;
    }else value = previousValue;
  }

  // float to string.
  valueText = String(value, decimal_places);
  
  // second_line_update_cycle와 value를 기준으로 LCD에 표시
  // value 값이 변동되면 lcd에 출력
  if(currentMillis - previousMillis[0] > second_line_update_cycle
        && value != previousValue){
    sprintf(buf, "%14s g", valueText.c_str());
    setLcdText(String(buf), 1);
    Serial.println(buf);
    previousValue = value;
    previousMillis[0] = currentMillis;
  }

  // LCD가 변경된지 first_line_lower_limit를 초과하고
  // pass가 참이면 LCD 첫번째줄 초기 텍스트로 전환
  if(currentMillis - previousMillis[1] > first_line_lower_limit
        && pass){
    setLcdText(" * smart scale *",0);
    pass = false;
  }

  // 영점 조절 버튼
  if(digitalRead(6) == 0){
    setLcdText("zero set start..", 0);
//    Serial.print("영점조절 : ");
    scale.tare(10);
//    Serial.println(scale.get_offset());
    setLcdText("zero set success ", 0);

    // 이 if문이 실행후 2초후에 lcd 화면 초기로 돌림.
    previousMillis[1] = currentMillis;
    pass = true;
  }

  // 전송 버튼
  if(digitalRead(7) == 0){
    sprintf(buf, "send: %8s g", valueText.c_str());
    setLcdText(String(buf), 0);
    sprintf(buf, "%14s g", valueText.c_str());
    setLcdText(String(buf), 1);
    
    mySerial.write(valueText.c_str());

    // 이 if문이 실행후 2초후에 lcd 화면 초기로 돌림.
    previousMillis[1] = currentMillis;
    pass = true;
  }
  
  // 블루투스에 수신된 문자열이 있을경우
  if(inputString != ""){
    // Serial.println(inputString);
    // 그 문자열이 weight? 이면 현재 무게를 전송(g단위)
    if(inputString == "weight?"){
      mySerial.write(valueText.c_str());

    // 그 문자열이 offset? 이면 무게센서의 현재 offset 값을 전송
    } else if(inputString == "offset?"){
      mySerial.write(String(scale.get_offset()).c_str());

    // 그 문자열이 disconnected 라면 현재 상태를 전달
    } else if (inputString == "disconnected"){
      setLcdText("disconnected", 0);

      // 이 if문이 실행후 2초후에 lcd 화면 초기로 돌림.
      previousMillis[1] = currentMillis;
      pass = true;
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
