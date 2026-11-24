#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "OLED.h"
#include "esp_log.h"

QueueHandle_t semphore_handle; // 使用二值信号量
TimerHandle_t timer_handle;    // 该定时器用于获取网络时间并显示到屏幕上

char myTime[20] = {0}; // 用于存储解析到的当前时刻
void getTime(char * myTime){}  // 该函数会联网以获取当前时间，并转换成字符串形式
void timer_callback(TimerHandle_t timer_handle) // 这里预留了获取时间的桩函数
{
    getTime(myTime);

    OLED_ShowString(2, 1, myTime);
}

void show_OLED_task(void *param) // 本任务会由信号量唤醒再执行
{
    uint32_t i = 0;
    BaseType_t err;

    while(1)
    {
        err = xSemaphoreTake(semphore_handle, portMAX_DELAY); /* 获取信号量并死等 */

        if(err == pdTRUE)
        {
            printf("获取信号量成功\r\n");
            i++; // 统计按下次数
 
            OLED_ShowNum(4, 1, i, 7); // 第二行。显示的数字长度，按 7 位对齐
        }
        else 
            printf("已超时");
    }
}

void key_task(void *param)
{
    BaseType_t err;

    while(1)
    {
        if(gpio_get_level(GPIO_NUM_4) == 0)
        {
            vTaskDelay(20); // 消抖

            while(gpio_get_level(GPIO_NUM_4) == 0)
            {
                gpio_set_level(GPIO_NUM_5, 0); // 在按键按下时，让灯亮
                gpio_set_level(GPIO_NUM_6, 0); // 在按键按下时，让蜂鸣器响

                err = xSemaphoreGive(semphore_handle); // 消息传递，让 OELD 显示按下次数
                if(err == pdPASS)
                {
                    printf("信号量释放成功！！\r\n");
                }else 
                    printf("信号量释放失败！！\r\n");
            }

            vTaskDelay(20); // 松手时候的消抖

            gpio_set_level(GPIO_NUM_5, 1);   // 在按键松手时，让灯灭
            gpio_set_level(GPIO_NUM_6, 1);   // 在按键松手时，让蜂鸣器停
        }

        vTaskDelay(5);      // 为了任务之间的顺利切换
    }
}

void app_main(void)
{
    led_init();
    beep_init();
    OLED_Init();
    key_init();

    getTime(myTime); // 预先设置 OLED 的显示格局：前两行显示时间，后两行显示按下次数
    OLED_ShowString(1, 1, "Time now :");     // 第一行，显示个提示
    OLED_ShowString(2, 1, myTime);

    OLED_ShowString(3, 1, "key down num :"); // 第三行，显示个提示
    OLED_ShowNum(4, 1, 0, 7);                // 第四行。显示的数字长度，按 7 位对齐

    semphore_handle = xSemaphoreCreateBinary(); // 赋值二值信号量
    timer_handle = xTimerCreate("Timer", 1000, pdTRUE, (void *)3, timer_callback);

    xTaskCreatePinnedToCore( show_OLED_task, "show_OLED_task", 4096, NULL, 1, NULL, 0 );
    xTaskCreatePinnedToCore( key_task      , "key_task"      , 4096, NULL, 2, NULL, 0 );
}