#include <stdio.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_http_server.h"
#include "esp_https_server.h"
#include <esp_event.h>
#include <esp_system.h>
#include "esp_eth.h"
#include "esp_tls.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"
#include "i2c-lcd.h"

#define ledR 33
#define ledG 25
#define ledB 26
#define STACK_SIZE 1024*10

int8_t instrucciones;
int8_t intentos = 0;
int8_t led_r_state = 0;
int8_t led_g_state = 0;
int8_t led_b_state = 0;

char msj1[500]=" ";
float v1 = 22.5;
float v2 = 22.7;
float v3 = 22.9;
float v4 = 23.4;
float v5 = 23.8;
float v6 = 24.6;
float v7 = 25.8;
float v8 = 26.9;
float v9 = 30;
float v10 = 31;

extern const uint8_t certificate_pem_start[] asm("_binary_certificate_pem_start");
extern const uint8_t certificate_pem_end[]   asm("_binary_certificate_pem_end");

static const char *TAG = "dato base";
void wifi_connection(void);
static httpd_handle_t start_webserver(void);

esp_err_t init_led(void);
esp_err_t ComandosDefinidos(int led);

//segundo nucleo
esp_err_t create_tasks(void);
void Nucleo1(void * pvParameters);






//lcd16x2


static const char *LCD = "i2c-simple-example";


/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_NUM_0;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 1000000,
    };

    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}





//config wifi 

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        lcd_clear();
        lcd_put_cur(0, 0);
        lcd_send_string("conectando");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        lcd_clear();
        lcd_put_cur(0, 0);
        lcd_send_string("conexion");
        lcd_put_cur(1, 0);
        lcd_send_string("establecida");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        start_webserver(); 
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        printf(" ------ intento de reconexión ----- \n");

        lcd_clear();
        lcd_put_cur(0, 0);
        lcd_send_string("conexion perdida");
        lcd_put_cur(1, 0);
        lcd_send_string("reconectando");

        vTaskDelay(5000 / portTICK_PERIOD_MS);
        wifi_connection();
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection(void)
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            //.ssid = "JDDGG",
            //.password = "123456789"}};
            .ssid = "JERUSALEM",
            .password = "***AneliM***777"}};
            //.ssid = "CLARO_WIFIDB3",
            //.password = "Fuerzasespeciales23."}};

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        printf("...");
        //printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;
    }
    return ESP_OK;
}




