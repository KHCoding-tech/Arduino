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
        // Handle jumping physics
        if (isJumping) {
            dinoY += velocity;
            velocity += gravity;
            if (dinoY >= groundY) {
                dinoY = groundY;
                isJumping = false;
            }
        }

        // Move obstacle
        cactusX -= speed;
        if (cactusX < -10) {
            cactusX = 128;
            if (random(0, 2) == 0) {
                isBird = false;
            } else {
                isBird = true;
            }
        }

        // Speed up every 5 seconds
        if (millis() - lastSpeedUp > 5000) {
            speed++;
            lastSpeedUp = millis();
        }

        // Increase score and update LCD
        if (millis() - lastScoreUpdate > (200 - speed * 10)) {
            score++;
            lastScoreUpdate = millis();

            if (score != lastDisplayedScore) {
                lcd.setCursor(0, 0);
                lcd.print("Score:");
                lcd.print(score);
                lcd.print("    ");

                lcd.setCursor(0, 1);
                lcd.print("High:");
                lcd.print(highScore);
                lcd.print("    ");

                lastDisplayedScore = score;
            }
        }

        // Draw scene
        display.clearDisplay();
        display.fillRect(10, dinoY, 10, 10, SSD1306_WHITE); // Dino

        if (isBird) {
            display.fillRect(cactusX, groundY - 20, 15, 10, SSD1306_WHITE); // Bird
        } else {
            display.fillRect(cactusX, groundY, 10, 15, SSD1306_WHITE); // Cactus
        }

        display.drawLine(0, 58, 128, 58, SSD1306_WHITE);

        // Draw score and high score on OLED too (optional)
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.print("Score:");
        display.print(score);
        display.setCursor(80, 0);
        display.print("High:");
        display.print(highScore);

        display.display();

        // Collision detection
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
    lastDisplayedScore = -1;

    lcd.clear();
    display.clearDisplay();
    display.display();
}

void endGame() {
    playGameOverSound();
    if (score > highScore) {
        highScore = score;
    }
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 5);
    display.print("Game Over");

    display.setTextSize(1);
    display.setCursor(20, 35);
    display.print("Score: ");
    display.print(score);

    display.setCursor(20, 45);
    display.print("High: ");
    display.print(highScore);

    display.setCursor(5, 57);
    display.print("Press Btn to Restart");

    display.display();
    gameOver = true;
}

// --- Sound Functions ---
void playJumpSound() {
    tone(BUZZER_PIN, 1000, 100);
}

void playGameOverSound() {
    tone(BUZZER_PIN, 200, 500); // Shorter tone (500ms)
}
