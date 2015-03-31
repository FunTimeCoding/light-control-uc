#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

// addr, en, rw, rs, d4, d5, d6, d7, bl, blpol
LiquidCrystal_I2C lcd(0x20, 6, 5, 4, 0, 1, 2, 3, 7, NEGATIVE);

RotaryEncoder redEncoder(P2_1, P2_0);
RotaryEncoder greenEncoder(P2_3, P2_2);
RotaryEncoder blueEncoder(P2_5, P2_4);

int red = 0;
int green = 0;
int blue = 0;

volatile int volatileRedButtonState = LOW;
volatile int volatileGreenButtonState = LOW;
volatile int volatileBlueButtonState = LOW;

int redButtonState = LOW;
int greenButtonState = LOW;
int blueButtonState = LOW;

volatile unsigned long redButtonTime = 0;
volatile unsigned long lastRedButtonTime = 0;
volatile unsigned long greenButtonTime = 0;
volatile unsigned long lastGreenButtonTime = 0;
volatile unsigned long blueButtonTime = 0;
volatile unsigned long lastBlueButtonTime = 0;

#define BUFFER_LENGTH 21
char readBuffer[BUFFER_LENGTH];
char screenMessageBuffer[BUFFER_LENGTH];
int readBufferPosition = 0;

void setup()
{
    Serial.begin(9600);
    lcd.begin(20, 2);

    pinMode(P1_3, INPUT_PULLUP);
    pinMode(P1_4, INPUT_PULLUP);
    pinMode(P1_5, INPUT_PULLUP);

    attachInterrupt(P1_3, redButtonPressed, FALLING);
    attachInterrupt(P1_4, greenButtonPressed, FALLING);
    attachInterrupt(P1_5, blueButtonPressed, FALLING);

    Wire.begin();
    Wire.beginTransmission(9);
    Wire.write('o');
    Wire.endTransmission();

    setLedColor(0, 0, 0);

    lcd.setCursor(0, 0);
    lcd.print("Hello.");
    Serial.println("Hello.");
    delay(1000);
    screenMessageBuffer[0] = '\0';
    refreshScreen();
    sendColor();
}

void redButtonPressed()
{
    redButtonTime = millis();

    if(redButtonTime - lastRedButtonTime > 250)
    {
        volatileRedButtonState = !volatileRedButtonState;
        lastRedButtonTime = redButtonTime;
    }
}

void greenButtonPressed()
{
    greenButtonTime = millis();

    if(greenButtonTime - lastGreenButtonTime > 250)
    {
        volatileGreenButtonState = !volatileGreenButtonState;
        lastGreenButtonTime = greenButtonTime;
    }
}

void blueButtonPressed()
{
    blueButtonTime = millis();

    if(blueButtonTime - lastBlueButtonTime > 250)
    {
        volatileBlueButtonState = !volatileBlueButtonState;
        lastBlueButtonTime = blueButtonTime;
    }
}

void setLedColor(int r, int g, int b)
{
    Wire.beginTransmission(9);
    Wire.write('n');
    Wire.write(r);
    Wire.write(g);
    Wire.write(b);
    Wire.endTransmission();
}

void sendColor()
{
    Serial.print("C");
    Serial.print(" ");
    Serial.print(red);
    Serial.print(" ");
    Serial.print(green);
    Serial.print(" ");
    Serial.print(blue);
    Serial.println();
}

void refreshScreen()
{
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
    //lcd.setCursor(0, 1);
    //lcd.print(screenMessageBuffer);
}

void loop()
{
    redEncoder.tick();
    greenEncoder.tick();
    blueEncoder.tick();

    int newRedValue = redEncoder.getPosition();
    int newGreenValue = greenEncoder.getPosition();
    int newBlueValue = blueEncoder.getPosition();

    int valueWasReceived = 0;
    int screenBufferChanged = 0;

    if(Serial.available() > 0)
    {
        char character = Serial.read();

        if(character == '\n')
        {
            Serial.print("Msg: ");
            Serial.print(readBuffer);
            Serial.println();

            if('C' == readBuffer[0])
            {
                char* token = strtok(readBuffer, " ");
                int tokenNumber = 0;

                while(token)
                {
                    int color = 0;

                    if(1 == tokenNumber)
                    {
                        color = atoi(token);
                        redEncoder.setPosition(color);
                        newRedValue = color;
                    }
                    else if(2 == tokenNumber)
                    {
                        color = atoi(token);
                        greenEncoder.setPosition(color);
                        newGreenValue = color;
                    }
                    else if(3 == tokenNumber)
                    {
                        color = atoi(token);
                        blueEncoder.setPosition(color);
                        newBlueValue = color;
                    }

                    token = strtok(NULL, " ");
                    tokenNumber++;
                }
            }

            strcpy(screenMessageBuffer, readBuffer);
            valueWasReceived = 1;
            screenBufferChanged = 1;
            readBufferPosition = 0;
            readBuffer[0] = '\0';
        }
        else
        {
            if((readBufferPosition + 1) < BUFFER_LENGTH)
            {
                readBuffer[readBufferPosition] = character;
                readBufferPosition++;
                readBuffer[readBufferPosition] = '\0';
            }
            else
            {
                Serial.print('_');
            }
        }
    }

    if(newRedValue < 0)
    {
        redEncoder.setPosition(0);
        newRedValue = 255;
    }
    else if(newRedValue > 255)
    {
        redEncoder.setPosition(255);
        newRedValue = 255;
    }

    if(newGreenValue < 0)
    {
        greenEncoder.setPosition(0);
        newGreenValue = 0;
    }
    else if(newGreenValue > 255)
    {
        greenEncoder.setPosition(255);
        newGreenValue = 255;
    }

    if(newBlueValue < 0)
    {
        blueEncoder.setPosition(0);
        newBlueValue = 0;
    }
    else if(newBlueValue > 255)
    {
        blueEncoder.setPosition(255);
        newBlueValue = 255;
    }

    if(redButtonState != volatileRedButtonState)
    {
        redButtonState = volatileRedButtonState;

        if(newRedValue == 0)
        {
            newRedValue = 255;
        }
        else if(newRedValue == 255)
        {
            newRedValue = 0;
        }
        else
        {
            newRedValue = 0;
        }

        redEncoder.setPosition(newRedValue);
    }

    if(greenButtonState != volatileGreenButtonState)
    {
        greenButtonState = volatileGreenButtonState;

        if(newGreenValue == 0)
        {
            newGreenValue = 255;
        }
        else if(newGreenValue == 255)
        {
            newGreenValue = 0;
        }
        else
        {
            newGreenValue = 0;
        }

        greenEncoder.setPosition(newGreenValue);
    }

    if(blueButtonState != volatileBlueButtonState)
    {
        blueButtonState = volatileBlueButtonState;

        if(newBlueValue == 0)
        {
            newBlueValue = 255;
        }
        else if(newBlueValue == 255)
        {
            newBlueValue = 0;
        }
        else
        {
            newBlueValue = 0;
        }

        blueEncoder.setPosition(newBlueValue);
    }

    if(red != newRedValue || green != newGreenValue || blue != newBlueValue)
    {
        red = newRedValue;
        green = newGreenValue;
        blue = newBlueValue;

        setLedColor(red, green, blue);
        screenBufferChanged = 1;

        if(0 == valueWasReceived)
        {
            sendColor();
        }
    }

    if(1 == screenBufferChanged)
    {
        refreshScreen();
    }
}