/* An HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    extern unsigned char view_start[] asm("_binary_view_html_start");
    extern unsigned char view_end[] asm("_binary_view_html_end");
    size_t view_len = view_end - view_start;
    char viewHtml[view_len];
    memcpy(viewHtml, view_start, view_len);
    ESP_LOGI(TAG, "URI: %s", req->uri);
    
    if (strcmp(req->uri, "/?textBoxSerial=Datos") == 0)
    {
        strcpy(msj1, "Datos");
        ComandosDefinidos(4);

    }

    if (strcmp(req->uri, "/?textBoxSerial=Calibrar") == 0)
    {
        strcpy(msj1, ":visualizarV - ModificarV");
        ComandosDefinidos(5);

    }

    if (strcmp(req->uri, "/?textBoxSerial=visualizarV") == 0)
    {
        strcpy(msj1, "las variables implementadas son...");
        ComandosDefinidos(5);

    }

    if (strcmp(req->uri, "/?textBoxSerial=ModificarV") == 0)
    {
        strcpy(msj1, "establezca los valores de las variables usando...");
        ComandosDefinidos(5);

    }

    char d1[10] = "";
    char d2[10] = "";
    char d3[10] = "";
    char d4[10] = "";
    char d5[10] = "";
    char d6[10] = "";
    char d7[10] = "";
    char d8[10] = "";
    char d9[10] = "";
    char d10[10] = "";

    sprintf(d1, "%f", v1);
    sprintf(d2, "%f", v2);
    sprintf(d3, "%f", v3);
    sprintf(d4, "%f", v4);
    sprintf(d5, "%f", v5);
    sprintf(d6, "%f", v6);
    sprintf(d7, "%f", v7);
    sprintf(d8, "%f", v8);
    sprintf(d9, "%f", v9);
    sprintf(d10, "%f", v10);


    char *viewHtmlUpdated;
    int formattedStrResult = asprintf(&viewHtmlUpdated, viewHtml, msj1, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10);

    httpd_resp_set_type(req, "text/html");

    if (formattedStrResult > 0)
    {
        httpd_resp_send(req, viewHtmlUpdated, view_len);
        free(viewHtmlUpdated);
    }
    else
    {
        ESP_LOGE(TAG, "Error updating variables");
        httpd_resp_send(req, viewHtml, view_len);
    }

    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
    conf.transport_mode = HTTPD_SSL_TRANSPORT_INSECURE;
    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    return server;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_ssl_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server)
    {
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL)
    {
        *server = start_webserver();
    }
}

esp_err_t init_led(void)
{
    gpio_config_t pGPIOConfig;
    pGPIOConfig.pin_bit_mask = (1ULL << ledR) | (1ULL << ledG) | (1ULL << ledB);
    pGPIOConfig.mode = GPIO_MODE_DEF_OUTPUT;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&pGPIOConfig);

    ESP_LOGI(TAG, "init led completed");
    return ESP_OK;
}

esp_err_t ComandosDefinidos(int codigo)
{
    int8_t state = 0;
    switch (codigo)
    {
    case 1:
        led_r_state = !led_r_state;
        state = led_r_state;
        break;
    case 2:
        led_g_state = !led_g_state;
        state = led_g_state;
        break;
    case 3:
        led_b_state = !led_b_state;
        state = led_b_state;
        break;
    case 4:
        printf("Datos Seriales adquiridos\n");
        //mensaje[15] = "Datos Seriales adquiridos";
        break;
    case 5:
        printf("Menu de calibracion abierto\n");
        //mensaje[15] = "Datos Seriales adquiridos";
        break;

    default:
        /*gpio_set_level(ledR, 0);
        gpio_set_level(ledG, 0);
        gpio_set_level(ledB, 0);
        led_r_state = 0;
        led_g_state = 0;
        led_b_state = 0;*/
        break;
    }
    //gpio_set_level(led, state);
    return ESP_OK;
}



//base de datos

static void rest_post()
{
    esp_http_client_config_t post = {
        .url = "https://sistemademonitoreotest-default-rtdb.firebaseio.com/datos.json",
        .method = HTTP_METHOD_POST,
        .cert_pem = (const char *)certificate_pem_start,
        .event_handler = client_event_get_handler,
        .max_authorization_retries = 3
        };
        
    esp_http_client_handle_t client = esp_http_client_init(&post);
    char *post_data = "{\"clave\":\"12345\",\"public\":\"true\"}";
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type","application/json");
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}



void app_main(void)
{

    create_tasks();
    nvs_flash_init();
    wifi_connection();

    while(1)
    {
        vTaskDelay(8000 / portTICK_PERIOD_MS);
        printf("###");
    }

}


esp_err_t create_tasks(void)
{
    static uint8_t ucParameterToPass;
    TaskHandle_t xhandle = NULL;

    xTaskCreatePinnedToCore(
        Nucleo1,
        "Nucleo1",       // Function that implements the task.
        STACK_SIZE,          // Text name for the task.
        &ucParameterToPass,    // Parameter passed into the task.
        1,              // Priority at which the task is created.
        &xhandle,
        0);  // Variable to hold the task's data structure.
 
    return ESP_OK;
}


void Nucleo1(void * pvParameters)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(LCD, "I2C - comunicación realizada");
    lcd_init();

    while (1)
    {
        vTaskDelay(8000 / portTICK_PERIOD_MS);
        rest_post();
        v1 = v1 + 1;
        v3 = v3 + 1;
        v5 = v5 + 1;
    }
}