/*   Declare extern variable   */
extern long int voltage_in;
extern int Ctrlbit;
//extern int Ctrl_Switch;  // to control current status at startup
/*********************************************************************/

float Voltage_Read_Func()
   
  {
     uint32_t start_time = millis();
     
     while((millis()-start_time) < 1000) //sample for 1 Sec
     {
       float sensorValue = analogRead(A1);   
       if(sensorValue>5)
        {
          float voltage = sensorValue * (5 / 1024.0);     
          float Volt_Transformerout = 1.4+(5.7 * voltage); //r1=4.7kohm,r2=1kohm   
          voltage_in= (Volt_Transformerout*13.5);   
          
          
          Ctrlbit++;
       }
      else
       {
         Serial.println("Not sufficient volatge");  
         voltage_in = 0.00;
       }
      
     }   
     //Ctrl_Switch++;
     return voltage_in;
 }
