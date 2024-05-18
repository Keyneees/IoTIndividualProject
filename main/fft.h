#include <math.h>

#include "esp_dsp.h"
#include "common.h"
// #include "struct.h"

#define FFT "Signal_test"
// #define SAMPLES 1024
#define MAXF SAMPLES/1

float window[SAMPLES];
float fft[SAMPLES*2];
float dBresult[SAMPLES/2];
float zScore[SAMPLES/2];
float input_avg=0.0;
float delay=0.0;

void fft_task(uint32_t values[]){
        ESP_ERROR_CHECK(dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE));

        dsps_wind_hann_f32(window, SAMPLES);

        for(int i=0; i<SAMPLES; i++){
            fft[i]=values[i]*window[i];
            fft[i*2+1]=0;
        }

        dsps_fft2r_fc32(fft, SAMPLES);
        dsps_bit_rev_fc32(fft, SAMPLES);
        dsps_cplx2reC_fc32(fft, SAMPLES);

        float dBmean=0.0;
        // int infIndex=0;
        // int maxValue=0;
        // int maxIndex=0;
        for(int i=0; i<SAMPLES/2; i++){
            dBresult[i]=10*log10f((fft[i*2]*fft[i*2] + fft[i*2+1]*fft[i*2+1])/SAMPLES);
            dBmean+=dBresult[i];
            input_avg+=(float)values[i];
            // if(!isinf(fft[i])){
            //     // printf("%f\t", dBresult[i]);
            //     if((i+1)%12==0){
            //         printf("\n");
            //     }
            // }
            // if(maxValue<fft[i]){
            //     maxIndex=i;
            //     maxValue=fft[i];
            // }
        }
        printf("\n");
        dBmean/=(SAMPLES/2);
        float deviation=0.0;
        for(int i=0; i<SAMPLES/2; i++){
            deviation+=(dBresult[i]-dBmean)*(dBresult[i]-dBmean);
        }
        deviation=sqrt(deviation/(SAMPLES/2));
        // printf("Deviation %f\n", deviation);
        // printf("Mean %f\n", dBmean);
        
        float zValue=0.0;
        int count=0;
        float threshold=1.5;
        int leftIndex=0;
        float leftValue=0.0;
        for(int i=0; i<SAMPLES/2; i++){
            zValue=(dBresult[i]-dBmean)/deviation;
            // printf("zValue %f\n", zValue);
            if(zValue>threshold){
                zScore[i-count]=dBresult[i];
                // printf("%f\t", zScore[i-count]);
                leftIndex=i;
                leftValue=dBresult[i];
            }else{ 
                zScore[SAMPLES/2-count]=0.0;
                // printf("%f\t", zScore[SAMPLES/2-count]);
                count++;
            }
        }

        // delay=((float)maxIndex)/SAMPLES;
        // delay*=MAXF;
        printf("Left index %d, left value %f\n", leftIndex, leftValue);

        dsps_view(dBresult, SAMPLES/2, 128, 10, 0/*min*/, 100/*max*/, '|');
        // dsps_view(zScore, SAMPLES/2, 128, 10, 0/*min*/, 100/*max*/, '|');
        // printf("Average value detected %f\n", ((float)dBsum)/SAMPLES);
        // printf("Max value %d at index %d\n", maxValue, maxIndex);
        // printf("Delay determined %f\n", delay);
}