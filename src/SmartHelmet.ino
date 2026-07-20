#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

#include <Preferences.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

Preferences preferences;

// BLE UART UUIDs
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

// ================= CONFIG VARIABLES =================
String recipientNumber = ""; 
String customMessage = "";
const float FREEFALL_THRESH = 4.0;      
const float IMPACT_THRESH = 20.0;       
const unsigned long FALL_WINDOW = 1000; 

const unsigned long SLEEP_DELAY = 1000;    
const float SPEED_LIMIT = 10.0;            
const unsigned long SPEED_COOLDOWN = 15000; 

// ================= PIN DEFINITIONS =================
#define I2C_SDA_PIN 8 
#define I2C_SCL_PIN 9

#define GPS_RX_PIN 18  
#define GPS_TX_PIN 17  

#define SIM_RX_PIN 16  
#define SIM_TX_PIN 15  

#define DF_RX_PIN 13   
#define DF_TX_PIN 14   

#define IR_SENSOR_PIN 4  // Receives signal from IR Sensor

// ================= OBJECTS =================
Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); 
HardwareSerial sim800(2);
SoftwareSerial dfSerial(DF_RX_PIN, DF_TX_PIN); 
DFRobotDFPlayerMini myDFPlayer;

// ================= STATE VARIABLES =================
bool possibleFallDetected = false;
unsigned long fallStartTime = 0;
bool crashReported = false; 

unsigned long eyeClosedStartTime = 0;
bool isEyeClosed = false;
bool isSleepAlarmPlaying = false;

unsigned long lastSpeedWarningTime = 0;

// ================= BLE CLASSES & FUNCTIONS =================
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue().c_str();

      if (rxValue.length() > 0) {
        Serial.print("Received Value: "); Serial.println(rxValue);

        // Expecting format: "NUMBER|MESSAGE"
        int separatorIndex = rxValue.indexOf('|');
        if (separatorIndex != -1) {
            recipientNumber = rxValue.substring(0, separatorIndex);
            customMessage = rxValue.substring(separatorIndex + 1);

            // Save to permanent memory
            preferences.begin("helmet_config", false);
            preferences.putString("phone", recipientNumber);
            preferences.putString("msg", customMessage);
            preferences.end();

            Serial.println("✅ Config Saved to Memory!");
            Serial.println("New Num: " + recipientNumber);
            Serial.println("New Msg: " + customMessage);
        }
      }
    }
};

void setupBLE() {
  preferences.begin("helmet_config", true);
  recipientNumber = preferences.getString("phone", "+94714075212"); // Default if empty
  customMessage = preferences.getString("msg", "SOS! Helmet Crash!");
  preferences.end();

  BLEDevice::init("SmartHelmet");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                       BLECharacteristic::PROPERTY_WRITE
                     );
                     
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE Started. Waiting for Android App...");
}

// ================= MAIN SETUP =================
void setup() {
  Serial.begin(115200);
  //while (!Serial) delay(10);
  Serial.println("\n=== SMART HELMET SYSTEM BOOTING ===");

  // Init Pins
  pinMode(IR_SENSOR_PIN, INPUT);

  // Init Serials
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  
  sim800.begin(9600, SERIAL_8N1, SIM_RX_PIN, SIM_TX_PIN);
  delay(1000);
  sim800.println("AT+CMGF=1"); delay(500);
  sim800.println("AT+CSCS=\"GSM\"");

  // Init DFPlayer
  dfSerial.begin(9600);
  Serial.println("Initializing DFPlayer...");
  if (!myDFPlayer.begin(dfSerial, false, true)) {
    Serial.println("⚠️ Audio Warning: Check wiring or SD card.");
  } else {
    Serial.println("✅ Audio Ready.");
    myDFPlayer.volume(30); 
  }

  // Init MPU6050
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  if (!mpu.begin()) {
    Serial.println("❌ MPU6050 Not Found! System Halted.");
    while (1) delay(10);
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);

  Serial.println("✅ SYSTEM ONLINE. Monitoring Active.");
  
  // --- AUTO-START AUDIO & BLE ---
  setupBLE();                  // Start Bluetooth Configuration Server
  myDFPlayer.playMp3Folder(3); // Play "System Active / Ride Safe" track
  delay(1000);                 // Give the audio module a brief second to start playing
}

// ================= MAIN LOOP =================
void loop() {
  // --- 1. CRITICAL: ALWAYS FEED GPS BUFFER ---
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // --- 2. DROWSINESS DETECTION (IR SENSOR) ---
  bool irDetectsSleep = digitalRead(IR_SENSOR_PIN) == LOW;

  if (irDetectsSleep) { 
    if (!isEyeClosed) {
      isEyeClosed = true;
      eyeClosedStartTime = millis(); 
    }
    if (millis() - eyeClosedStartTime > SLEEP_DELAY) {
      if (!isSleepAlarmPlaying) {
        Serial.println("💤 WAKE UP ALARM TRIGGERED! (IR Detection)");
        myDFPlayer.playMp3Folder(1); // Play Wake-Up track
        isSleepAlarmPlaying = true;
      }
    }
  } else {
    isEyeClosed = false;
    isSleepAlarmPlaying = false; 
  }

  // --- 3. SPEED WARNING LOGIC ---
  if (!isSleepAlarmPlaying && gps.location.isValid() && gps.speed.isUpdated()) {
    float currentSpeed = gps.speed.kmph();
    if (currentSpeed > SPEED_LIMIT) {
      if (millis() - lastSpeedWarningTime > SPEED_COOLDOWN) {
        Serial.print("⚠️ OVERSPEED! Speed: "); Serial.println(currentSpeed);
        myDFPlayer.playMp3Folder(2); // Play Speed Warning track
        lastSpeedWarningTime = millis();
      }
    }
  }

  if (crashReported) return; 

  // --- 4. CRASH DETECTION ---
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float totalAccel = sqrt(sq(a.acceleration.x) + sq(a.acceleration.y) + sq(a.acceleration.z));

  if (totalAccel < FREEFALL_THRESH && !possibleFallDetected) {
    possibleFallDetected = true;
    fallStartTime = millis();
    Serial.println("🔻 FALL DETECTED 🔻");
  }

  if (possibleFallDetected) {
    if (millis() - fallStartTime > FALL_WINDOW) {
      possibleFallDetected = false;
    }
    else if (totalAccel > IMPACT_THRESH) {
      Serial.println("\n🚨 CRASH CONFIRMED! 🚨");
      String googleMapsLink = getGPSLocationString();
      sendEmergencySMS(totalAccel, googleMapsLink);
      crashReported = true; 
    }
  }
}

// ================= HELPERS =================
String getGPSLocationString() {
  if (gps.location.isValid()) {
    String link = "https://maps.google.com/?q=";
    link += String(gps.location.lat(), 6); link += ","; link += String(gps.location.lng(), 6);
    return link;
  }
  return "GPS Signal Lost. Approx Location Unavailable.";
}

void sendEmergencySMS(float force, String locationLink) {
  Serial.println(">>> SENDING SMS... <<<");
  sim800.print("AT+CMGS=\""); sim800.print(recipientNumber); sim800.println("\""); delay(500);
  
  sim800.print(customMessage); 
  sim800.print("\nForce: "); sim800.print(force); sim800.print(" m/s^2\nLocation:\n");
  sim800.print(locationLink); delay(500);
  
  sim800.write(26); 
  Serial.println(">>> SMS REQUEST SENT <<<");
}