#include <Wire.h>
#include <VL53L0X.h>

#define IN1_1 8  
#define IN2_1 9
#define IN3_1 10
#define IN4_1 11

#define IN1_2 4  
#define IN2_2 5
#define IN3_2 6
#define IN4_2 7

#define XSHUT_RT 3  
#define XSHUT_RS 2  

VL53L0X rt;  
VL53L0X rs;  

int stepDelay1 = 8;  
int stepDelay2 = 1;  
int platformSteps = 0; 
int hs = 0;  
int scanCount = 0;  

void setup() {
    Serial.begin(9600);
    Wire.begin();

    pinMode(XSHUT_RT, OUTPUT);
    pinMode(XSHUT_RS, OUTPUT);

    digitalWrite(XSHUT_RT, LOW);
    digitalWrite(XSHUT_RS, LOW);
    delay(10);

    digitalWrite(XSHUT_RT, HIGH);
    delay(10);
    rt.init();
    rt.setTimeout(500);
    rt.setAddress(0x30); 

    digitalWrite(XSHUT_RS, HIGH);
    delay(10);
    rs.init();
    rs.setTimeout(500);
    rs.setAddress(0x31); 

    pinMode(IN1_1, OUTPUT);
    pinMode(IN2_1, OUTPUT);
    pinMode(IN3_1, OUTPUT);
    pinMode(IN4_1, OUTPUT);

    pinMode(IN1_2, OUTPUT);
    pinMode(IN2_2, OUTPUT);
    pinMode(IN3_2, OUTPUT);
    pinMode(IN4_2, OUTPUT);
}

void stepMotor1(int step) {
    switch (step) {
        case 0: digitalWrite(IN1_1, HIGH); digitalWrite(IN2_1, LOW); digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, LOW); break;
        case 1: digitalWrite(IN1_1, HIGH); digitalWrite(IN2_1, HIGH); digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, LOW); break;
        case 2: digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, HIGH); digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, LOW); break;
        case 3: digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, HIGH); digitalWrite(IN3_1, HIGH); digitalWrite(IN4_1, LOW); break;
        case 4: digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, LOW); digitalWrite(IN3_1, HIGH); digitalWrite(IN4_1, LOW); break;
        case 5: digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, LOW); digitalWrite(IN3_1, HIGH); digitalWrite(IN4_1, HIGH); break;
        case 6: digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, LOW); digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, HIGH); break;
        case 7: digitalWrite(IN1_1, HIGH); digitalWrite(IN2_1, LOW); digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, HIGH); break;
    }
}

void stepMotor2(int step) {
    switch (step) {
        case 0: digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, LOW); digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, HIGH); break;
        case 1: digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, LOW); digitalWrite(IN3_2, HIGH); digitalWrite(IN4_2, HIGH); break;
        case 2: digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, LOW); digitalWrite(IN3_2, HIGH); digitalWrite(IN4_2, LOW); break;
        case 3: digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, HIGH); digitalWrite(IN3_2, HIGH); digitalWrite(IN4_2, LOW); break;
        case 4: digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, HIGH); digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, LOW); break;
        case 5: digitalWrite(IN1_2, HIGH); digitalWrite(IN2_2, HIGH); digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, LOW); break;
        case 6: digitalWrite(IN1_2, HIGH); digitalWrite(IN2_2, LOW); digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, LOW); break;
        case 7: digitalWrite(IN1_2, HIGH); digitalWrite(IN2_2, LOW); digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, HIGH); break;
    }
}

void loop() {
    if (scanCount >= 55) {
        while (1);  
    }

    for (int i = 0; i < 4096; i++) {
        stepMotor1(i % 8);
        platformSteps++;

        if (platformSteps % 50 == 0) {
            int rtValue = rt.readRangeSingleMillimeters();
            int rsValue = rs.readRangeSingleMillimeters();
            
            float theta = (platformSteps % 4096) * (360.0 / 4096.0);
            float radians = theta * (3.14159265358979 / 180.0);  
            
            float x = rsValue * cos(radians);
            float y = rsValue * sin(radians);
            float z;

            if (hs == 0 && (rtValue >= 164 && rtValue <= 170)) {
                z = hs;
            } else if (hs != 0 && !(rtValue >= 164 && rtValue <= 170)) {
                z = 168 - rtValue;
            } else {
                z = 168 - rtValue; 
            }

            if (x <= 175) {
                Serial.print(x);
                Serial.print(", ");
                Serial.print(y);
                Serial.print(", ");
                Serial.println(z);
            }
        }

        delay(stepDelay1);
    }
    
    hs += 2;  
    scanCount++;  

    for (int i = 0; i < 8192; i++) {
        stepMotor2(i % 8);
        delay(stepDelay2);
    }
}
