#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_tls.h"
// #include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define APP_NAME "mqtt_example"
#define WIFI_SSID "TIM-30577277"
#define WIFI_PWD "SHsxdHKFQGHch6dsTYH4FyQu"

#define MQTT_USERNAME "IotIndividualAssignment"
#define MQTT_PWD "IotTest0"

#define URI "mqtt://broker.emqx.io:1883"

static void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data){
    esp_mqtt_event_handle_t event=event_data;
    esp_mqtt_client_handle_t client=event->client;
    esp_mqtt_event_id_t id=(esp_mqtt_event_id_t) event_id;

    ESP_LOGI(APP_NAME, "Event received: %d", (int)event_id);

    switch(id){
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(APP_NAME, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(APP_NAME, "MQTT_EVENT_DISCONNECT");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(APP_NAME, "MQTT_EVENT_SUBSCRIBED");
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(APP_NAME, "MQTT_EVENT_UNSUBSCRIBED");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(APP_NAME, "MQTT_EVENT_DATA");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(APP_NAME, "MQTT_EVENT_ERROR");
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(APP_NAME, "MQTT_EVENT_PUBLISHED");
            break;
        default:
            ESP_LOGI(APP_NAME, "DEFUALT");
            break;
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT) {
        ESP_LOGI(APP_NAME, "Wifi event");
        if (event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
            ESP_LOGI(APP_NAME, "Wifi start");
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            // prova a riconnettersi
            ESP_LOGI(APP_NAME, "Wifi disconnect");
        } else {
            ESP_LOGI(APP_NAME, "Wifi event default");
        }
    } else if (event_base == IP_EVENT) {
        ESP_LOGI(APP_NAME, "IP event");
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ESP_LOGI(APP_NAME, "Ip got");
        } else {
            ESP_LOGI(APP_NAME, "Default ip event");
        }
    } else {
        ESP_LOGI(APP_NAME, "Default");
    }

}

static void mqtt_configuration(){
    esp_mqtt_client_config_t mqtt_cfg={
        .broker.address.uri=URI,
    };
    
    ESP_LOGI(APP_NAME, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());

    esp_mqtt_client_handle_t client=esp_mqtt_client_init(&mqtt_cfg);
    if(client==NULL)
        ESP_LOGE(APP_NAME, "Can't init client");

    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    char* msg="hello world";
    char* topic="/test/device1/iot";
    int res=esp_mqtt_client_publish(client, topic, msg, strlen(msg), 1, false);
    ESP_LOGI(APP_NAME, "Message sent: '%s', sent result %d", msg, res);

}

static void wifi_configuration(){
    EventGroupHandle_t s_wifi_event_group = xEventGroupCreate();
    
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_cfg=WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg));

    esp_event_handler_instance_t wifi_handler;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &wifi_handler));

    wifi_config_t wifi_struct={
        .sta.ssid=WIFI_SSID,
        .sta.password=WIFI_PWD,
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_struct));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_connect();
    mqtt_configuration();
}

void fft_task(){
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //connetersi al wifi
    wifi_configuration();
}