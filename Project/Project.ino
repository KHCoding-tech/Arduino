#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUTTON_PIN 7   // Button input
#define BUZZER_PIN 8   // Speaker/Buzzer output

bool isJumping = false;
int dinoY = 40;
int velocity = 0;
const int gravity = 2;
const int groundY = 40;
int cactusX = 128;
bool gameOver = false;

bool isBird = false;   // NEW: Is current obstacle a bird?

int speed = 3;
unsigned long lastSpeedUp = 0;
unsigned long lastScoreUpdate = 0;
int score = 0;
unsigned long startTime = 0;
unsigned long timeSurvived = 0;
unsigned long highTime = 0;
int highScore = 0;

int lastDisplayedScore = -1;

Adafruit_SSD1306 display(128, 64, &Wire, -1);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust address if needed

void setup() {
    pinMode(BUTTON_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (1);
    }

    lcd.init();
    lcd.backlight();
    lcd.clear();

    randomSeed(analogRead(0));  // Initialize randomness
    display.clearDisplay();
    startTime = millis();
}

void loop() {
    static bool buttonPressed = false;

    if (digitalRead(BUTTON_PIN) == HIGH && !buttonPressed) {
        buttonPressed = true;

        if (gameOver) {
            restartGame();
        } else {
            isJumping = true;
            velocity = -10;  // Jump power
            playJumpSound();
        }
        delay(50); // Debounce
    } 

    if (digitalRead(BUTTON_PIN) == LOW) {
        buttonPressed = false;
    }

    if (!gameOver) {
        if (isJumping) {
            dinoY += velocity;
            velocity += gravity;
            if (dinoY >= groundY) {
                dinoY = groundY;
                isJumping = false;
            }
        }

        cactusX -= speed;
        if (cactusX < -10) {
            cactusX = 128;
            isBird = (random(0, 2) == 1);
        }

        if (millis() - lastSpeedUp > 5000) {
            speed++;
            lastSpeedUp = millis();
        }

        if (millis() - lastScoreUpdate > (200 - speed * 10)) {
            score++;
            timeSurvived = (millis() - startTime) / 1000;
            lastScoreUpdate = millis();

            lcd.setCursor(0, 0);
            lcd.print("High Score:");
            lcd.print(highScore);
            lcd.print("   ");

            lcd.setCursor(0, 1);
            lcd.print("High Time:");
            lcd.print(highTime);
            lcd.print("s   ");
        }

        display.clearDisplay();
        display.fillRect(10, dinoY, 10, 10, SSD1306_WHITE);

        if (isBird) {
            display.fillRect(cactusX, groundY - 20, 15, 10, SSD1306_WHITE);
        } else {
            display.fillRect(cactusX, groundY, 10, 15, SSD1306_WHITE);
        }

        display.drawLine(0, 58, 128, 58, SSD1306_WHITE);

        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.print("Score:");
        display.print(score);

        display.setCursor(0, 10);
        display.print("Time:");
        display.print(timeSurvived);
        display.print("s");

        display.display();

        if (cactusX < 20 && cactusX > 5) {
            if (!isBird && dinoY == groundY) {
                endGame();
            }
            if (isBird && dinoY < groundY - 5) {
                endGame();
            }
        }
    }

    delay(50);
}

void restartGame() {
    isJumping = false;
    dinoY = groundY;
    velocity = 0;
    cactusX = 128;
    gameOver = false;
    speed = 3;
    lastSpeedUp = millis();
    lastScoreUpdate = millis();
    score = 0;
    isBird = false;
    timeSurvived = 0;
    lastDisplayedScore = -1;
    startTime = millis();

    lcd.clear();
    display.clearDisplay();
    display.display();
}

void endGame() {
    playGameOverSound();
    if (timeSurvived > highTime) {
        highTime = timeSurvived;
    }
    if (score > highScore) {
        highScore = score;
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 5);
    display.print("Game Over");

    display.setTextSize(1);
    display.setCursor(20, 30);
    display.print("Score: ");
    display.print(score);

    display.setCursor(20, 45);
    display.print("Time: ");
    display.print(timeSurvived);
    display.print("s");

    display.setCursor(5, 57);
    display.print("Press Btn to Restart");

    display.display();
    gameOver = true;
}

void playJumpSound() {
    tone(BUZZER_PIN, 1000, 100);
}

void playGameOverSound() {
    tone(BUZZER_PIN, 200, 500);
}
