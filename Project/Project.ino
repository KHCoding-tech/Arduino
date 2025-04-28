#include <Adafruit_SSD1306.h>

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

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
    pinMode(BUTTON_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (1);
    }
    
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
            // Randomly decide new obstacle: cactus or bird
            if (random(0, 2) == 0) {
                isBird = false; // Cactus
            } else {
                isBird = true;  // Bird
            }
        }

        // Speed up every 5 seconds
        if (millis() - lastSpeedUp > 5000) {
            speed++;
            lastSpeedUp = millis();
        }

        // Increase score based on speed
        if (millis() - lastScoreUpdate > (200 - speed * 10)) {  // Faster score increment with speed
            score++;
            lastScoreUpdate = millis();
        }

        // Draw scene
        display.clearDisplay();
        display.fillRect(10, dinoY, 10, 10, SSD1306_WHITE); // Dino

        if (isBird) {
            // Bird obstacle
            display.fillRect(cactusX, groundY - 20, 15, 10, SSD1306_WHITE); // Bird flying
        } else {
            // Cactus obstacle
            display.fillRect(cactusX, groundY, 10, 15, SSD1306_WHITE); // Cactus on ground
        }
        
        display.drawLine(0, 58, 128, 58, SSD1306_WHITE); // Ground
        
        // Draw score
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.print("Score:");
        display.print(score);

        // Draw high score
        display.setCursor(80, 0);
        display.print("High:");
        display.print(highScore);
        
        display.display();

        // Collision detection
        if (cactusX < 20 && cactusX > 5) { 
            if (!isBird && dinoY == groundY) {
                // Hit cactus on ground while on ground -> crash
                endGame();
            }
            if (isBird && dinoY < groundY - 5) {
                // Jumped into bird -> crash
                endGame();
            }
        }
    }

    delay(50);  // Smooth gameplay
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
    tone(BUZZER_PIN, 1000, 100);  // 1000Hz beep for 100ms
}

void playGameOverSound() {
    tone(BUZZER_PIN, 200, 500);   // 200Hz deep sound for 500ms
}
