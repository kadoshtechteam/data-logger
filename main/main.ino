//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd (0X27, 20, 4);

#define tempPin A0
#define NUM_SENSORS 15

// Define multiplexer select pins
const int S0 = D5; //23;
const int S1 = D6; //25;
const int S2 = D7; //26;
const int S3 = D8; //27;


float VREF = 3.3; // 5.0
float tempX [15];
unsigned long lastDisplay = 0;
const int READ_SAMPLES = 100;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  pinMode(tempPin, INPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  //lcd.init();
  //lcd.backlight();

}

// the loop routine runs over and over again forever:
void loop() {
  //takeing in the temperature values
  for (int i = 1; i <= 15; i++) {
    multiplexerSelect(i);
    tempX[i - 1] = readTemp();
  }

  // print out the value you read every 2 secs.
  if (millis() - lastDisplay > 1000) {
    for (int i = 0; i < 15; i++) {
      Serial.print("Temperature " + String(i + 1) + ": ");
      Serial.println(tempX[i]);
    }
    //lcdDisplay();
    lastDisplay = millis();
    Serial.println("===============================");
  }
}

float readTemp() {

  long sum = 0;
  for (int i = 0; i < READ_SAMPLES; ++i) {
    sum += analogRead(tempPin);
  }
  float avg = (float)sum / READ_SAMPLES;
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = avg * (VREF / 1023); //4095.0 for ESP 32
  float temperature = voltage * 100;
  return temperature;
}

// Function to select channel
void multiplexerSelect(int sensorNum) {
  digitalWrite(S0, bitRead(sensorNum, 0));
  digitalWrite(S1, bitRead(sensorNum, 1));
  digitalWrite(S2, bitRead(sensorNum, 2));
  digitalWrite(S3, bitRead(sensorNum, 3));
}



void lcdDisplay() {
  const uint8_t sensorsPerPage = 8;  // two per row * 4 rows
  static uint8_t page = 0;
  uint8_t pages = (NUM_SENSORS + sensorsPerPage - 1) / sensorsPerPage;

  uint8_t startIdx = page * sensorsPerPage;

  for (uint8_t row = 0; row < 4; ++row) {
    uint8_t leftIdx  = startIdx + row * 2;
    uint8_t rightIdx = leftIdx + 1;

    // Build left field (width 10 chars)
    String leftField = "";
    if (leftIdx < NUM_SENSORS) {
      leftField = String(leftIdx) + ":" + String(tempX[leftIdx], 1) + "C";
    }
    while (leftField.length() < 10) leftField += ' ';

    // Build right field (width 10 chars)
    String rightField = "";
    if (rightIdx < NUM_SENSORS) {
      rightField = String(rightIdx) + ":" + String(tempX[rightIdx], 1) + "C";
    }
    while (rightField.length() < 10) rightField += ' ';

    //lcd.setCursor(0, row);
    //lcd.print(leftField);
    //lcd.setCursor(10, row);
    //lcd.print(rightField);
  }

  // Advance page (caller controls timing)
  page = (page + 1) % pages;
}
