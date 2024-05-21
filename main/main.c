#include "fft.h"
#include "input_signal.h"
#include "mqtt.h"

float total_avg=0.0;

void avg(int samples){
    for(int i=0; i<samples; i++){
        input_avg+=(float)values[i];
    }
    total_avg+=input_avg;
}

void app_main(){
    //INITIALIZATIONS AND FIRST DATA SAMPLE
    init_input();
    fft_task(values);
    avg(SAMPLES);
    mqtt_init_wifi(input_avg/SAMPLES);
    input_avg=0.0;

    //SAMPLING WINDOW PARAMETER COMPUTATION
    int total_samples=SAMPLE_TIME*opt_hertz;
    int cicles=total_samples/SAMPLES;
    int rest= total_samples%SAMPLES;

    //LOOP FOR A CONTINUOUS DATA SAMPLE AND AVERAGE COMMMUNICATION
    while(1){
        if(total_samples>0){
            for(int i=0; i<cicles; i++){
                input_task(SAMPLES, SAMPLES/opt_hertz);
                avg(SAMPLES);
            }
        }
        if(rest>0){
            input_task(rest, SAMPLES/opt_hertz);
            avg(rest);
        }
        mqtt_send_avg(total_avg/total_samples);
        total_avg=0.0;
    }
}