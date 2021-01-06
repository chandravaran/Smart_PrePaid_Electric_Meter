#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include<EEPROM.h>

uint32_t ts1 = micros();
double Sens=6.872;
int theft=0;
LiquidCrystal lcd(9,8,5,4,3,2);
SoftwareSerial mySerial(7,6);

#define relay 13
#define buzzer 12

String bal="";
float slope1 = 90.9;
float slope2 = 0.1;
int rupees=100;
float u=0;
char str[70],flag1=0,flag2=0;
unsigned int temp=0,i=0,x=0,k=0;

void setup()
{
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);


  lcd.begin(16,2);
  mySerial.begin(9600);
  Serial.begin(9600);

  lcd.setCursor(0,0);
  lcd.print("Automatic Energy");

  lcd.setCursor(0,1);
  lcd.print("      Meter    ");
  delay(500);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GSM Initializing...");

  //gsm_init();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("System Ready");

  Serial.println("AT+CNMI=2,2,0,0,0");

  init_sms();

  send_data("System Ready");

  delay(200);
  lcd.clear();
  lcd.setCursor(0,0);
  Serial.println("Setup exited");
}


void loop()
{
  theft_detection();

  String mes;

  if(theft!=1)
  {
    if(rupees<15)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("LOW Balance");
      lcd.setCursor(0,1);
      lcd.print("Recharge!!");
      delay(50);
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Balance:");
      lcd.print(rupees);//here we are displaying the balance
      lcd.print("      ");
      delay(500);
      read_pulse();
      check_status();
    }
    if(temp==1)
    {
     decode_message();
     //send_confirmation_sms();
    }
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Theft!!!");
    delay(100);
    lcd.setCursor(0,1);
    lcd.print("check connection");
    mes="theft of elecricity is occuring check your connection";
    delay(100);
    send_data(mes);
  }
}

void theft_detection()
{
  int v1,v2,i1,i2;
  v1=analogRead(A0);
  Serial.println(v1);
  v2=analogRead(A3);
  Serial.println(v2);                   //this function is used to detect if theft of electricity
  i1=analogRead(A2);
  Serial.println(i1);
  i2=analogRead(A1);
  Serial.println(i2);
  i1=((i1+3)*5.0)/1023;
  i1=(i1-2.5)*Sens*1.414;
  i2=(i2*5.0)/1023;
  i2=(i2-2.5)*Sens*1.414;
  if((v1<v2)||(i1<i2))
  {
    theft=1;
    digitalWrite(relay,HIGH);
    digitalWrite(buzzer,HIGH);
  }
}

void gsm_init()
{
  lcd.clear();
  lcd.print("Finding Module..");
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");

    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    delay(1000);
  }

  lcd.clear();
  lcd.print("Module Connected..");
  delay(1000);

  lcd.clear();
  lcd.print("Disabling ECHO");
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }

  lcd.clear();
  lcd.print("Echo OFF");
  delay(1000);

  lcd.clear();
  lcd.print("Finding Network..");
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      net_flag=0;
    }
    delay(1000);
  }

  lcd.clear();
  lcd.print("Network Found..");
  delay(1000);
  lcd.clear();
}

void init_sms()
{
   Serial.println("AT+CMGF=1");       //format of message is set to text mode
   delay(200);
   Serial.println("AT+CMGS=\"+918287114222\"");    //sends to the network provider
   delay(200);
}

void send_data(String message)
{                                       //The below part is the actual code for sms but for simualtion purposes we have used the beside code
   Serial.println(message);            //mySerial.println("AT+CMGF=1");
  delay(200);                          //delay(1000);  // Delay of 1 second
                                      //mySerial.println("AT+CMGS=\"+918095179909\"\r"); // Replace x with mobile number
                                      //mySerial.println(message);// The SMS text you want to sen
}

void serialEvent()
{
  while(Serial.available())         //if something is there to be read
  {
    Serial.println("Enter command");
   // delay(500);
    char ch=(char)Serial.read();
    str[i++]=ch;      //adds the new character to the string
    if(ch == '*')    //* will basically clear the lcd
    {
      temp=1;
      lcd.clear();
      lcd.print("Message Received");
      delay(500);
      break;
    }
  }
}

void read_pulse()
{
      uint32_t ts2 = micros();
      uint32_t tsf = ts2 - ts1;
      float r;
      float power,units,I1,If;

      float V = analogRead(A0);
      float I = analogRead(A1);
      I1=(I*5.0)/1023;
      If=(I1-2.5)*Sens*1.414;


      V = V*slope1*5/1023;

      power = V * If;
      units = power * tsf;

      r=(units-u)*0.05;
      u+=(units-u);
      rupees=rupees-r;
      Serial.print("After deduction:");
      Serial.println(rupees);
      delay(2000);


    }

void check_status()
{
      if(rupees>15)
      {
        digitalWrite(relay, LOW);
        flag1=0;
        flag2=0;
      }
      if(rupees<15 && flag1==0)
      {
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("LOW Balance");                           //initialising necessary condn (phone number) to send the message.
       send_data("Energy Meter Balance Alert:");
       digitalWrite(relay, HIGH);
       digitalWrite(buzzer,HIGH);
       delay(500);                                         //send alert to the gsm module too

       lcd.clear();
       lcd.setCursor(0,0);
       Serial.println("Current Balance = ");
       Serial.println(rupees);
       delay(200);

       send_data("Please recharge your energy meter soon.\n Thank you");

       message_sent();
       flag1=1;
      }
      if(rupees<5 && flag2==0)
     {
      digitalWrite(relay, LOW);
      lcd.clear();
      lcd.print("Light Cut Due to");
      lcd.setCursor(0,1);
      lcd.print("Low Balance");
      delay(500);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Please Recharge ");
      lcd.setCursor(0,1);
      lcd.print("UR Energy Meter ");
      send_data("Energy Meter Balance Alert:\nLight cut due to low Balance\nPlease recharge your energy meter soon.\n Thank you");
      message_sent();
      flag2=1;
    }
}


void decode_message()
{
  x=0,k=0,temp=0;
     while(x<i)
     {
      while(str[x]=='#')
      {
        x++;
        bal="";
        while(str[x]!='*')          //the message sent by the user is getting decoded
        {
          bal+=str[x++];
        }
      }
      x++;
    }
    bal+='\0';
}

void send_confirmation_sms()
{
    int recharge_amount = bal.toInt();
    rupees+=recharge_amount;
    lcd.clear();
    lcd.print("Energy Meter ");
    lcd.setCursor(0,1);
    lcd.print("Recharged:");
    lcd.print(recharge_amount);
    send_data("Energy Meter Balance Alert:\nYour energy meter has been recharged Rs:");
    send_data(bal);
    send_data("Total Balance:");
    Serial.println(rupees);
    delay(200);
    send_data("Eelctricity Has Been Connected\nThank you");
    temp=0;
    i=0;
    x=0;
    k=0;
    delay(1000);
    message_sent();
}

void message_sent()
{
  lcd.clear();
  lcd.print("Message Sent.");
  delay(1000);
}
