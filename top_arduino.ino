#include<LiquidCrystal.h>

LiquidCrystal LCD(9,8,5,4,3,2);
double offset=2.5;
double Sens=6.872;
double cur=0;
double tension=0;

void setup() {
  // put your setup code here, to run once:
  LCD.begin(16,2);

}

void loop() {
  // put your main code here, to run repeatedly:
double value=analogRead(A4);
double v=analogRead(A5);
v = v*90.9*5/1023;
tension = (value*5.0/1024);
cur=(tension-offset)*Sens*1.414;
LCD.clear();
LCD.setCursor(0,0);
LCD.print("Current=");
LCD.print(cur);
LCD.print("AMP");
LCD.setCursor(0,1);
LCD.print("Volatge=");
LCD.print(v);
LCD.print("V");
delay(500);
}
