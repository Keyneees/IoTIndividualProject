#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include <math.h>

#include "esp_dsp.h"

#include "wave.h"

#define APP_NAME "Signal_test"
#define SAMPLES 1024
#define R 2000000

float window[SAMPLES];
float input_waves[SAMPLES];
float fft[SAMPLES*2];

void fft_task(){
        ESP_ERROR_CHECK(dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE));

        dsps_wind_hann_f32(window, SAMPLES);

        for(int i=0; i<SAMPLES; i++){
            input_waves[i]=waves[i];
        }

        for(int i=0; i<SAMPLES; i++){
            fft[i*2]=input_waves[i]*window[i];
            // fft[i*2+1]=0;
        }

        dsps_fft2r_fc32(fft, SAMPLES);
        dsps_bit_rev_fc32(fft, SAMPLES);
        dsps_cplx2reC_fc32(fft, SAMPLES);

        float dBsum=0.0;
        int infIndex=0;
        int maxValue=0;
        int maxIndex=0;
        for(int i=0; i<SAMPLES; i++){
            fft[i]=10*log10f((fft[i]*fft[i])/SAMPLES);
            if(!isinf(fft[i])){
                printf("%f\t", fft[i]);
                if((i+1)%16==0){
                    printf("\n");
                }
                dBsum+=fft[i];
            }else{
                infIndex++;
            }
            if(maxValue<fft[i]){
                maxIndex=i;
                maxValue=fft[i];
            }
        }

        dsps_view(fft, SAMPLES, 64, 10, 0/*min*/, 100/*max*/, '|');
        printf("Average value detected %f\n", ((float)dBsum)/(SAMPLES-infIndex));
        printf("Max value %d at index %d\n", maxValue, maxIndex);
        // printf("Max frequency %f\n", ((float)index/(SAMPLES-infIndex))*R);
}