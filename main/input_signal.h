#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "common.h"
#include "mqtt.h"

#define INPUT "Input_test"

static esp_adc_cal_characteristics_t adc1_chars;
__attribute__((aligned(16)))
uint32_t values[SAMPLES];
// uint32_t voltage;

void input_task(){
    for(int i=0; i<SAMPLES; i++){
        values[i]=esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
        // ESP_LOGI(INPUT, "%" PRIu32 " mV index value: %d", voltage, i);
        // values[i]=voltage;
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    return;
}

void init_input(void * params){
    inputParamsTask_t* inputParams=(inputParamsTask_t*)params;

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11)); 

    input_task();
}