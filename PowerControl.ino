
/*Header file include */
#include "Func_Init.h"
#include "Voltage_Read_Func.h"
#include "Current_Read_Func.h"
#include <LiquidCrystal.h>
/*********************************************************************/

/* Enum declarariom */
enum powerstatus
{
  low,
Normal,
 High,
 Abnormal,
 Endoflist,
};

enum powerstatus status_voltage =Normal;
/*********************************************************************/

/* define Macros */
#define RELAY_CTRL 8
#define MAXVOLATGE 250
#define MINVOLTAGE 200
#define MAXCURRENT 6.25


#define NORMAL_VOLT 257
#define LOW_VOLT 200
#define MAX_CURRENT_TOLERANCE 0.25

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
static int status_current=1;
long int voltage_in;
//static double Cycle_Time;
int ct=0;
static int SPIK_FLAG,Spike_Init_Flag,High_Current_Set_Flag=0,Max_Curr_Delay=0,CycleTime=0,counter;
volatile int seconds = 0; //make it volatile because it is used inside the interrupt
double Current_Value= 0;
double Volatge_Value = 0;


 // initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
LiquidCrystal lcd(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
//char buff[5];
/*********************************************************************/
int i;
ISR(TIMER1_COMPA_vect)
{ //Interrupt Service Routine, Timer/Counter1 Compare Match A
  seconds++;
  if(seconds >= 45) 
  { 
    
    //set to however many seconds you want  example: 2 seconds x 30 times = 60 seconds (1 minute)          
    seconds = 0;    // after '60' seconds clear the flag
    Max_Curr_Delay=1;
   // Serial.println("90s over");
    if(CycleTime <3 )
    {
    digitalWrite(RELAY_CTRL,LOW);
    //delay(30000);
    for(i=0;i<500;i++)
    {
      Serial.println("");
    }
    }
    //Current_Value = Current_Read_Func(); //Read current
 }
}



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
  lcd.begin(16, 2);
 digitalWrite(RELAY_CTRL,LOW);
 // set up the LCD's number of columns and rows:
 
  ////////////////////Timer Initialization//////////////////////////////////
 
// //Interupt Service Routine and timer setup
 // noInterrupts();// kill interrupts until everybody is set up
  //We use Timer 1 b/c it's the only 16 bit timer
  TCCR1A = B00000000;//Register A all 0's since we're not toggling any pins
  TCCR1B = 0;//bit 3 set for CTC mode, will call interrupt on counter match, bit 2 set to divide clock by 1024, so 16MHz/1024=15625Hz
  TIMSK1 = 0;//bit 1 set to call the interrupt on an OCR1A match
  OCR1A  = 0 ;//15625Hz / 0.5 frequency in Hz


//////////////////// End of Timer Initialization//////////////////////////////////

}
/*********************************************************************/

/*    put your main code here, to run repeatedly      */
void loop()
{  
//  if(Max_Curr_Delay ==1)
//  {
//    Serial.println(++ct);
//    delay(300);
//  }
 Volatge_Value = Voltage_Read_Func(); //Read volt
  Current_Value = Current_Read_Func(); //Read current
 VRMS = (Current_Value/2.0) *0.707;  //root 2 is 0.707
 
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 
////////////////////VOLTAGE DECISION CODE///////////////////////////////////////////

  if((Volatge_Value > NORMAL_VOLT) || (Volatge_Value < LOW_VOLT))//max voltage 250
    {       
       status_voltage = Abnormal;      //enum value 2-  
    }

   else
   {
     status_voltage = Normal; // Enum value 0
   }
    
 counter++;
 //Serial.print("counter");
 // Serial.println(counter);
///////////////////END OF VOLTAGE DECISION////////////////////////////////////////////////
 
/////////////////////RunTime Maximum Current Calculation////////////////////////////
 if(counter>5)
 {
  //Serial.println("hi");
 if((AmpsRMS >(MAXCURRENT+MAX_CURRENT_TOLERANCE))||(AmpsRMS >(MAXCURRENT-MAX_CURRENT_TOLERANCE)))
   {
    status_current=High;
    //Serial.println("HIGH current");
   }
   counter=0;
 //else
   //{
    //if(AmpsRMS > 1.0)
    //{
    //status_current=Normal;
   // Serial.println("LOW current");
    //Serial.println("print5");
    //}
    //else
    //{
      //status_current=low; 
    //}
   
   }
//   Serial.print("Max_Curr_Delay");
//    Serial.println(Max_Curr_Delay);
/////////////////End Of Maximum Current Calculation///////////////////////////////////

 //////////////VOLTAGE ABNROMAL////////////////
  if(status_voltage == Abnormal) 
   {
      digitalWrite(RELAY_CTRL,HIGH);
      //Serial.println("print1");
      //Serial.println("Voltage abnormal");
   } 

//////////////CURRENT HIGH////////////////
   if (status_current== High)
   {
    if(High_Current_Set_Flag==0)
    {
      //Serial.println("1st current abnormal");
      //Serial.println("timer start");
     digitalWrite(RELAY_CTRL,HIGH);
     //Serial.println("print2");
     High_Current_Set_Flag=1;
      TCCR1B = B00001101;
      TIMSK1 = B00000010;
      OCR1A  = (unsigned long)31250UL ;
      interrupts();//restart interrupts
    }
   }
  if ((Max_Curr_Delay==1)&&(status_current!=High)&& (CycleTime <3))
   {
    TCCR1B = 0;
      TIMSK1 = 0;
      OCR1A  = 0 ;
      // noInterrupts();
       Max_Curr_Delay=0;
    digitalWrite(RELAY_CTRL,LOW);
   // Serial.println("print6");
    //Serial.println(" device ON back");
   // CycleTime=0;
    High_Current_Set_Flag=0;
   }
   
  if ((Max_Curr_Delay==1)&&(status_current==High)&& (CycleTime<3))
   {
    //Serial.print("current  abnormal");
    digitalWrite(RELAY_CTRL,HIGH);
    //Serial.println("print3");
    CycleTime++;
    //Serial.print("Cycle==");
   //Serial.println(CycleTime);
       Max_Curr_Delay=0;
     if(CycleTime>3)
     {  
      //Serial.println("CycleTime_3reached");
    digitalWrite(RELAY_CTRL,HIGH);
    //Serial.println("print4");
    //CycleTime=0;
     }
   }


   if((status_voltage==Normal)&&(status_current==Normal)&&(CycleTime <3)&&(Max_Curr_Delay==0))
   {
    digitalWrite(RELAY_CTRL,LOW);
   // Serial.println("print7");
    High_Current_Set_Flag=0;
    //Max_Curr_Delay=0;
    CycleTime=0;
   }
  Serial.print("V =  ");
  Serial.print(Volatge_Value);
  Serial.print("  C = ");
  Serial.print(AmpsRMS);
  Serial.print("  CycleTime :");
Serial.println(CycleTime);
//Serial.println(Cycle_Time);
//Serial.println(RELAY_CTRL);
//Serial.print("Max_Curr_Delay ");
//Serial.println(Max_Curr_Delay);
//Serial.print("status_current ");
//Serial.println(status_current);

//Serial.print("status_voltage  ");
//Serial.println(status_voltage);

/*         Print a message to the LCD          */
 pinMode(RELAY_CTRL,OUTPUT);
 pinMode(LCD_RS,OUTPUT);
 pinMode(LCD_EN,OUTPUT);
 pinMode(LCD_D4,OUTPUT);
 pinMode(LCD_D5,OUTPUT);
 pinMode(LCD_D6,OUTPUT);
 pinMode(LCD_D7,OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
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
