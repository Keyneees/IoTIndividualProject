#include <stddef.h>
#include <time.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "nvs_flash.h"

#include "mqtt_client.h"
#include "certificate.h"
#include "common.h"
// #include "struct.h"

#define MQTT "MQTT TASK"
#define WIFI_SSID "TIM-30577277"
#define WIFI_PWD "SHsxdHKFQGHch6dsTYH4FyQu"

#define MQTT_USERNAME "IotIndividualAssignment"
#define MQTT_PWD "IotTest0"

#define URI "mqtts://0990f18322944025907409d1514a0cf7.s1.eu.hivemq.cloud:8883"

esp_mqtt_client_config_t mqtt_cfg={
    .broker.address.uri=URI,
    .broker.verification.certificate=(const char*)certificate_pem,
    .credentials.username=MQTT_USERNAME,
    .credentials.authentication.password=MQTT_PWD,
};
esp_mqtt_client_handle_t client;

struct timespec start;
struct timespec end;
long secdiff=0;
long nsecdiff=0;
long latency=0;

static void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
    esp_mqtt_event_handle_t event=event_data;
    client=event->client;
    esp_mqtt_event_id_t id=(esp_mqtt_event_id_t) event_id;

    ESP_LOGI(MQTT, "Event received: %d", (int)event_id);

    switch(id){
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT, "MQTT_EVENT_DISCONNECT");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(MQTT, "MQTT_EVENT_SUBSCRIBED");
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(MQTT, "MQTT_EVENT_UNSUBSCRIBED");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT, "MQTT_EVENT_DATA");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(MQTT, "MQTT_EVENT_ERROR");
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(MQTT, "MQTT_EVENT_PUBLISHED");
            clock_gettime(CLOCK_MONOTONIC, &end);
            secdiff=end.tv_sec-start.tv_sec;
            nsecdiff=end.tv_nsec-start.tv_nsec;
            latency=secdiff*1000+nsecdiff/1000000;
            ESP_LOGI(MQTT, "Latency for sending message: %ld ms", latency/2);
            return;
            break;
        default:
            ESP_LOGI(MQTT, "DEFUALT");
            break;
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT) {
        ESP_LOGI(MQTT, "Wifi event");
        if (event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
            ESP_LOGI(MQTT, "Wifi start");
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            // prova a riconnettersi
            ESP_LOGI(MQTT, "Wifi disconnect");
        } else {
            ESP_LOGI(MQTT, "Wifi event default");
        }
    } else if (event_base == IP_EVENT) {
        ESP_LOGI(MQTT, "IP event");
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ESP_LOGI(MQTT, "Ip got");
        } else {
            ESP_LOGI(MQTT, "Default ip event");
        }
    } else {
        ESP_LOGI(MQTT, "Default");
    }

}

static void mqtt_configuration(float input_avg){
    char msg[64];
    sprintf(msg, "Average input signal %f", input_avg);
    char* topic="/test/device1/iot";
    clock_gettime(CLOCK_MONOTONIC, &start);
    int res=esp_mqtt_client_publish(client, topic, msg, strlen(msg), 1, false);
    ESP_LOGI(MQTT, "Message sent: '%s', sent result %d", msg, res);

}

static void mqtt_send_avg(float input_avg){
    // esp_wifi_connect();
    mqtt_configuration(input_avg);
}

void mqtt_init_wifi(float input_avg){
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //connetersi al wifi
    EventGroupHandle_t s_wifi_event_group = xEventGroupCreate();
    
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_cfg=WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg));

    esp_event_handler_instance_t wifi_handler;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &wifi_handler));

    wifi_config_t wifi_struct={
        .sta.ssid=WIFI_SSID,
        .sta.password=WIFI_PWD,
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_struct));
    ESP_ERROR_CHECK(esp_wifi_start());

    client=esp_mqtt_client_init(&mqtt_cfg);
    if(client==NULL)
        ESP_LOGE(MQTT, "Can't init client");

    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    vTaskDelay(15000/portTICK_PERIOD_MS);
    mqtt_configuration(input_avg);
}