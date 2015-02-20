#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

// addr, en, rw, rs, d4, d5, d6, d7, bl, blpol
LiquidCrystal_I2C lcd(0x20, 6, 5, 4, 0, 1, 2, 3, 7, NEGATIVE);

RotaryEncoder encoder_red(P2_1, P2_0);
RotaryEncoder encoder_green(P2_3, P2_2);
RotaryEncoder encoder_blue(P2_5, P2_4);

int red = 0;
int green = 0;
int blue = 0;

volatile int state_red = LOW;
volatile int state_green = LOW;
volatile int state_blue = LOW;

int button_state_red = LOW;
int button_state_green = LOW;
int button_state_blue = LOW;

volatile unsigned long button_time_red = 0;
volatile unsigned long last_button_time_red = 0;
volatile unsigned long button_time_green = 0;
volatile unsigned long last_button_time_green = 0;
volatile unsigned long button_time_blue = 0;  
volatile unsigned long last_button_time_blue = 0; 

String read_buffer = "";
String screen_line = "";

void setup()
{
  Serial.begin(9600);
  lcd.begin(20, 2);
  
  pinMode(P1_3, INPUT_PULLUP);
  pinMode(P1_4, INPUT_PULLUP);
  pinMode(P1_5, INPUT_PULLUP);
  
  attachInterrupt(P1_3, button_red_pressed, FALLING);
  attachInterrupt(P1_4, button_green_pressed, FALLING);
  attachInterrupt(P1_5, button_blue_pressed, FALLING);
  
  lcd.setCursor(0, 0);
  lcd.print("hello");
  Serial.println("hello");
  delay(1000);
  update_screen();
}

void button_red_pressed()
{
  button_time_red = millis();
  
  if(button_time_red - last_button_time_red > 250)
  {
    state_red = !state_red;
    last_button_time_red = button_time_red;
  }
}

void button_green_pressed()
{
  button_time_green = millis();
  
  if(button_time_green - last_button_time_green > 250)
  {
    state_green = !state_green;
    last_button_time_green = button_time_green;
  }
}

void button_blue_pressed()
{
  button_time_blue = millis();
  
  if(button_time_blue - last_button_time_blue > 250)
  {
    state_blue = !state_blue;
    last_button_time_blue = button_time_blue;
  }
}

void update_screen()
{
  Serial.print("RGB: ");
  Serial.print(red);
  Serial.print(" ");
  Serial.print(green);
  Serial.print(" ");
  Serial.print(blue);
  Serial.println();
  
  /*
  Serial.print("Button pins: ");
  Serial.print(digitalRead(P1_3));
  Serial.print(" ");
  Serial.print(digitalRead(P1_4));
  Serial.print(" ");
  Serial.print(digitalRead(P1_5));
  Serial.println();
  */
    
  /*
  Serial.print("Buttons global: ");
  Serial.print(state_red);
  Serial.print(" ");
  Serial.print(state_green);
  Serial.print(" ");
  Serial.print(state_blue);
  Serial.println();
  */
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("R:");
  lcd.setCursor(2, 0);
  lcd.print(red);
  
  lcd.setCursor(6, 0);
  lcd.print("G:");
  lcd.setCursor(8, 0);
  lcd.print(green);
  
  lcd.setCursor(12, 0);
  lcd.print("B:");
  lcd.setCursor(14, 0);
  lcd.print(blue);
}

void loop()
{
  encoder_red.tick();
  encoder_green.tick();
  encoder_blue.tick();
  
  int newRed = encoder_red.getPosition();
  int newGreen = encoder_green.getPosition();
  int newBlue = encoder_blue.getPosition();
  
  if(newRed < 0)
  {
    encoder_red.setPosition(0);
    newRed = 255;
  }
  else if(newRed > 255)
  {
    encoder_red.setPosition(255);
    newRed = 255;
  }
  
  if(newGreen < 0)
  {
    encoder_green.setPosition(0);
    newGreen = 0;
  }
  else if(newGreen > 255)
  {
    encoder_green.setPosition(255);
    newGreen = 255;
  }
  
  if(newBlue < 0)
  {
    encoder_blue.setPosition(0);
    newBlue = 0;
  }
  else if(newBlue > 255)
  {
    encoder_blue.setPosition(255);
    newBlue = 255;
  }
  
  if(button_state_red != state_red)
  {
    button_state_red = state_red;
    Serial.print("Button: red");
    Serial.println();
    
    if(newRed == 0)
    {
      newRed = 255;
    }
    else if(newRed == 255)
    {
      newRed = 0;
    }
    else
    {
      newRed = 0;
    }
    
    encoder_red.setPosition(newRed);
  }
  if(button_state_green != state_green)
  {
    button_state_green = state_green;
    Serial.print("Button: green");
    Serial.println();
    
    if(newGreen == 0)
    {
      newGreen = 255;
    }
    else if(newGreen == 255)
    {
      newGreen = 0;
    }
    else
    {
      newGreen = 0;
    }
    
    encoder_green.setPosition(newGreen);
  }
  if(button_state_blue != state_blue)
  {
    button_state_blue = state_blue;
    Serial.print("Button: blue");
    Serial.println();
    
    if(newBlue == 0)
    {
      newBlue = 255;
    }
    else if(newBlue == 255)
    {
      newBlue = 0;
    }
    else
    {
      newBlue = 0;
    }
    
    encoder_blue.setPosition(newBlue);
  }
  
  if(Serial.available() > 0)
  {
    char character = Serial.read();
    
    if(character == '\n')
    {
      //update_screen();
      
      lcd.setCursor(0, 1);
      int len = read_buffer.length() + 1;
      char buf[len];
      read_buffer.toCharArray(buf, len);
      
      lcd.print(buf);
      
      Serial.print("Message: ");
      Serial.print(read_buffer);
      Serial.println();
      
      read_buffer = "";
    }
    else
    {
      Serial.print(character);
      Serial.println();
      read_buffer.concat(character);
    }
  }
  
  if(red != newRed || green != newGreen || blue != newBlue)
  {
    red = newRed;
    green = newGreen;
    blue = newBlue;
    
    update_screen();
  }
}
