#include "TelemetryData.h"






TelemetryData::TelemetryData()
{
	strcpy_s(title, "");
	kohlsmann = 0;
	altitude = 0;
	latitude = 0;
	longitude = 0;
	pitch = 0;
	bank = 0;
	heading = 0;
	groundvelocity = 0;
	airspeed = 0;
	gforcemax = 0;

}

TelemetryData::TelemetryData(const TelemetryData &T1)
{
	strcpy_s(this->title, T1.title);
	this->kohlsmann = T1.kohlsmann;
	this->altitude = T1.altitude;
	this->latitude = T1.latitude;
	this->longitude = T1.longitude;
	this->pitch = T1.pitch;
	this->bank = T1.bank;
	this->heading = T1.heading;
	this->groundvelocity = T1.groundvelocity;
	this->airspeed = T1.airspeed;
	this->gforcemax = T1.gforcemax;
}

void TelemetryData::setTitle(const char* Title)
{
	strcpy_s(this->title, Title);
}

char* TelemetryData::getTitle()
{
	return this->title;
}

void TelemetryData::setKohlsmann(const double &Kohlsmann)
{
	this->kohlsmann = Kohlsmann;
}

double TelemetryData::getKohlsmann()
{
	return this->kohlsmann;
}

void TelemetryData::setAltitude(const double &Altitude)
{
	this->altitude = Altitude;
}

double TelemetryData::getAltitude()
{
	return this->altitude;
}

void TelemetryData::setLatitude(const double &Latitude)
{
	this->latitude = Latitude;
}

double TelemetryData::getLatittude()
{
	return this->latitude;
}

void TelemetryData::setLongitude(const double &Longitude)
{
	this->longitude = Longitude;
}

double TelemetryData::getLongitude()
{
	return this->longitude;
}

void TelemetryData::setPitch(const double &Pitch)
{
	this->pitch = Pitch;
}

double TelemetryData::getPitch()
{
	return this->pitch;
}

void TelemetryData::setBank(const double &Bank)
{
	this->bank = Bank;
}

double TelemetryData::getBank()
{
	return this->bank;
}

void TelemetryData::setHeading(const double &Heading)
{
	this->heading = Heading;
}

double TelemetryData::getHeading()
{
	return this->heading;
}

void TelemetryData::setGroundVelocity(const double &GroundVelocity)
{
	this->groundvelocity = GroundVelocity;
}

double TelemetryData::getGroundVelocity()
{
	return this->groundvelocity;
}

void TelemetryData::setAirspeed(const double &Airspeed)
{
	this->airspeed = Airspeed;
}

double TelemetryData::getAirspeed()
{
	return this->airspeed;
}

void TelemetryData::setGForceMax(const double &GForceMax)
{
	this->gforcemax = GForceMax;
}

double TelemetryData::getGForceMax()
{
	return this->gforcemax;
}


