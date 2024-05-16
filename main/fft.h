#include <math.h>

#include "esp_dsp.h"
#include "common.h"

#define FFT "Signal_test"
#define SAMPLES 1024
#define MAXF 2000000

float window[SAMPLES];
// float input_waves[SAMPLES];
float fft[SAMPLES*2];
float input_avg=0.0;
float delay=0.0;

void fft_task(uint32_t values[]){
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
            input_avg+=(float)values[i];
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

        delay=((float)maxIndex)/SAMPLES;
        delay*=MAXF;

        dsps_view(fft, SAMPLES, 64, 10, 0/*min*/, 100/*max*/, '|');
        printf("AveMAXFage value detected %f\n", ((float)dBsum)/SAMPLES);
        printf("Max value %d at index %d\n", maxValue, maxIndex);
        printf("Delay determined %f\n", delay);
}