#include "user_interface.h"

#include "i2c.h"
#include "lwip.h"

#include "SSD1306_OLED.h"
#include "fonts/fonts.h"
#include "GFX_BW.h"

#include <stdio.h>


void StartUserInterfaceTask(void* args)
{
    char data[32] = { 0 };
    uint32_t i = 0;

	SSD1306_Init(&hi2c1);
	GFX_SetFont(font_8x5);

    /* Infinite loop */
    for(;;)
    {
        SSD1306_Clear(BLACK);

        sprintf(data, "Hello %ld", i++);
        GFX_DrawString(0, 0, data, WHITE, 0);
        uint32_t ip_addr = gnetif.ip_addr.addr;
        if(ip_addr)
        {
            sprintf(data, "IP: %ld.%ld.%ld.%ld",
                    (ip_addr & 0x000000FF),
                    (ip_addr & 0x0000FF00) >> 8,
                    (ip_addr & 0x00FF0000) >> 16,
                    (ip_addr & 0xFF000000) >> 24
                    );
            GFX_DrawString(0, 10, data, WHITE, 0);
        }
        SSD1306_Display();
        osDelay(1000);
    }
}