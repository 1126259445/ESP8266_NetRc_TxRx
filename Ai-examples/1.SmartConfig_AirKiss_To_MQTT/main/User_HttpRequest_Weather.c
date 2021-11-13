/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs.h"
#include "nvs_flash.h"

#include <netdb.h>
#include <sys/socket.h>
#include "cJSON.h"
#include "User_HttpRequest_Weather.h"

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "api.seniverse.com"
#define WEB_PORT 80
#define WEB_URL "https://api.seniverse.com/v3/weather/now.json?key=SJG1TE67YcBqU7D9X&location=shenzhen&language=en&unit=c"

static const char *TAG = "Weather";

static const char *REQUEST = "GET " WEB_URL " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";


HttpRequest_Weather Http_Weather;

/**************HTTP Weather interface***********************/
HttpRequest_Weather Get_Http_Weather()
{
    return Http_Weather;
}

char* Get_Http_Weather_W()
{
    return Http_Weather.weather;
}

char* Get_Http_Weather_T()
{
    return Http_Weather.temperature;
}


//Json_return data {"results":[{"location":{"id":"WS10730EM8EV","name":"深圳","country":"CN","path":"深圳,深圳,广东,中国","timezone":"Asia/Shanghai","timezone_offset":"+08:00"},"now":{"text":"多云","code":"4","temperature":"22"},"last_update":"2021-11-13T11:11:52+08:00"}]}
static uint8_t Http_Data_process(char *recv_buf)
{
     cJSON *root;
	////首先整体判断是否为一个json格式的数据
	root = cJSON_Parse(recv_buf);
	//如果是否json格式数据
	if (root == NULL)
	{
		printf("\r\n[SY] Http_Data_process not json ... \r\n");
		cJSON_Delete(root);
		return 0;
	}
    else{
        cJSON *result_arr = cJSON_GetObjectItem(root, "results");
        cJSON *arr0 = cJSON_GetArrayItem(result_arr,0);
        cJSON *now = cJSON_GetObjectItem(arr0,"now");
        char *weather = cJSON_GetObjectItem(now,"text")->valuestring;
        char *temperature = cJSON_GetObjectItem(now,"temperature")->valuestring;

        memcpy(Http_Weather.weather,weather,strlen(weather));
        memcpy(Http_Weather.temperature,temperature,strlen(temperature));

        printf("\r\nHTTP Weather >>>>>>>>>>>\r\n %s   %s  \r\n>>>>>>>>>>>>>>>>>>\r\n",Http_Weather.weather,Http_Weather.temperature);
    }
    cJSON_Delete(root);
    return 0;
}


static void Task_HttpRequestWeather(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[256];

    char all_buf[5120] = {0};
    uint16_t count = 0;

    while(1)
    {
        int err = getaddrinfo(WEB_SERVER, "80", &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

            Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);
 

        while (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        /* Read HTTP response */
        count = 0;
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }

            memcpy((void*)&all_buf[count],(void*)recv_buf,r);
            count += r;
            if(count > sizeof(all_buf)) count = 0;

        } while(r > 0);
        
        char *data_pt = strstr((const char *) all_buf,(const char *)"results");
        Http_Data_process(data_pt-2);

        ESP_LOGI(TAG, "\r\n... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(s);
        vTaskDelay(15000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Http_Request Starting again!\r\n");
    }
}


void HTTP_Weather_Init()
{
    int ret = pdFAIL;
    ret = xTaskCreate(Task_HttpRequestWeather, "Task_HttpRequestWeather", 1024*20, NULL, 5, NULL);
    if (ret != pdPASS)
    {
        printf("create Task_HttpRequestTime thread failed.\n");
    }
}
