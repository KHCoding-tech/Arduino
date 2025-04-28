#include <Adafruit_SSD1306.h>

#define BUTTON_PIN 7  // Pin connected to the button

bool isJumping = false;
int dinoY = 40;
int velocity = 0;
const int gravity = 2;
const int groundY = 40;
int cactusX = 128;
bool gameOver = false;  // New variable to track game state

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
    pinMode(BUTTON_PIN, INPUT);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (1);  // If display fails
    }
    
    display.clearDisplay();
}

void loop() {
    static bool buttonPressed = false;
    
    // Read button input
    if (digitalRead(BUTTON_PIN) == HIGH && !buttonPressed) {
        buttonPressed = true;

        if (gameOver) {
            restartGame();  // Restart if game over
        } else {
            isJumping = true;
            velocity = -10;  // Jump power
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

        // Move cactus
        cactusX -= 3;
        if (cactusX < -10) cactusX = 128;

        // Draw scene
        display.clearDisplay();
        display.fillRect(10, dinoY, 10, 10, SSD1306_WHITE); // Dino
        display.fillRect(cactusX, groundY, 10, 15, SSD1306_WHITE); // Cactus
        display.drawLine(0, 58, 128, 58, SSD1306_WHITE); // Ground
        display.display();

        // Collision detection
        if (cactusX < 20 && cactusX > 5 && dinoY == groundY) {
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(20, 20);
            display.print("Game Over");
            display.display();
            gameOver = true; // Set game over flag
        }
    }

    delay(50);  // Smooth gameplay
}

void restartGame() {
    // Reset all necessary variables
    isJumping = false;
    dinoY = groundY;
    velocity = 0;
    cactusX = 128;
    gameOver = false;

    display.clearDisplay();
    display.display();
}
