#define NO_OF_GAME_BUTTONS 9

const int ledPins[NO_OF_GAME_BUTTONS] = {17, 19, 23, 27, 31, 35, 39, 43, 53};
const int buttonPins[NO_OF_GAME_BUTTONS] = {16, 18, 22, 26, 30, 34, 38, 42, 52};

const int startButtonPin = 14;
const int startLEDPin = 15;

bool gameRunning = false;
unsigned long gameStartTime = 0;
int activeIndex = -1;

void setup() {
  Serial.begin(115200);

  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(startLEDPin, OUTPUT);
  digitalWrite(startLEDPin, HIGH);  // Show ready state

  for (int i = 0; i < NO_OF_GAME_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void loop() {
  if (!gameRunning && digitalRead(startButtonPin) == LOW) {
    // Start the game
    Serial.write(0);
    gameRunning = true;
    gameStartTime = millis();
    digitalWrite(startLEDPin, LOW);  // Start LED OFF during game
    delay(300); // debounce
    chooseRandomLED();
  }

  if (gameRunning) {
    // Check time
    if (millis() - gameStartTime >= 30000) {
      endGame();
    } else {
      // Check if correct button is pressed
      if (activeIndex != -1 && digitalRead(buttonPins[activeIndex]) == LOW) {
	Serial.write(1);
        digitalWrite(ledPins[activeIndex], LOW);
        delay(200);
        chooseRandomLED();
      }
    }
  }
}

void chooseRandomLED() {
  int newIndex;
  do {
    newIndex = random(0, NO_OF_GAME_BUTTONS);
  } while (newIndex == activeIndex);

  activeIndex = newIndex;
  digitalWrite(ledPins[activeIndex], HIGH);
}

void endGame() {
  gameRunning = false;
  digitalWrite(startLEDPin, HIGH); // Start LED back ON

  for (int i = 0; i < NO_OF_GAME_BUTTONS; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  activeIndex = -1;
}