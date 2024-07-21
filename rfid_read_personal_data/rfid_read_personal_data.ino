#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 27 or 3F
#define SDA_PIN 25
#define SCK_PIN 26
#define MOSI_PIN 27
#define MISO_PIN 14
#define RST_PIN 33

MFRC522 mfrc522(SDA_PIN, RST_PIN);  // Create MFRC522 instance
String Employee_ID = "";
String Employee_Name = "";
//*****************************************************************************************//

byte LEN;
byte block;
byte buffer[34];
MFRC522::StatusCode status;
MFRC522::MIFARE_Key key;
String TEMP;
void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SDA_PIN);
  SPI.begin();                                                // Init SPI bus
  mfrc522.PCD_Init();                                         // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:"));  //shows in serial that it is ready to read
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place your card");
  lcd.setCursor(0, 1);
  lcd.print("on RF ID Module");
}

//*****************************************************************************************//
void loop() {

 for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    return;
  lcd.clear();
  Serial.println("================== Start Reading ==================");
  lcd.clear();
  block = 1;
  Readinfo();
  Serial.print("Employee Name: ");
  Serial.println(Employee_Name);
  lcd.setCursor(0, 0);
  lcd.print(Employee_Name);
  block = 2;
  Readinfo();
  Serial.print("Employee ID: ");
  Serial.println(Employee_ID);
  lcd.setCursor(0, 1);
  lcd.print(Employee_ID);
  Serial.println(F("================================================="));
  Serial.println(F("**End Reading**"));

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Employee_Name);
  lcd.setCursor(0, 1);
  lcd.print(Employee_ID);
  delay(3000);  //change value if you want to read cards faster
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place your card");
  lcd.setCursor(0, 1);
  lcd.print("on RF ID Module");
}
//*****************************************************************************************//
void Readinfo() {
  LEN = 18;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));  //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer, &LEN);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  TEMP = "";
  for (uint8_t i = 0; i < 16; i++) {
    if (buffer[i] != 32) {
      //Serial.write(buffer[i]);
      TEMP += char(buffer[i]);
    }
  }
  TEMP.trim();
  if (block == 1)
    Employee_Name = TEMP;
  if (block == 2)
    Employee_ID = TEMP;
}