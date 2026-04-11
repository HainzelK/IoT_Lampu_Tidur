#define PIR_PIN 12    // PIR motion sensor connected to digital pin 12
#define BLUE_PIN 10   // Blue LED → PWM pin 10
#define GREEN_PIN 9   // Green LED → PWM pin 9
#define RED_PIN 11    // Red LED → PWM pin 11

bool isActive = false;              // Is the fade cycle running?
bool lastMotionState = LOW;         // Previous PIR reading (edge detection)
const unsigned long debounceDelay = 500;  // Ignore rapid re-triggers (ms)
unsigned long lastToggleTime = 0;   // Last toggle timestamp

// Fade cycle: 0-765 (3 transitions × 255 steps)
// 0-255: Blue→Green | 256-511: Green→Red | 512-765: Red→Blue
int colorProgress = 0;              
const int fadeSpeed = 3;            // Higher = faster fade (steps per loop)

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  
  // Start with all LEDs OFF
  analogWrite(BLUE_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(RED_PIN, 0);
  
  Serial.begin(9600);
  Serial.println("🌈 Ready: Motion toggles smooth Blue→Green→Red fade");
}

void loop() {
  int motionState = digitalRead(PIR_PIN);
  
  // ✅ Detect RISING EDGE: motion just started
  if (motionState == HIGH && lastMotionState == LOW) {
    if (millis() - lastToggleTime > debounceDelay) {
      
      // 🔁 Toggle active state
      isActive = !isActive;
      
      if (isActive) {
        Serial.println("▶️ Fade cycle STARTED: Blue → Green → Red");
      } else {
        // Turn OFF all LEDs when deactivated
        analogWrite(BLUE_PIN, 0);
        analogWrite(GREEN_PIN, 0);
        analogWrite(RED_PIN, 0);
        colorProgress = 0;  // Reset for next activation
        Serial.println("⏹ Fade cycle STOPPED: All OFF");
      }
      
      lastToggleTime = millis();
    }
  }
  
  // 🎨 If active, update the smooth fade
  if (isActive) {
    updateFadeColors();
    colorProgress = (colorProgress + fadeSpeed) % 765;  // Loop 0-764
  }
  
  lastMotionState = motionState;
  delay(20);  // Small delay for smooth timing
}

// 🎨 Calculate and output smooth RGB fade values
void updateFadeColors() {
  int r = 0, g = 0, b = 0;
  
  if (colorProgress < 256) {
    // 🔵→🟢 Blue to Green
    b = 255 - colorProgress;
    g = colorProgress;
    r = 0;
  } 
  else if (colorProgress < 512) {
    // 🟢→🔴 Green to Red
    int p = colorProgress - 256;
    g = 255 - p;
    r = p;
    b = 0;
  } 
  else {
    // 🔴→🔵 Red to Blue
    int p = colorProgress - 512;
    r = 255 - p;
    b = p;
    g = 0;
  }
  
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
  
  // Optional: Debug output (uncomment if needed)
  // Serial.print("RGB: "); Serial.print(r); Serial.print(", "); 
  // Serial.print(g); Serial.print(", "); Serial.println(b);
}
