#include <SPI.h>
#include <DMD2.h>
#include <fonts/Arial_Black_16.h>
#include <Adafruit_NeoPixel.h>

// ====== P10 LED Display Setup ======
const int WIDTH = 2;
const uint8_t *FONT = Arial_Black_16;
SoftDMD dmd(WIDTH, 1);  // 64x16 display (2 panels)

// ====== Pin Configurations ======
const int loopPin = 2;           // Touch wire
const int startButtonPin = 3;
const int buttonLEDPin = 4;
const int buzzerPin = 10;
const int ledPin = 12;

// ====== NeoPixel Setup ======
#define NEOPIXEL_PIN 5
#define NUMPIXELS    60
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// ====== Game Variables ======
volatile bool touched = false;
int touchCount = 0;
bool gameStarted = false;

unsigned long gameStartTime = 0;
const unsigned long gameDuration = 30000; // 30 seconds
unsigned long lastTouchTime = 0;
unsigned long lastDisplayUpdate = 0;

void setup() {
  pinMode(loopPin, INPUT_PULLUP);
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(buttonLEDPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);

  // P10 Display
  dmd.setBrightness(255);
  dmd.selectFont(FONT);
  dmd.begin();
  dmd.clearScreen();

  // NeoPixel
  pixels.begin();
  pixels.clear();
  pixels.show();

  // Ready LED ON
  digitalWrite(buttonLEDPin, HIGH);

  // Interrupt for touch
  attachInterrupt(digitalPinToInterrupt(loopPin), handleTouch, FALLING);
}

void loop() {
  unsigned long currentTime = millis();

  // ===== Start the Game =====
  if (!gameStarted && digitalRead(startButtonPin) == LOW) {
    delay(50); // debounce
    if (digitalRead(startButtonPin) == LOW) {
      gameStarted = true;
      gameStartTime = currentTime;
      touchCount = 0;
      digitalWrite(buttonLEDPin, LOW);
      Serial.println("Game Started!");

      // NeoPixels GREEN
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green
      }
      pixels.show();
    }
  }

  // ===== P10 Display Update =====
  if (currentTime - lastDisplayUpdate >= 1000) {
    lastDisplayUpdate = currentTime;

    dmd.clearScreen();

    int remainingTime = gameStarted ? max(0, (int)((gameDuration - (currentTime - gameStartTime)) / 1000)) : 0;

    String leftStr = String(remainingTime); // Countdown
    String rightStr = String(touchCount);   // Touch count

    int leftWidth = 0, rightWidth = 0;
    for (char c : leftStr) leftWidth += dmd.charWidth(c);
    for (char c : rightStr) rightWidth += dmd.charWidth(c);

    int leftX = (32 - leftWidth) / 2;
    int rightX = 32 + (32 - rightWidth) / 2;

    dmd.drawString(leftX, 0, leftStr.c_str());
    dmd.drawString(rightX, 0, rightStr.c_str());
  }

  // ===== Game Active =====
  if (gameStarted) {
    // Handle touch with debounce
    if (touched && currentTime - lastTouchTime > 5) {
      touchCount++;
      lastTouchTime = currentTime;

      Serial.print("Touched! Count: ");
      Serial.println(touchCount);

      // Buzzer & LED
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(ledPin, HIGH);

      // NeoPixels ORANGE
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 100, 0)); // Orange
      }
      pixels.show();

      delay(200);  // short alert

      digitalWrite(buzzerPin, LOW);
      digitalWrite(ledPin, LOW);

      // NeoPixels back to GREEN
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green again
      }
      pixels.show();

      touched = false;
    }

    // ===== Time Over =====
// ===== Time Over =====
if (currentTime - gameStartTime >= gameDuration) {
  gameStarted = false;
  Serial.println("Time's up!");
  Serial.print("Final Score: ");
  Serial.println(touchCount);

  // Flash RED 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    for (int j = 0; j < NUMPIXELS; j++) {
      pixels.setPixelColor(j, pixels.Color(255, 0, 0)); // Red
    }
    pixels.show();
    delay(200);

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    pixels.clear();
    pixels.show();
    delay(200);
  }

  // Wait 3 seconds before resetting touch count
  delay(3000);
  touchCount = 0;

  // Clear and update P10 display
  dmd.clearScreen();
  dmd.drawString(32 + (32 - dmd.charWidth('0')) / 2, 0, "0"); // show 0 in right panel
  dmd.drawString((32 - dmd.charWidth('0')) / 2, 0, "0");      // show 0 in left panel

  digitalWrite(buttonLEDPin, HIGH); // Ready LED ON
  pixels.clear();
  pixels.show(); // Turn off NeoPixels
}

  }
}

// ===== Touch Handler =====
void handleTouch() {
  if (gameStarted) {
    touched = true;
  }
}
