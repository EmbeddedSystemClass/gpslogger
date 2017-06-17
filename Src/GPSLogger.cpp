#include <Arduino_FreeRTOS.h>

#include "USBDevice.h"
#include "SerialUSB.h"

#include "Screens/ScreenManager.h"
#include "GPS/GPSThread.h"

#include "BoardInit.h"
#include "LEDThread.h"
#include "ButtonsThread.h"

#include <FreeRTOSHelpers.h>

Thread<LEDThread> ledThread;

int main(void)
{
	InitBoard();

	//SerialUSB.begin(115200);
	USBDeviceFS.reenumerate();
	USBDeviceFS.beginCDC();

	//initDisplay();
	//initButtons();
	//initScreens();
	//initGPS();

	// Set up threads
	// TODO: Consider encapsulating init and task functions into a class(es)

	//xTaskCreate(vDisplayTask, "Display Task", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);
	//xTaskCreate(vButtonsThread, "Buttons Thread", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	//xTaskCreate(vGPSTask, "GPS Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

	ledThread.start();

	// Run scheduler and all the threads
	vTaskStartScheduler();

	// Never going to be here
	return 0;
}
