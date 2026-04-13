#define PIR_PIN 12
#define BLUE_PIN 10
#define GREEN_PIN 9
#define RED_PIN 11

const int WARM_YELLOW_R = 255;
const int WARM_YELLOW_G = 80;
const int WARM_YELLOW_B = 150;

bool isActive = false;
int lastMotionState = LOW;
unsigned long lastToggleTime = 0;
unsigned long lastValidMotion = 0;
const unsigned long debounceDelay = 1000;
const unsigned long motionTimeout = 10000;  // Reset jika 10 detik tidak ada valid motion

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  
  analogWrite(BLUE_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(RED_PIN, 0);
  
  Serial.begin(9600);
  Serial.println("💡 Motion Toggle dengan Auto-Recovery");
}

void loop() {
  int motionState = digitalRead(PIR_PIN);
  unsigned long currentMillis = millis();
  
  // 🔄 FORCE RESET: Jika sensor stuck HIGH terlalu lama
  if (motionState == HIGH && (currentMillis - lastValidMotion > motionTimeout)) {
    Serial.println("⚠️ Force reset - sensor stuck HIGH");
    lastMotionState = HIGH;  // Paksa state agar tidak trigger
    lastValidMotion = currentMillis;
  }
  
  // ✅ DETEKSI RISING EDGE yang lebih reliable
  if (motionState == HIGH && lastMotionState == LOW) {
    
    // Cek debounce
    if (currentMillis - lastToggleTime > debounceDelay) {
      
      // ✅ TOGGLE STATE
      isActive = !isActive;
      lastToggleTime = currentMillis;
      lastValidMotion = currentMillis;
      
      Serial.print("🎯 Toggle! State: ");
      Serial.println(isActive ? "ON" : "OFF");
      
      // Set LED
      if (isActive) {
        analogWrite(RED_PIN, WARM_YELLOW_R);
        analogWrite(GREEN_PIN, WARM_YELLOW_G);
        analogWrite(BLUE_PIN, WARM_YELLOW_B);
      } else {
        analogWrite(RED_PIN, 0);
        analogWrite(GREEN_PIN, 0);
        analogWrite(BLUE_PIN, 0);
      }
    }
  }
  
  // ✅ DETEKSI FALLING EDGE - penting untuk reset detection
  if (motionState == LOW && lastMotionState == HIGH) {
    lastValidMotion = currentMillis;  // Update waktu valid motion
    Serial.print(".");  // Debug: titik = sensor kembali LOW
  }
  
  // Update state terakhir
  lastMotionState = motionState;
  
  // Debug setiap 5 detik
  static unsigned long lastDebug = 0;
  if (currentMillis - lastDebug > 5000) {
    Serial.print("\n📊 Status: ");
    Serial.print(isActive ? "Lampu ON" : "Lampu OFF");
    Serial.print(" | Sensor: ");
    Serial.print(motionState == HIGH ? "HIGH" : "LOW");
    Serial.print(" | Last motion: ");
    Serial.print((currentMillis - lastValidMotion) / 1000);
    Serial.println("s ago");
    lastDebug = currentMillis;
  }
  
  delay(50);  // Stabilisasi
}
