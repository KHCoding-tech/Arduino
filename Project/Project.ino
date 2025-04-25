// ultramegabombasticfuze on https://projecthub.arduino.cc/

// Screen: VCC-5V GND-GND SCL-A5 SDA-A4
//Button: 1pin-GND (with a resistor 10KO) 2pin-5V 3pin-D7 4pin-empty

#include <Adafruit_SSD1306.h>

#define BUTTON_PIN 7  // Pin connected to the button

bool isJumping = false;
int dinoY = 40;
int velocity = 0;
const int gravity = 2;
const int groundY = 40;
int cactusX = 128;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
    pinMode(BUTTON_PIN, INPUT);  // Set BUTTON_PIN as input
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (1);  // If the display is not initialized, stop the program
    }
    
    display.clearDisplay();
}

void loop() {
    static bool buttonPressed = false;
    
    // Button is pressed when the signal is HIGH (due to direct connection to 5V)
    if (digitalRead(BUTTON_PIN) == HIGH && !buttonPressed) {
        buttonPressed = true;
        isJumping = true;
        velocity = -10;  // Jump power
        delay(50);  // Debounce delay to avoid multiple presses
    } 
    
    if (digitalRead(BUTTON_PIN) == LOW) {
        buttonPressed = false; // Reset when button is released
    }

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
    if (cactusX < 20 && cactusX > 5 && dinoY == groundY) { // Check if the dino is on the ground and cactus is near
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 20);
        display.print("Game Over");
        display.display();
        while (1); // Stop the game after Game Over
    }

    delay(50);  // Delay for smooth gameplay
}