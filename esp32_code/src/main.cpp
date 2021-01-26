#include <SPIFFS.h>
#include "neopixel.h"
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "webserver.h"
#include "ota_update.h"

TaskHandle_t Task1;
TaskHandle_t Task2;

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  start_wifi();
  start_server();
  start_ota();

  for(;;){
    handle_ota();
    delay(1000);
  }
}

void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  start_leds();
}

void setup() {
    Serial.begin(115200);
    

    //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
    xTaskCreatePinnedToCore(
                      Task1code,   /* Task function. */
                      "Task1",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      1,           /* priority of the task */
                      &Task1,      /* Task handle to keep track of created task */
                      0);          /* pin task to core 0 */                  
    delay(500); 

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

}




void loop() {
    delay(500);
}