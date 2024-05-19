#include "fft.h"
#include "input_signal.h"
#include "mqtt.h"
// #include "struct.h"

TaskHandle_t inputTask=NULL;
TaskHandle_t mqttTask=NULL;

float total_avg=0.0;

void avg(int samples){
    for(int i=0; i<samples; i++){
        input_avg+=(float)values[i];
    }
    total_avg+=input_avg;
}

void app_main(){
    init_input();
    fft_task(values);
    avg(SAMPLES);
    mqtt_init_wifi(input_avg/SAMPLES);
    input_avg=0.0;

    int total_samples=SAMPLE_TIME*opt_hertz;
    int cicles=total_samples/SAMPLES;
    int rest= total_samples%SAMPLES;

    while(1){
        if(total_samples>0){
            for(int i=0; i<cicles; i++){
                input_task(SAMPLES);
                avg(SAMPLES);
            }
        }
        if(rest>0){
            input_task(rest);
            avg(rest);
        }
        mqtt_send_avg(total_avg/total_samples);
        total_avg=0.0;

        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}