#include "fft.h"
#include "input_signal.h"
#include "mqtt.h"
// #include "struct.h"

TaskHandle_t inputTask=NULL;
TaskHandle_t mqttTask=NULL;

void avg(){
    for(int i=0; i<SAMPLES; i++){
        input_avg+=(float)values[i];
    }
}

void app_main(){
    init_input();
    fft_task(values);
    avg();
    mqtt_init_wifi(input_avg);

    while(1){
        input_task();
        avg();
        mqtt_send_avg(input_avg);

        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}