idf_component_register(SRCS "main.c" "i2c-lcd.c"
                    REQUIRES 
                    "esp_https_server" "esp_wifi" "nvs_flash"
                    "esp_http_client"  "esp_eth"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certificate.pem" "view.html")