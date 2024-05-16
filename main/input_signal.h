#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "common.h"

#define INPUT "Input_test"

static esp_adc_cal_characteristics_t adc1_chars;
uint32_t values[SAMPLES];

void input_task(void * params){
    uint32_t voltage;


    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));

    int i=0;
    // long int sum=0;
    while(1){
        if(i<SAMPLES){
            voltage=esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
            ESP_LOGI(INPUT, "%" PRIu32 " mV index value: %d", voltage, i);
            values[i]=voltage;
            i++;
        }else{
            for(int j=0; j<SAMPLES; j++){
                // sum+=values[i];
                printf("%ld,", values[j]);
                if((j+1)%32==0){
                    printf("\n");
                }
            }
            // printf("Average value: %f\n", sum/((float)SAMPLES));
            // sum=0.0;
            i=0;
            vTaskDelay(5000/portTICK_PERIOD_MS);
            return;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}