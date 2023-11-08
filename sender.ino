#include <IRremote.h>
#define IR_LED_PIN 3
#define IR_LED_PIN2 4
#define IR_LED_PIN3 5

// Create an IRsend object
IRsend irsend(IR_LED_PIN);
IRsend irsend2(IR_LED_PIN2);
IRsend irsend3(IR_LED_PIN3);
// Binary string to send
const int signalSend = 20;

void setup() {
  // put your setup code here, to run once:
  // Initialize the IR LED pin
  pinMode(IR_LED_PIN, OUTPUT);
  pinMode(IR_LED_PIN2, OUTPUT);
  pinMode(IR_LED_PIN3, OUTPUT);
}

//for now, we try sending different controller signal, but in reality, we just need to send one controller signal. 
void loop() {
  // put your main code here, to run repeatedly:
   // Send the binary string
  irsend.sendNEC(signalSend, 32);
  irsend2.sendNEC(signalSend, 32);
  irsend3.sendNEC(signalSend, 32);

  // Wait for a moment before sending another signal
  delay(1000); // 5 seconds delay, adjust as needed
}
