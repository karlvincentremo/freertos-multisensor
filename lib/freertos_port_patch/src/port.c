#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"


/*
 * FreeRTOS calls this hook on every RTOS tick.
 * The STM32 HAL also needs its tick counter updated.
 */
extern "C" void vApplicationTickHook(void)
{
    HAL_IncTick();
}


/*
 * Simple test task.
 *
 * It does not control hardware yet.
 * For now, we only want to verify that FreeRTOS
 * can create and run a task.
 */
void TestTask(void *argument)
{
    (void)argument;

    while (1)
    {
        /*
         * Wait for 1 second.
         *
         * This is a blocking FreeRTOS delay.
         */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


/*
 * STM32 clock configuration.
 *
 * The Blue Pill starts using the internal 8 MHz HSI clock.
 * We can keep this simple for the initial FreeRTOS test.
 */
static void SystemClock_Config(void)
{
}


/*
 * Main program.
 */
int main(void)
{
    /*
     * Point the vector table to the STM32 Flash memory.
     *
     * The custom FreeRTOS port needs this when starting
     * the first task.
     */
    SCB->VTOR = FLASH_BASE;

    /*
     * Initialize the STM32 HAL.
     */
    HAL_Init();

    /*
     * Configure the system clock.
     */
    SystemClock_Config();


    /*
     * Create our first FreeRTOS task.
     */
    BaseType_t result = xTaskCreate(
        TestTask,
        "TestTask",
        256,
        NULL,
        1,
        NULL
    );


    /*
     * If the task was created successfully,
     * start the FreeRTOS scheduler.
     */
    if (result == pdPASS)
    {
        vTaskStartScheduler();
    }


    /*
     * We should never reach this point if the
     * scheduler starts correctly.
     */
    while (1)
    {
    }
}