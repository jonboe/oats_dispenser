#include <LiquidCrystal.h>

#define DEFAULT_PORTIONS 2
#define RIGHT  0
#define UP     1
#define DOWN   2
#define LEFT   3
#define SELECT 4
#define NONE   5

#define MOTOR_PIN_BACK A1
#define MOTOR_PIN_FORW A2

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
int adc_key_val[5] ={50, 200, 400, 600, 780 };
int NUM_KEYS = 5;
int adc_key_in;
int key=-1;
int oldkey=-1;
unsigned int portions = DEFAULT_PORTIONS;

long dispense_time(int portions)
{
  int portion_size = 40; //cookbook
  int slot_size = 14; //measured (gyllenmust)
  int slot_time = 500;//measured (12 rpm motor, 12 V)

  if (portions > 2) {
    return ((portions * portion_size)/slot_size)*slot_time;
  }
  else if (portions == 2)
  { // bad granularity of dispenser, forces us to compensate.   
    return slot_time + ((portions * portion_size)/slot_size)*slot_time;
  }
  else
  { // bad granularity of dispenser, forces us to add one slot TO BE SAFE.   
    return slot_time*1.5 + ((portions * portion_size)/slot_size)*slot_time;
  }  
}

void soft_start(unsigned int pin, unsigned int delayVal)
{
  for (int i = 0; i <= 16; i++)
  {
    analogWrite(pin, 16 * i);
    delay(delayVal);
  }    
}

void soft_stop(unsigned int pin, unsigned int delayVal)
{ 
   for (int i = 16; i > 0; i--)
  {
    analogWrite(pin, 16 * i);
    delay(delayVal);
  }      
}
void run_motor(unsigned int portions) 
{
  char str[4][2] = {"< ",
                    "| ",
                    "- ",
                    "> "};
  int j = 0;
  
  long stop_time = millis() + dispense_time(portions); 
  soft_start(MOTOR_PIN_FORW, 16);
  analogWrite(MOTOR_PIN_FORW, 0xFF);
  while (stop_time > millis()){
     delay(100);
     lcd.setCursor(14,1);         
     lcd.print(str[j]);
     lcd.print(" ");
     j++;
     if (j > 3) {
      j = 0;
     }
    }
  soft_stop(MOTOR_PIN_FORW, 16); 
  // short reverse to stop extraneous dispenising
  soft_start(MOTOR_PIN_BACK, 16);
  delay(50);
  soft_stop(MOTOR_PIN_BACK, 16);
  lcd.setCursor(9,1);         
  lcd.print("        ");  
  
  portions = DEFAULT_PORTIONS;
  lcd.setCursor(9,1); 
  lcd.print(portions); 
}

void setup()
{
  pinMode(MOTOR_PIN_FORW, OUTPUT);
  analogWrite(MOTOR_PIN_FORW,0);
  pinMode(MOTOR_PIN_BACK, OUTPUT);
  analogWrite(MOTOR_PIN_BACK,0);
  lcd.begin(16, 2);
  lcd.clear();
  
  lcd.setCursor(0,0); 
  lcd.print("Antal Portioner?"); 
  portions = DEFAULT_PORTIONS;
  lcd.setCursor(9,1); 
  lcd.print(portions); 
}
void loop()
{  
   adc_key_in = analogRead(0); 
   key = get_key(adc_key_in);  
   if (key != oldkey)          
   {
     delay(60);  // wait for debounce time
     adc_key_in = analogRead(0);
     key = get_key(adc_key_in); 
     if (key != oldkey)    
     {   
       oldkey = key;
     }
        switch(key)
    {
   case RIGHT:
     {
     break;
     }
   case LEFT:
     {
     break;
     }
   case UP:
     {
      if (portions < 5) {
         portions += 1;
       }        
      lcd.setCursor(9,1);  
      lcd.print(portions);
      break;
     }
   case DOWN:
     {
      if (portions > 1) {
         portions -= 1;
       }        
       lcd.setCursor(9,1); 
       lcd.print(portions);
       break;
     }
   case SELECT:
     {
      run_motor(portions);
      portions = DEFAULT_PORTIONS;
      break;
     }
   case NONE:
     {
      break;
     }
 } //Switch
   }
   delay(150);
}

int get_key(unsigned int input)
{
    int k;
    for (k = 0; k < NUM_KEYS; k++)
    {
      if (input < adc_key_val[k])
      {
        return k;
      }
    }   
    if (k >= NUM_KEYS)k = -1;  
    return k;
}
