// This sketch looks for 1-wire devices and
// prints their addresses (serial number) to
// the UART, in a format that is useful in Arduino sketches
// Tutorial: 
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html

#include <OneWire.h>

#define MAX_SENSORS 12

OneWire  ds(3);  // Connect your 1-wire device to pin 3
byte sensors[MAX_SENSORS][8];
byte nsensors = 0;

void setup(void) {
  Serial.begin(9600);
  discoverOneWireDevices();
}



void discoverOneWireDevices(void) {
  byte i;
  byte addr[8];
  nsensors = 0;
  
  Serial.print("Looking for 1-Wire devices...\n\r");
  while(ds.search(addr)) {
    Serial.print("Found \'1-Wire\' device with address: ");
    for( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    Serial.print("\n");
    if ( OneWire::crc8( addr, 7) == addr[7]) {
      if ( nsensors < MAX_SENSORS ) {
        Serial.print("CRC good, adding to list of sensors\n");
        for ( i = 0; i < 8; i++) {
                sensors[nsensors][i] = addr[i];
            }
        nsensors++;
      }
      else {
        Serial.print("CRC good, but sensor list full\n");
      }
    }
    else {
      Serial.print("CRC is not valid!\n");
      return;
    }
  }
  Serial.print("\nThat's it.\n");
  ds.reset_search();
  return;
}

void printDevices(void) {
  byte i,j;
  Serial.print("Nsensors: ");
  Serial.print(nsensors,DEC);
  Serial.print("\n");
  for (j=0; j<nsensors;j++) {
    for( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (sensors[j][i] < 16) {
        Serial.print('0');
      }
      Serial.print(sensors[j][i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    Serial.print("\n");
  }
}

void readDevices(void) {
  byte i,j;
  
  for (i=0; i<nsensors; i++) {
    Serial.print("Reading sensor ");
    Serial.print(i,DEC);
    Serial.print(" with address ");
    for( j = 0; j < 8; j++) {
      Serial.print("0x");
      if (sensors[i][j] < 16) {
        Serial.print('0');
      }
      Serial.print(sensors[i][j], HEX);
      if (j < 7) {
        Serial.print(", ");
      }
    }
    Serial.print("\n");   
    ds.reset();
    ds.select(sensors[i]);
    ds.write(0xBE);
    
    byte alldata[9];
    for ( j = 0; j < 9; j++) 
    {           // we need 9 bytes
      byte data;
      data = ds.read();
      alldata[j] = data;
      Serial.print("0x");
      if (data < 16) {
        Serial.print('0');
      }
      Serial.print(data, HEX);
      if (j < 8) {
        Serial.print(", ");
      }
    }
    Serial.print("\n"); 
    unsigned int unstemp;
    unstemp = alldata[1];
    unstemp <<= 8;
    unstemp += alldata[0]; 
    float temp;
    temp = unstemp;
    temp = temp / 16.;
    Serial.print(temp,4);
    Serial.print("\n");
  }
}

void setConfigFast(byte i) {
  byte j;
  Serial.print("Setting fast conv for sensor  ");
  Serial.print(i,DEC);
  Serial.print(" with address ");
  for( j = 0; j < 8; j++) {
    Serial.print("0x");
    if (sensors[i][j] < 16) {
      Serial.print('0');
    }
    Serial.print(sensors[i][j], HEX);
    if (j < 7) {
      Serial.print(", ");
    }
  }
  Serial.print("\n");   
  
  ds.reset();
  ds.select(sensors[i]);
  ds.write(0x4E);
  ds.write(0x4B);
  ds.write(0x46);
  ds.write(0x1F);
  Serial.print("Done with write\n");
}

void setConfigFastAll(void) {
  byte j;
  Serial.print("Setting fast conv for all ");
  Serial.print("\n");   
  
  ds.reset();
  ds.write(0xCC);
  ds.write(0x4E);
  ds.write(0x4B);
  ds.write(0x46);
  ds.write(0x1F);
  Serial.print("Done with write\n");
}

void convertSensor(byte i) {
  byte j;
  Serial.print("Running conversion for sensor ");
  Serial.print(i,DEC);
  Serial.print(" with address ");
  for( j = 0; j < 8; j++) {
    Serial.print("0x");
    if (sensors[i][j] < 16) {
      Serial.print('0');
    }
    Serial.print(sensors[i][j], HEX);
    if (j < 7) {
      Serial.print(", ");
    }
  }
  Serial.print("\n");   
  
  ds.reset();
  ds.select(sensors[i]);
  ds.write(0x44);
  byte data;
  data = ds.read();
  while (data == 0) {
    Serial.print('*');
    data = ds.read();
  }
  Serial.print("\nDone with conversion\n");
}

void convertAll(void) {
  byte j;
  Serial.print("Running conversion for all sensors ");
  Serial.print("\n");   
  
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);
  byte data;
  data = ds.read();
  while (data == 0) {
    Serial.print('*');
    data = ds.read();
  }
  Serial.print("\nDone with conversion\n");
}

void discoverOneWireDevicesSilent(void) {
  byte i;
  byte addr[8];
  nsensors = 0;
  
  while(ds.search(addr)) {
    if ( OneWire::crc8( addr, 7) == addr[7]) {
      if ( nsensors < MAX_SENSORS ) {
        for ( i = 0; i < 8; i++) {
                sensors[nsensors][i] = addr[i];
            }
        nsensors++;
      }
    }
  }
  ds.reset_search();
  return;
}

void setConfigFastAllSilent(void) {
  ds.reset();
  ds.write(0xCC);
  ds.write(0x4E);
  ds.write(0x4B);
  ds.write(0x46);
  ds.write(0x1F);
}


void convertAllSilent(void) {  
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);
  byte data;
  data = ds.read();
  while (data == 0) {
    data = ds.read();
  }
}


void readAllBinary(void) {
  byte i,j;
  
  for (i=0; i<nsensors; i++) {
    //send sensor address
    for( j = 0; j < 8; j++) {
      Serial.write(sensors[i][j]);
    }

    //read the sensor at that address
    ds.reset();
    ds.select(sensors[i]);
    ds.write(0xBE);
    
    for ( j = 0; j < 9; j++) 
    {
      Serial.write(ds.read());
    }
  }
}

void fastReadAllBinary(void) {
  discoverOneWireDevicesSilent();
  setConfigFastAllSilent();
  convertAllSilent();
  readAllBinary();
}

void loop(void) {
  byte ch;
  if (Serial.available() > 0) {
    ch = Serial.read();
    if (ch == 's') discoverOneWireDevices();
    if (ch == 'l') printDevices();
    if (ch == 'r') readDevices();
    if (ch == '1') setConfigFast(0);
    if (ch == '2') setConfigFast(1);
    if (ch == '!') convertSensor(0);
    if (ch == '@') convertSensor(1);
    if (ch == 'a') convertAll();
    if (ch == 'q') setConfigFastAll();
    if (ch == 'b') fastReadAllBinary();
  }
}



