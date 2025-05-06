#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>

const int buttonPin = 2;
const int buzzerPin = 4;
const int ledPin = 6;

const int numLEDs = 4;
Adafruit_NeoPixel strip(numLEDs, ledPin, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C lcd(0x27, 16, 2);


enum ScreenState { STARTUP, IDLE, START_RUNTIME, SHREDDING, DONE };
ScreenState currentScreen = STARTUP;


unsigned long shredStartTime = 0;
unsigned long lastTimerUpdate = 0;
float finalShredTime = 0.0;


bool buttonLock = false;
unsigned long lastButtonTime = 0;
const unsigned long debounceDelay = 300;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.backlight();
  strip.begin();
  strip.show();

  showStartup(); 
  setLEDColor(0, 255, 0);
}

void loop() {
  unsigned long now = millis();
  int buttonState = digitalRead(buttonPin);

  
  if (buttonState == HIGH && !buttonLock && now - lastButtonTime > debounceDelay) {
    lastButtonTime = now;
    buttonLock = true;
    nextScreen(); 
  }

  if (buttonState == HIGH) {
    buttonLock = false;
  }


  if (currentScreen == SHREDDING) {
  unsigned long now = millis();

  if (now - lastTimerUpdate >= 500) {
    lastTimerUpdate = now;
    float elapsed = (now - shredStartTime) / 1000.0;
    lcd.setCursor(6, 1);
    lcd.print("          ");
    lcd.setCursor(6, 1);
    lcd.print(elapsed, 1);
    lcd.print("s");
  }


  static unsigned long lastFlashTime = 0;
  static bool isYellowOn = false;

  if (now - lastFlashTime >= 500) {
    lastFlashTime = now;
    isYellowOn = !isYellowOn;

    if (isYellowOn) {
      setLEDColor(255, 255, 0);  
    } else {
      setLEDColor(0, 0, 0);      
    }
  }
}
}


void nextScreen() {
  currentScreen = static_cast<ScreenState>((currentScreen + 1) % 5);

  switch (currentScreen) {
    case STARTUP:
      showStartup();
      setLEDColor(0, 255, 0); 
      break;
    case IDLE:
      showIdle();
      setLEDColor(0, 255, 0); 
      break;
    case START_RUNTIME:
      showStartRuntime();
      setLEDColor(0, 255, 0); 
     break;
    case SHREDDING:
      showShredding();
      tone(buzzerPin, 500, 300); 
      setLEDColor(255, 255, 0); 
      break;
    case DONE:
      finalShredTime = (millis() - shredStartTime) / 1000.0;
      showDone();
      tone(buzzerPin, 700, 300);
      delay(500);
      tone(buzzerPin, 700, 300);
      setLEDColor(0, 0, 255);
      break;
  }
}

void showStartup() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Shredder ready");
  lcd.setCursor(0, 1);
  lcd.print("Insert material");
}

void showIdle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Secure lid");
  lcd.setCursor(0, 1);
  lcd.print("Next page...");
}

void showStartRuntime() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("For runtime:");
  lcd.setCursor(0, 1);
  lcd.print("Press when start");
}

void showShredding() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Shredding...");
  lcd.setCursor(0, 1);
  lcd.print("Time: 0.0s");

  shredStartTime = millis();
  lastTimerUpdate = millis();
}

void showDone() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Shredding Done");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(finalShredTime, 1);
  lcd.print("s");
}

void setLEDColor(int r, int g, int b) {
  for (int i = 0; i < numLEDs; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}
