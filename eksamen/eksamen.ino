// TFT Screen
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h> // SCL 13 SDA 11

#define TFT_CS         10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// LED
const int redLedPin = 7;
const int greenLedPin = 4;

// Audio
const int buzzerPin = 6;

// Buttons
const int leftButton = 3;
const int rightButton = 5;

// States
int redLedState = LOW;
int buttonStateLeft;
int lastButtonStateLeft = LOW;
int greenLedState = LOW;
int buttonStateRight;
int lastButtonStateRight = LOW;

// Values
int redScore = 0;
int greenScore = 0;
unsigned long playerOneTime;
unsigned long playerTwoTime;
unsigned long lastDebounceTimeLeft;
unsigned long lastDebounceTimeRight;
unsigned long debounceDelay = 25;

// Booleans
bool ongoingGame;
bool readyPlayerOne;
bool readyPlayerTwo;


void setup() {
  // put your setup code here, to run once:
  tft.initR(INITR_MINI160x80);  // Init ST7735S mini display
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);
  Serial.begin(9600);
  Serial.println("serial started");
  digitalWrite(redLedPin, redLedState);
  digitalWrite(greenLedPin, greenLedState);
  readyPlayerOne = false;
  readyPlayerTwo = false;
  tftPrintText();
}

void loop() {
  int currentButtonStateLeft = digitalRead(leftButton);
  int currentButtonStateRight = digitalRead(rightButton);

  if (buttonStateLeft != lastButtonStateLeft) {
    // reset the debouncing timer
    lastDebounceTimeLeft = millis();
  }

  if (buttonStateRight != lastButtonStateRight) {
    // reset the debouncing timer
    lastDebounceTimeRight = millis();
  }

  if ((millis() - lastDebounceTimeLeft) > debounceDelay) {
    if (currentButtonStateLeft != buttonStateLeft) {
      buttonStateLeft = currentButtonStateLeft;
      if (buttonStateLeft == HIGH)
      {
        Serial.println("Left click");
        readyPlayerOne = !readyPlayerOne;
        redLedState = !redLedState;
      }
    }
  }

  if ((millis() - lastDebounceTimeRight) > debounceDelay) {
    if (currentButtonStateRight != buttonStateRight) {
      buttonStateRight = currentButtonStateRight;
      if (buttonStateRight == HIGH)
      {
        Serial.println("Right click");
        readyPlayerTwo = !readyPlayerTwo;
        greenLedState = !greenLedState;
      }
    }
  }

  lastButtonStateLeft = buttonStateLeft;
  lastButtonStateRight = buttonStateRight;
  digitalWrite(redLedPin, redLedState);
  digitalWrite(greenLedPin, greenLedState);

  if (readyPlayerOne == true && readyPlayerTwo == true) {
    tone(buzzerPin, 3500, 400);
    Serial.println("start game");
    startGame();
  }
}

void startGame() {
  bool playerOneClicked = false;
  bool playerTwoClicked = false;
  Serial.println("start game called");
  ongoingGame = true;
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  delay(3000);
  tone(buzzerPin, 3500, 400);
  delay(1);
  long gameTime = millis();
  Serial.println("loop starting");
  while (ongoingGame == true) {
    int currentButtonStateLeft = digitalRead(leftButton);
    int currentButtonStateRight = digitalRead(rightButton);
    if (currentButtonStateLeft == HIGH && playerOneClicked == false) {
      Serial.println("Left click");
      playerOneTime = millis();
      playerOneClicked = true;
      Serial.println(playerOneClicked);
    }
    if (currentButtonStateRight == HIGH && playerTwoClicked == false) {
      Serial.println("Right click");
      playerTwoTime = millis();
      playerTwoClicked = true;
      Serial.println(playerTwoClicked);
    }

    if (playerOneClicked == true && playerTwoClicked == true) {
      Serial.println((playerOneTime - gameTime));
      Serial.println((playerTwoTime - gameTime));
      if ((playerOneTime - gameTime) < (playerTwoTime - gameTime)) {
        digitalWrite(redLedPin, HIGH);
        redScore = ++redScore;
        Serial.print("player 1 wins: ");
        Serial.print(redScore);
        ongoingGame = false;
        tftPrintText();
        delay(5000);
      } else {
        digitalWrite(greenLedPin, HIGH);
        ongoingGame = false;
        greenScore = ++greenScore;
        Serial.print("player 2 wins: ");
        Serial.print(greenScore);
        tftPrintText();
        delay(5000);
      }
    }
  }
  delay(1);
  readyPlayerOne = false;
  readyPlayerTwo = false;
  redLedState = LOW;
  greenLedState = LOW;
}


void tftPrintText() {
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_WHITE);
  tft.setRotation(3);
  tft.setTextSize(4);
  tft.setCursor(5, 5);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Red: ");
  tft.print(redScore);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setCursor(5, 45);
  tft.setTextSize(3);
  tft.print("Green: ");
  tft.print(greenScore);
}
