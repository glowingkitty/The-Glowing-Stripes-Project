#include <SPIFFS.h>
#include "neopixel.h"
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "strip_config.h"

HardwareSerial Receiver(2); // Define a Serial port instance called 'Receiver' using serial port 2

#define Receiver_Txd_pin 17
#define Receiver_Rxd_pin 16

TaskHandle_t Task1;
TaskHandle_t Task2;

void Task1code( void * pvParameters ){
  start_leds();
}

void Task2code( void * pvParameters ){
  Serial.println("");
  Serial.print("|| Core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" || Start Receiver...");
  Serial.println("");
  // setup serial for receiving new mode
  Receiver.begin(115200, SERIAL_8N1, Receiver_Txd_pin, Receiver_Rxd_pin);
  
  String new_stripe_config = "";

  for(;;){
    // update stripe_config.json based on Receiver.read
    while (Receiver.available()) {
      // TODO auto stop existing animation - and transition it out by decreasing brightness (options: create file to trigger stop or stop task directly)
      new_stripe_config += char(Receiver.read());
    }

    if (new_stripe_config!=""){
      Serial.println("Received new stripe_config.json via Serial: ");
      Serial.println(new_stripe_config);
      update_stripe_config_based_on_string(new_stripe_config);
      new_stripe_config = "";
    }
  }
}


void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
    xTaskCreatePinnedToCore(
                      Task2code,   /* Task function. */
                      "Task2",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      1,           /* priority of the task */
                      &Task2,      /* Task handle to keep track of created task */
                      1);          /* pin task to core 1 */

    delay(500); 


    //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
    xTaskCreatePinnedToCore(
                      Task1code,   /* Task function. */
                      "Task1",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      1,           /* priority of the task */
                      &Task1,      /* Task handle to keep track of created task */
                      0);          /* pin task to core 0 */                  

    
}


void loop() {
  delay(500);
  // String new_stripe_config = "";

  // //TODO gets ignored...
  // for(;;){
  //   Serial.println("Another loop...");
  //   // update stripe_config.json based on Receiver.read
  //   while (Receiver.available()) {
  //     new_stripe_config += char(Receiver.read());
  //   }

  //   if (new_stripe_config!=""){
  //     Serial.println("Received new stripe_config.json via Serial: ");
  //     Serial.println(new_stripe_config);
  //     update_stripe_config_based_on_string(new_stripe_config);
  //     new_stripe_config = "";
  //   }
  // }
}