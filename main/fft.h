#include <math.h>

#include "esp_dsp.h"
#include "common.h"
// #include "struct.h"

#define FFT "Signal_test"
#define HALF_SAMPLES SAMPLES/2

__attribute__((aligned(16)))
float window[SAMPLES];
__attribute__((aligned(16)))
float fft[SAMPLES*2];
float* fft1=&fft[0];
float* fft2=&fft[SAMPLES];
__attribute__((aligned(16)))
float dBresult[HALF_SAMPLES];
// float zScore[HALF_SAMPLES];
float hertz=0.0;

void fft_task(uint32_t values[]){
        ESP_ERROR_CHECK(dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE));

        dsps_wind_hann_f32(window, SAMPLES);

        for(int i=0; i<SAMPLES; i++){
            fft[i*2+0]=values[i]*window[i];
            fft[i*2+1]=0;
        }

        dsps_fft2r_fc32(fft, SAMPLES);
        dsps_bit_rev_fc32(fft, SAMPLES);
        dsps_cplx2reC_fc32(fft, SAMPLES);

        float dBmean=0.0;
        for(int i=0; i<HALF_SAMPLES; i++){
            fft1[i]=10*log10f((fft1[i*2]*fft1[i*2] + fft1[i*2+1]*fft1[i*2+1])/SAMPLES);
            fft2[i]=10*log10f((fft2[i*2]*fft2[i*2] + fft2[i*2+1]*fft2[i*2+1])/SAMPLES);
            dBresult[i]=fmax(fft1[i],fft2[i]);
            dBmean=dBmean+dBresult[i];
        }
        dBmean=dBmean/HALF_SAMPLES;
        float deviation=0.0;
        for(int i=0; i<HALF_SAMPLES; i++){
            deviation=deviation+pow((dBresult[i]-dBmean),2);
        }
        deviation=sqrt(deviation/(HALF_SAMPLES));
        
        float zValue=0.0;
        float threshold=3.0;
        int rigthIndex=0;
        for(int i=0; i<HALF_SAMPLES; i++){
            zValue=(dBresult[i]-dBmean)/deviation;
            if(zValue>threshold){
                rigthIndex=i;
            }
        }
        
        hertz=(rigthIndex*FREQUENCY)/SAMPLES;
        printf("Frequency recustructed %f\n", hertz);
        printf("Optimistic sample frequency %f\n", hertz*2);

        dsps_view(dBresult, HALF_SAMPLES, 128, 10, 0/*min*/, 100/*max*/, '|');
}