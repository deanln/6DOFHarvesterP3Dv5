#include "string.h"

class TelemetryData
{

private:

	char title[256];
	double kohlsmann;
	double altitude;
	double latitude;
	double longitude;
	double pitch;
	double bank;
	double heading;
	double groundvelocity;
	double airspeed;
	double gforcemax;

public:

	TelemetryData();

	TelemetryData(const TelemetryData &T1);

	void setTitle(const char* Title);

	char* getTitle();

	void setKohlsmann(const double &Kohlsmann);

	double getKohlsmann();

	void setAltitude(const double &Altitude);

	double getAltitude();

	void setLatitude(const double &Latitude);

	double getLatittude();

	void setLongitude(const double &Longitude);

	double getLongitude();

	void setPitch(const double &Pitch);

	double getPitch();

	void setBank(const double &Bank);

	double getBank();

	void setHeading(const double &Heading);

	double getHeading();

	void setGroundVelocity(const double &GroundVelocity);

	double getGroundVelocity();

	void setAirspeed(const double &Airspeed);

	double getAirspeed();

	void setGForceMax(const double &GForceMax);

	double getGForceMax();




};

