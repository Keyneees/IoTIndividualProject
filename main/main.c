#include "fft.h"
#include "input_signal.h"
#include "mqtt.h"

TaskHandle_t inputTask=NULL;
TaskHandle_t mqttTask=NULL;

void app_main(){
    input_task();
    fft_task(values);
    mqtt_task(input_avg);

    //LOOP BEGINING
    input_task();
    fft_task(values);

    xTaskCreate(mqtt_task, "Mqtt task", 4096, NULL, 10, &mqttTask);
    xTaskCreate(input_task, "Input task", 4096, NULL, 10, &inputTask);

    while(1){
        fft_task(values);

        vTaskResume(mqttTask);
        vTaskResume(inputTask);
    }
}