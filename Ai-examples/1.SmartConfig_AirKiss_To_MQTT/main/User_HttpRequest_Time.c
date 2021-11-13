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
#include "User_HttpRequest_Time.h"

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "quan.suning.com"
#define WEB_PORT 80
#define WEB_URL "http://quan.suning.com/getSysTime.do"

static const char *TAG = "Time";

static const char *REQUEST = "GET " WEB_URL " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";


/*Time struct*/
HttpRequest_Time Http_Time;
Http_int_Time_t Http_int_Time;

/**************HTTP time interface***********************/
HttpRequest_Time Get_Http_Time()
{
    return Http_Time;
}

char* Get_Http_Time1()
{
    return Http_Time.sysTime1;
}

char* Get_Http_Time2()
{
    return Http_Time.sysTime2;
}

Http_int_Time_t Get_Http_Int_Time()
{
    return Http_int_Time;
}
uint32_t Get_Http_Int_Years()
{
    return  Http_int_Time.years;  
}
uint8_t Get_Http_Int_Mounth()
{
    return Http_int_Time.month;
}
uint8_t Get_Http_Int_Day()
{
    return Http_int_Time.day;
}
uint8_t Get_Http_Int_Hours()
{
    return Http_int_Time.hours;
}
uint8_t Get_Http_Int_Minuter()
{
    return Http_int_Time.minute;
}
uint8_t Get_Http_Int_Second()
{
    return Http_int_Time.second;
}
/*******************************************************/



void calculation(char *str_time)
{
    char str_years[5] ={0};
    char str_month[3] ={0};
    char str_day[3] ={0};
    char str_hours[3] ={0};
    char str_minute[3] ={0};
    char str_second[3] ={0};

    memcpy(str_years,str_time,4);
    memcpy(str_month,str_time+4,2);
    memcpy(str_day,str_time+6,2);
    memcpy(str_hours,str_time+8,2);
    memcpy(str_minute,str_time+10,2);
    memcpy(str_second,str_time+12,2);

    Http_int_Time.years = atoi(str_years);
    Http_int_Time.month = atoi(str_month);
    Http_int_Time.day = atoi(str_day);
    Http_int_Time.hours = atoi(str_hours);
    Http_int_Time.minute = atoi(str_minute);
    Http_int_Time.second = atoi(str_second);

    printf("Http_int_Time = %d %d %d %d:%d:%d\r\n",Http_int_Time.years,Http_int_Time.month,Http_int_Time.day,Http_int_Time.hours,Http_int_Time.minute,Http_int_Time.second);
}

//Json_return data {"sysTime2":"2021-11-07 13:06:03","sysTime1":"20211107130603"}
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
        /* 获取消息ID确认消息类型 */
        char *Net_Time2 = cJSON_GetObjectItem(root, "sysTime2")->valuestring;
        char *Net_Time1 = cJSON_GetObjectItem(root, "sysTime1")->valuestring;
        memcpy(Http_Time.sysTime2,Net_Time2,strlen(Net_Time2));
        memcpy(Http_Time.sysTime1,Net_Time1,strlen(Net_Time1));
        calculation(Http_Time.sysTime1);
        printf("\r\nHTTP Time >>>>>>>>>>>\r\n %s   %s  \r\n>>>>>>>>>>>>>>>>>>\r\n",Http_Time.sysTime2,Http_Time.sysTime1);
    }
    cJSON_Delete(root);
    return 0;
}


static void Task_HttpRequestTime(void *pvParameters)
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
        
        char *data_pt = strstr((const char *) all_buf,(const char *)"sysTime2");
        Http_Data_process(data_pt-2);

        ESP_LOGI(TAG, "\r\n... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(s);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Http_Request Starting again!\r\n");
    }
}

void HTTP_Time_Init()
{
    int ret = pdFAIL;
//Task_HttpRequestTime 
    ret = xTaskCreate(Task_HttpRequestTime, "Task_HttpRequestTime", 1024*10, NULL, 5, NULL);
    if (ret != pdPASS)
    {
        printf("create Task_HttpRequestTime thread failed.\n");
    }
}