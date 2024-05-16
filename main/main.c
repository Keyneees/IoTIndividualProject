#include "fft.h"
#include "input_signal.h"
#include "mqtt.h"
#include "struct.h"

TaskHandle_t inputTask=NULL;
TaskHandle_t mqttTask=NULL;

// mqttParamsTask_t mqttParams;
// inputParamsTask_t inputParams;

void app_main(){
    input_task(NULL);
    fft_task(values);
    mqtt_init_wifi(input_avg);

    // mqttParams=(mqttParamsTask_t*)malloc(sizeof(mqttParamsTask_t));
    // inputParams=(inputParamsTask_t*)malloc(sizeof(inputParamsTask_t)); 
    //LOOP BEGINING
    input_task(NULL);
    fft_task(values);
    mqttParamsTask_t mqttParams={
        .average=input_avg,
    };
    inputParamsTask_t inputParams={
        .timeDelay=delay,
    };

    xTaskCreate(mqtt_task, "Mqtt task", 4096, (void*) &mqttParams, 10, &mqttTask);
    xTaskCreate(input_task, "Input task", 4096, (void*) &inputParams, 10, &inputTask);

    while(1){
        fft_task(values);

        vTaskResume(mqttTask);
        vTaskResume(inputTask);
    }
}