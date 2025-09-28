// ----- Pins -----
const int relayPin   = 7;   // Relay control (Active HIGH)
const int currentPin = A0;  // Voltage across shunt
const int voltagePin = A1;  // Voltage divider

// ----- Constants -----
const float R_shunt = 10.0;    // Shunt resistor in ohms
const float ADC_REF = 5.0;     // Arduino ADC reference voltage
const int   ADC_MAX = 1023;    // 10-bit ADC resolution

// Voltage divider resistors
const float R1 = 22000.0;   // Top resistor (22k)
const float R2 = 3200.0;    // Bottom resistor (3.2k)

// ----- Thresholds -----
const float MIN_CURRENT = 0.01;  // 15 mA lower bound
const float MAX_CURRENT = 0.1;  // 35 mA upper bound
const float MIN_VOLTAGE = 0.00;    // Min LED voltage
const float MAX_VOLTAGE = 50;    // Max LED voltage

// ----- Debounce -----
int abnormalCount = 0;
const int thresholdCount = 3;   // Require 3 consecutive abnormal readings

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);

  // Start with relay ON (Active HIGH)
  digitalWrite(relayPin, HIGH);
  delay(2000);  // Let sensors stabilize
}

void loop() {
  // ----- Read Current -----
  int rawCurrent = analogRead(currentPin);
  float V_shunt = (rawCurrent * ADC_REF) / ADC_MAX;  // Voltage across shunt
  float current = V_shunt / R_shunt;                // Current in Amps

  // ----- Read Voltage -----
  int rawVoltage = analogRead(voltagePin);
  float V_div = (rawVoltage * ADC_REF) / ADC_MAX;   // ADC voltage

  // Corrected voltage calculation:
  float voltage = V_div * ((R1 + R2) / R2);         

  // ----- Print values -----
  Serial.print("Raw Voltage ADC: "); Serial.print(rawVoltage);
  Serial.print("\tCurrent (A): "); Serial.print(current, 3);
  Serial.print("\tVoltage (V): "); Serial.println(voltage, 2);

  // ----- Abnormal check -----
  if (current < MIN_CURRENT || current > MAX_CURRENT ||
      voltage < MIN_VOLTAGE || voltage > MAX_VOLTAGE) {
    abnormalCount++;
    if (abnormalCount >= thresholdCount) {
      digitalWrite(relayPin, LOW); // OFF (Active HIGH relay)
      Serial.println("!!! ABNORMAL CONDITION - Relay OFF !!!");
    }
  } else {
    abnormalCount = 0;
    digitalWrite(relayPin, HIGH); // ON (Active HIGH relay)
  }

  delay(500);
}
