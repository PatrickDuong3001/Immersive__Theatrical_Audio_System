///
/// \file SerialRadio.ino
/// \brief Radio implementation using the Serial communication.
///
/// \author Matthias Hertel, http://www.mathertel.de
/// \copyright Copyright (c) by Matthias Hertel.\n
/// This work is licensed under a BSD 3-Clause license.\n
/// See http://www.mathertel.de/License.aspx
///
/// \details
/// This is a Arduino sketch radio implementation that can be controlled using commands on the Serial input.
/// It can be used with various chips after adjusting the radio object definition.\n
/// Open the Serial console with 115200 baud to see current radio information and change various settings.
///
/// Wiring
/// ------
/// The necessary wiring of the various chips are described in the Testxxx example sketches.
/// No additional components are required because all is done through the serial interface.
///
/// More documentation and source code is available at http://www.mathertel.de/Arduino
///
/// History:
/// --------
/// * 05.08.2014 created.
/// * 04.10.2014 working.

#include <Arduino.h>
#include <Wire.h>

#include <radio.h>

// all possible radio chips included.
#include <RDA5807M.h>
#include <RDSParser.h>


#include <IRremote.hpp>
#define IR_RECEIVE_PIN A1

// Define some stations available at your locations here:
// 89.40 MHz as 8940 or 100.10MHz as 10010

RADIO_FREQ preset[] = {10800, 9200, 8500, 7800};  //10 channels

int i_sidx = 0; ///< Start at Station with index=0
int prev_idx = 0;
int count = 0;

RDA5807M radio; ///< Create an instance of a RDA5807 chip radio

/// get a RDS parser
RDSParser rds;


/// State of Keyboard input for this radio implementation.
enum RADIO_STATE {
  STATE_PARSECOMMAND, ///< waiting for a new command character.
  STATE_PARSEINT, ///< waiting for digits for the parameter.
  STATE_EXEC ///< executing the command.
};

bool lowLevelDebug = false;

/// Update the Frequency on the LCD display.
void DisplayFrequency(RADIO_FREQ f)
{
  char s[12];
  radio.formatFrequency(s, sizeof(s));
  Serial.print("FREQ:");
  Serial.println(s);
} // DisplayFrequency()


void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4)
{
  rds.processData(block1, block2, block3, block4);
}

/// Setup a FM only radio configuration with I/O for commands and debugging on the Serial port.
void setup()
{
  // open the Serial port
  Serial.begin(115200);
  Serial.print("Radio...");
  delay(500);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  

#ifdef ESP8266
  // For ESP8266 boards (like NodeMCU) the I2C GPIO pins in use
  // need to be specified.
  Wire.begin(D2, D1); // a common GPIO pin setting for I2C
#endif

  // Enable information to the Serial port
  radio.debugEnable(true);
  radio._wireDebug(lowLevelDebug);

  // Initialize the Radio
  radio.init();

  radio.setBandFrequency(RADIO_BAND_FM, preset[i_sidx]); // 5. preset.

  // delay(100);

  radio.setMono(false);
  radio.setMute(false);
  radio.setVolume(10);

  // setup the information chain for RDS data.
  radio.attachReceiveRDS(RDS_process);
  radio.setFrequency(preset[i_sidx]);
} // Setup


/// Constantly check for serial input commands and trigger command execution.
void loop()
{
  if (IrReceiver.decode()) {
    count = 0;
    uint16_t command = IrReceiver.decodedIRData.command;
    int new_i_sidx = i_sidx; // Store the new value temporarily
    if (command == 10240) {
      new_i_sidx = 1;
    } else if (command == 20480) {
      new_i_sidx = 2;
    } else if (command == 30720) {
      new_i_sidx = 3;
    } else if (command == 5120) {
      new_i_sidx = 4;
    } else if (command == 19456) {
      new_i_sidx = 5;
    } else if (command == 15360) {
      new_i_sidx = 6;
    } else if (command == 25088) {
      new_i_sidx = 7;
    } else if (command == 2560) {
      new_i_sidx = 8;
    } else if (command == 23040) {
      new_i_sidx = 9;
    } else if (command == 9728) {
      new_i_sidx = 10;
    } 
    if (new_i_sidx != i_sidx) { // Only update if i_sidx has changed
      int fadeDuration = 5000; // 5 seconds fade out
      int fadeSteps = 100; // Number of steps for fading
      
      for (int fadeStep = 0; fadeStep < fadeSteps; fadeStep++) {
        int fadeVolume = map(fadeStep, 0, fadeSteps-1, 10, 0); // Map fadeStep to volume (from 10 to 0)
        radio.setVolume(fadeVolume);
        delay(fadeDuration / fadeSteps);
      }
      
      i_sidx = new_i_sidx;
      Serial.print(i_sidx);
      radio.setFrequency(preset[i_sidx]);
      
      // Fade in
      for (int fadeStep = 0; fadeStep < fadeSteps; fadeStep++) {
        int fadeVolume = map(fadeStep, 0, fadeSteps-1, 0, 10); // Map fadeStep to volume (from 0 to 10)
        radio.setVolume(fadeVolume);
        delay(fadeDuration / fadeSteps);
      }
    }
    IrReceiver.resume(); 
  } else {
    count++;
    delay(1000);
    if (count == 5 && i_sidx != 0) {
      int fadeDuration = 5000; // 5 seconds fade out
      int fadeSteps = 100; // Number of steps for fading
      for (int fadeStep = 0; fadeStep < fadeSteps; fadeStep++) {
        int fadeVolume = map(fadeStep, 0, fadeSteps-1, 10, 0); // Map fadeStep to volume (from 10 to 0)
        radio.setVolume(fadeVolume);
        delay(fadeDuration / fadeSteps);
      }
      
      i_sidx = 0;
      Serial.print("count" + count);
      radio.setFrequency(preset[i_sidx]);
      
      // Fade in
      for (int fadeStep = 0; fadeStep < fadeSteps; fadeStep++) {
        int fadeVolume = map(fadeStep, 0, fadeSteps-1, 0, 10); // Map fadeStep to volume (from 0 to 10)
        radio.setVolume(fadeVolume);
        delay(fadeDuration / fadeSteps);
      }
    }
  }

  // check for RDS data
  radio.checkRDS();
} // loop

// End.
