#include <SPIFFS.h>
#include "neopixel.h"
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "stripe_config.h"
#include "serial.h"

extern bool skip_remaining_animation;
extern bool received_stop_animation_command;

TaskHandle_t Task1;
TaskHandle_t Task2;

void Task1code( void * pvParameters ){
  start_leds();
}

void Task2code( void * pvParameters ){
  init_serial_to_web_esp();

  for(;;){
    if(web_esp_available()){
      String new_stripe_config = web_esp_received_message();

      if (new_stripe_config.startsWith("{")){
        Serial.println("Received new stripe_config.json via Serial: ");
        Serial.println(new_stripe_config);
        update_stripe_config_based_on_string(new_stripe_config);

        // TODO still causes some short glitching - should get fixed
        skip_remaining_animation = true;
        received_stop_animation_command = true;
      }
      
    }
  }
}


void setup() {
    Serial.begin(115200);
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    check_stripe_config();

    //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
    xTaskCreatePinnedToCore(
                      Task2code,   /* Task function. */
                      "Task2",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      1,           /* priority of the task */
                      &Task2,      /* Task handle to keep track of created task */
                      1);          /* pin task to core 1 */

    delay(100); 


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
}