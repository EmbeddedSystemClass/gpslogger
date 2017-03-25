#include <MapleFreeRTOS821.h>
#include <NMEAGPS.h>

#include "FreeRTOSHelpers.h"
#include "GPSDataModel.h"
#include "GPSOdometer.h"

GPSDataModel gpsDataModel;

GPSOdometer odometer0;
GPSOdometer odometer1;
GPSOdometer odometer2;

GPSDataModel::GPSDataModel()
{
	// Initialize the mutex
	// TODO: consider migrating to FreeRTOS 9.x which allows allocating mutex statically
	xGPSDataMutex = xSemaphoreCreateMutex();

	// Odometers are statically allocated
	odometers[0] = &odometer0;
	odometers[1] = &odometer1;
	odometers[2] = &odometer2;

	// All odometers inactive by default (perhaps this should be read from EEPROM)
	odometerWasActive[0] = false;
	odometerWasActive[1] = false;
	odometerWasActive[2] = false;
}

void GPSDataModel::processNewGPSFix(const gps_fix & fix)
{
	MutexLocker lock(xGPSDataMutex);
	
	// Update main fix values
	prev_fix = cur_fix;
	cur_fix = fix;
	
	// update odometer values
	odometers[0]->processNewFix(fix);
	odometers[1]->processNewFix(fix);
	odometers[2]->processNewFix(fix);
}

void GPSDataModel::processNewSatellitesData(NMEAGPS::satellite_view_t * sattelites, uint8_t count)
{
	MutexLocker lock(xGPSDataMutex);
	sattelitesData.parseSatellitesData(sattelites, count);
}

gps_fix GPSDataModel::getGPSFix() const
{
	MutexLocker lock(xGPSDataMutex);
	return cur_fix;	
}

GPSSatellitesData GPSDataModel::getSattelitesData() const
{
	MutexLocker lock(xGPSDataMutex);
	return sattelitesData;
}

float GPSDataModel::getVerticalSpeed() const
{
	MutexLocker lock(xGPSDataMutex);
	
	// Return NAN to indicate vertical speed not available
	if(!cur_fix.valid.altitude || !prev_fix.valid.altitude)
		return NAN;
	
	return cur_fix.altitude() - prev_fix.altitude(); // Assuming that time difference between cur and prev fix is 1 second
}

int GPSDataModel::timeDifference() const
{
	MutexLocker lock(xGPSDataMutex);
	
	// Return NAN to indicate vertical speed not available
	if(!cur_fix.valid.altitude || !prev_fix.valid.altitude)
		return 0;
	
	return cur_fix.dateTime - prev_fix.dateTime;
}

void GPSDataModel::resumeAllOdometer()
{
	if(odometerWasActive[0])
		odometers[0]->startOdometer();
	if(odometerWasActive[1])
		odometers[1]->startOdometer();
	if(odometerWasActive[2])
		odometers[2]->startOdometer();
}

void GPSDataModel::pauseAllOdometer()
{
	odometerWasActive[0] = odometers[0]->isActive();
	odometerWasActive[1] = odometers[1]->isActive();
	odometerWasActive[2] = odometers[2]->isActive();
	
	odometers[0]->pauseOdometer();
	odometers[1]->pauseOdometer();
	odometers[2]->pauseOdometer();
}

void GPSDataModel::resetAllOdometer()
{
	odometers[0]->resetOdometer();
	odometers[1]->resetOdometer();
	odometers[2]->resetOdometer();
	
	odometerWasActive[0] = false;
	odometerWasActive[1] = false;
	odometerWasActive[2] = false;
}
