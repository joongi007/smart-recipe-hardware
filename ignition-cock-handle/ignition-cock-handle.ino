// 전자 점화콕 손잡이
#include <SoftwareSerial.h>
#include<Wire.h>

//블루투스의 Tx, Rx핀을 2번 3번핀으로 설정
SoftwareSerial smartHandle(2, 3); 

const int MPU_ADDR = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; // 센서 값

double angleAcX, angleAcY, angleAcZ; // 가속도를 통한 각도
double angleGyX, angleGyY, angleGyZ; // 자이로를 통한 각도
double angleFiX, angleFiY, angleFiZ; // 상보 필터된 각도

const double RADIAN_TO_DEGREE = 180 / 3.14159; // 각도를 표시
const double DEGREE_PER_SECOND = 32767 / 250;  // 초당 회전하는 각도
const double ALPHA = 1 / (1 + 0.04);           // 상보필터 비중

unsigned long now = 0;  // 현재 시간
unsigned long past = 0; // 과거 시간
double dt = 0; // 루프문이 한번 돌 때 걸리는 시간

double baseAcX, baseAcY, baseAcZ;
double baseGyX, baseGyY, baseGyZ;

void setup() {
  // 각도 센서 연결을 위한 초기화
  initSensor();
  // 시리얼 통신의 속도를 9600으로 설정
//  Serial.begin(9600);
//  Serial.println("connect complete");

  // 센서의 초기값 저장
  calibrateSensor();
  past = millis();
  
  //블루투스와 아두이노의 통신속도를 9600으로 설정
  smartHandle.begin(38400);

}

int value=0, preValue=0;
enum Axis { X, Y, Z };
double nowZ;
Axis df = X; // default
int dfX=90, dfY=-90, dfZ=0; // default axis
int level_0 = 23, level_1 = 68, level_2 = 113, level_3 = 150;
unsigned long previousMillis = 0;
String inputString = "";

void loop() {
  // 현재 밀리초 저장
  unsigned long currentMillis = millis();

  getData(); // 데이터 받기
  getDT(); // 루프문 시간 구하기
  getAngleFi();
  
  // 블루투스에서 넘어온 데이터가 있다면
  while(smartHandle.available()) { 
    //시리얼모니터에 데이터를 출력
    // Serial.println(smartHandle.read());

    // 1바이트를 읽음
    char myChar = smartHandle.read();
    // 읽은 값이 \n과 \r이 아니면 inputString 에 붙여넣음
    if(myChar != '\n' && myChar != '\r') inputString += myChar;

  }
  float diff;
  switch(df){
    case X:
      diff = abs(dfX + angleFiX);
      break;
    case Y:
      diff = abs(dfY + angleFiY);
      break;
    case Z:
      delay(10);
      diff = abs(dfZ + angleFiZ);
      break;
  }
  if(diff < level_0) value = 0;
  else if(diff<level_1) value = 1;
  else if(diff<level_2) value = 2;
  else if(diff<level_3) value = 3;

  if(value != preValue){
    if(currentMillis - previousMillis > 999){
      preValue = value;
      smartHandle.write(value +'0');
//      Serial.print("변경된 단수 : ");
//      Serial.println(value);
      previousMillis = currentMillis;
    }
  }else previousMillis = currentMillis;
  
    
  

  // 블루투스에 수신된 문자열이 있을경우
  if(inputString != ""){
    // Serial.println(inputString);
    // 그 문자열이 fire? 이면 현재 불의 단수를 전송
    if(inputString == "fire?"){
      smartHandle.write(value +'0');

    // 그 문자열이 value? 이면 현재 스위치 값을 전송
    } else if(inputString.substring(0,3) == "set"){
      String t = inputString.substring(3,4);
      if(t == "X"){
        df = X;
      }else if(t == "Y"){
        df = Y;
      }else if(t == "Z"){
        angleFiZ = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
        df = Z;
      }else if(t == "D"){
        df = X;
        dfX = 90; 
        level_0 = 23;
        level_1 = 68;
        level_2 = 113;
        level_3 = 150;
      }else if(t == "0"){
        switch(df){
          case X:
            level_0 = abs(dfX + angleFiX) + 23;
            break;
          case Y:
            level_0 = abs(dfY + angleFiY) + 23;
            break;
          case Z:
            delay(10);
            level_0 = abs(dfZ + angleFiZ) + 23;
            break;
        }
      }else if(t == "1"){
         switch(df){
          case X:
            level_1 = abs(dfX + angleFiX) + 23;
            break;
          case Y:
            level_1 = abs(dfY + angleFiY) + 23;
            break;
          case Z:
            delay(10);
            level_1 = abs(dfZ + angleFiZ) + 23;
            break;
        }
      }else if(t == "2"){
        switch(df){
          case X:
            level_2 = abs(dfX + angleFiX) + 23;
            break;
          case Y:
            level_2 = abs(dfY + angleFiY) + 23;
            break;
          case Z:
            delay(10);
            level_2 = abs(dfZ + angleFiZ) + 23;
            break;
        }
      }else if(t == "3"){
        switch(df){
          case X:
            level_3 = abs(dfX + angleFiX) + 23;
            break;
          case Y:
            level_3 = abs(dfY + angleFiY) + 23;
            break;
          case Z:
            delay(10);
            level_3 = abs(dfZ + angleFiZ) + 23;
            break;
        }
      }
    }
    // 변수 초기화
    inputString = "";
  }
}


void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}

void getDT() {
  now = millis();
  dt = (now - past) / 1000.0;
  past = now;
}

void calibrateSensor() {
  double sumAcX = 0, sumAcY = 0, sumAcZ = 0;
  double sumGyX = 0, sumGyY = 0, sumGyZ = 0;
  getData();
  for (int i = 0; i < 10 ; i++) {
    getData();
    sumAcX += AcX; sumAcY += AcY; sumAcZ += AcZ;
    sumGyX += GyX; sumGyY += GyY; sumGyZ += GyZ;
    delay(50);
  }
  baseAcX = sumAcX / 10;
  baseAcY = sumAcY / 10;
  baseAcZ = sumAcZ / 10;
  baseGyX = sumGyX / 10;
  baseGyY = sumGyY / 10;
  baseGyZ = sumGyZ / 10;
}

void getAngleFi(){
  // X 각도
  angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
  angleAcX *= RADIAN_TO_DEGREE;
  angleGyX = ((GyX - baseGyX) / DEGREE_PER_SECOND) * dt;
  angleFiX = ALPHA * (angleFiX + angleGyX) + (1.0 - ALPHA) * angleAcX;
  
  // Y 각도
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
  angleGyY = ((GyY - baseGyY) / DEGREE_PER_SECOND) * dt;
  angleFiY = ALPHA * (angleFiY + angleGyY) + (1.0 - ALPHA) * angleAcY;

  // Z 각도
  angleGyZ = ((GyZ - baseGyZ) / DEGREE_PER_SECOND) * dt;
  angleFiZ += round(angleGyZ*100)/100;
}
