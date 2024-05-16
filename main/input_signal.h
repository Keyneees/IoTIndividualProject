#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "common.h"

#define APP_NAME "Input_test"
#define NUM 1024
static esp_adc_cal_characteristics_t adc1_chars;
uint32_t values[NUM];

void input_task(){
    uint32_t voltage;


    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));

    int i=0;
    // long int sum=0;
    while(1){
        if(i<NUM){
            voltage=esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
            ESP_LOGI(APP_NAME, "%" PRIu32 " mV index value: %d", voltage, i);
            values[i]=voltage;
            input_avg+=(float)voltage;
            i++;
        }else{
            for(int j=0; j<NUM; j++){
                // sum+=values[i];
                printf("%ld,", values[j]);
                if((j+1)%32==0){
                    printf("\n");
                }
            }
            // printf("Average value: %f\n", sum/((float)NUM));
            // sum=0.0;
            i=0;
            vTaskDelay(5000/portTICK_PERIOD_MS);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}