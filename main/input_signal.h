#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "common.h"

#define INPUT "INPUT TASK"

static esp_adc_cal_characteristics_t adc1_chars;
__attribute__((aligned(16)))
uint32_t values[SAMPLES];
float input_avg=0.0;

void input_task(float samples, float tick_delay){
    input_avg=0.0;
    for(int i=0; i<samples; i++){
        values[i]=esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    ESP_LOGI(INPUT, "Values save");
}

void init_input(){
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11)); 

    input_task(SAMPLES, SAMPLES/SAMPLES);
}