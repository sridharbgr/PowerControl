
/*Header file include */
#include "Func_Init.h"
#include "Voltage_Read_Func.h"
#include "Current_Read_Func.h"
#include <LiquidCrystal.h>
/*********************************************************************/

/* Enum declarariom */
enum powerstatus
{
 Highhigh,Lowlow,Normal
  
};

enum powerstatus status_voltage =Normal;
/*********************************************************************/

/* define Macros */
#define RELAY_CTRL 8
#define MAXVOLATGE 250

#define MINVOLTAGE 200
#define MAXCURRENT 6.5
#define MINCURRENT 1.7
#define LCD_RS 12
#define LCD_EN 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
/*********************************************************************/

/* Declare Global variable */
static int Ctrl_Switch =HIGH;  // to control current status at startup
uint32_t calc_time;
int mVperAmp = 100; // use 100 for 20A Module and 66 for 30A Module
int Ctrlbit;
double Voltage = 0;
double Volatge_manipulation = 0;
double VRMS = 0;
double AmpsRMS = 0;
//double voltage_in;
static int status_current=2;
long int voltage_in;
static double Cycle_Time;

 // initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
LiquidCrystal lcd(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
//char buff[5];
/*********************************************************************/

/*  put your setup code here, to run once     */
void setup(){ 
 Serial.begin(9600);
 pinMode(RELAY_CTRL,OUTPUT);
 pinMode(LCD_RS,OUTPUT);
 pinMode(LCD_EN,OUTPUT);
 pinMode(LCD_D4,OUTPUT);
 pinMode(LCD_D5,OUTPUT);
 pinMode(LCD_D6,OUTPUT);
 pinMode(LCD_D7,OUTPUT);
 
 digitalWrite(RELAY_CTRL,LOW);



  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");
}
/*********************************************************************/

/*    put your main code here, to run repeatedly      */
void loop()
{  
 Voltage = Current_Read_Func();
 Volatge_manipulation = Voltage_Read_Func();
 VRMS = (Voltage/2.0) *0.707;  //root 2 is 0.707
 if((calc_time >=200))
 {
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 if(AmpsRMS >MAXCURRENT)
   {
    status_current=0;
   }
   else if(AmpsRMS <MINCURRENT )
   {
    status_current=2;
   }
   else
   {
    status_current=2;
   }
 }
 voltage_in = Volatge_manipulation;
 Cycle_Time++;
 if(Cycle_Time >=6)
  {
  if(voltage_in > MAXVOLATGE )//max voltage 241v
    {       
       status_voltage = Highhigh;        
    }
   else if(voltage_in < MINVOLTAGE )//min voltage 200v
   {
     status_voltage = Lowlow;
     
   }
   else
   {
     status_voltage = Normal; 
   }
      //Ctrl_Switch = LOW;
      Cycle_Time=0;
  }

  
  //if(((status_voltage == Highhigh) ||(status_voltage == Lowlow) ||(status_current == 0)||(status_current == 1)) &&(Ctrl_Switch == HIGH))
  if((status_voltage == Highhigh) ||(status_voltage == Lowlow) ||(status_current == 0)||(status_current == 1)) 
   //if(1)
     {
     if((status_current == 0) ||(status_current == 1) )
         {
            Ctrl_Switch=LOW; 
            Serial.println('A');
         }
         Serial.println("ALL");
      digitalWrite(RELAY_CTRL,HIGH);
      
   }
   else if ((status_voltage != Highhigh) && (status_voltage != Lowlow))
   {
      Serial.println("bye");
      Ctrl_Switch=HIGH;
      digitalWrite(RELAY_CTRL,LOW);
    }
Serial.println(status_current);
//Serial.println(Cycle_Time);
//Serial.println(RELAY_CTRL);

/*         Print a message to the LCD          */
if(voltage_in >150){
lcd.clear();
lcd.setCursor(0, 0);
 lcd.print("V=");
lcd.setCursor(3, 0); 
lcd.print(voltage_in);
//lcd.setCursor(0, 1);
lcd.setCursor(7, 0);
lcd.print("C=");
lcd.setCursor(9, 0);
lcd.print(AmpsRMS);
lcd.setCursor(0, 1);
lcd.print("Status_amp:");
lcd.setCursor(13, 1);
lcd.print(status_current);
 
//sprintf(buff,"%f",AmpsRMS);

}
else
{
lcd.setCursor(0, 0);
lcd.clear();
lcd.setCursor(0, 0);
 lcd.print("Low Power mode");

 /*        END LCD display           */
}
}

/******      Main loop function ends       *************************/








