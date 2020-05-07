/*
  LED Control

  This example scans for BLE peripherals until one with the advertised service
  "19b10000-e8f2-537e-4f6c-d104768a1214" UUID is found. Once discovered and connected,
  it will remotely control the BLE Peripheral's LED, when the button is pressed or released.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button with pull-up resistor connected to pin 2.

  You can use it with another board that is compatible with this library and the
  Peripherals -> LED example.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

void setup() 
{
  Serial.begin(9600);
  

  // initialize the BLE hardware
  BLE.begin();

  Serial.println("BLE Central - Temp Control");

  // start scanning for peripherals
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  
  randomSeed(analogRead(0));
 }

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral)
  {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    if (peripheral.localName() != "Temperature") 
    {
      return;
    }

    // stop scanning
    BLE.stopScan();

    controlLed(peripheral);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  }
}

void controlLed(BLEDevice peripheral)
{
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect())
  {
    Serial.println("Connected");
  } 
  
  else
  {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes())
  {
    Serial.println("Attributes discovered");
  } 
  
  else 
  {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  // retrieve the temp characteristic
  BLECharacteristic firstTempCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
  BLECharacteristic secondTempCharacteristic = peripheral.characteristic("19b10002-e8f2-537e-4f6c-d104768a1214");  

  if (!firstTempCharacteristic || !secondTempCharacteristic)
  {
    Serial.println("Peripheral does not have a temperature characteristic!");
    peripheral.disconnect();
    return;
  } 
  
  else if (!firstTempCharacteristic.canWrite() || !secondTempCharacteristic.canWrite()) 
  {
    Serial.println("Peripheral does not have a writable temp characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) 
  {
    // while the peripheral is connected

      
      int temp = random (0, 500);
      Serial.print(temp);
      Serial.println(" sent to receiving device.");
      firstTempCharacteristic.writeValue(highByte(temp));
      secondTempCharacteristic.writeValue(lowByte(temp));
      delay(5000);
  }

  Serial.println("Peripheral disconnected");
}

