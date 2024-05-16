#include <math.h>

#include "esp_dsp.h"
#include "common.h"

#define APP_NAME "Signal_test"
#define SAMPLES 1024
#define R 2000000

float window[SAMPLES];
// float input_waves[SAMPLES];
float fft[SAMPLES*2];

void fft_task(){
        ESP_ERROR_CHECK(dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE));

        dsps_wind_hann_f32(window, SAMPLES);

        for(int i=0; i<SAMPLES; i++){
            fft[i]=values[i]*window[i];
            // fft[i*2+1]=0;
        }

        dsps_fft2r_fc32(fft, SAMPLES);
        dsps_bit_rev_fc32(fft, SAMPLES);
        dsps_cplx2reC_fc32(fft, SAMPLES);

        float dBsum=0.0;
        // int infIndex=0;
        int maxValue=0;
        int maxIndex=0;
        for(int i=0; i<SAMPLES; i++){
            fft[i]=10*log10f((fft[i]*fft[i])/SAMPLES);
            dBsum+=values[i];
            if(!isinf(fft[i])){
                printf("%f\t", fft[i]);
                if((i+1)%16==0){
                    printf("\n");
                }
            }else{
                // infIndex++;
            }
            if(maxValue<fft[i]){
                maxIndex=i;
                maxValue=fft[i];
            }
        }

        dsps_view(fft, SAMPLES, 64, 10, 0/*min*/, 100/*max*/, '|');
        printf("Average value detected %f\n", ((float)dBsum)/SAMPLES);
        printf("Max value %d at index %d\n", maxValue, maxIndex);
        // printf("Max frequency %f\n", ((float)index/SAMPLES)*R);
}