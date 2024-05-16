#include "fft.h"
#include "input_signal.h"
#include "mqtt.h"

TaskHandle_t inputTask();
TaskHandle_t mqttTask();

void app_main(){
    input_task();
    fft_task();
    mqtt_task();

    //LOOP BEGINING
    input_task();
    fft_task();

    xTaskCreate(mqtt_task, "Mqtt task", 4096, NULL, 10, &mqttTask);
    xTaskCreate(input_task, "Input task", 4096, NULL, 10, &inputTask);

    while(1){
        fft_task();

        vTaskResume(mqttTask);
        vTaskResume(inputTask);
    }
}