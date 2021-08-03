#include <iostream>
#include <ctime>
#include <string>

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include "SimConnect.h"
#include <zmq.hpp>

using namespace std;

//========== GLOBAL VARIABLES (DO NOT CHANGE/REMOVE) ===========//

int quit = 0;
HANDLE hSimConnect = NULL;
HRESULT hr;
SIMCONNECT_RECV* pData = NULL;
DWORD cbData;
string telemetryData;


//========== USER DEFINED MAPPING TO SIMCONNECT EVENTS ===========//

enum GROUP_ID {
    GROUP_ID_MENU,
};

enum {
    EVENT_ID_START,
    EVENT_ID_STOP,
    EVENT_ID_4_SEC
};

enum DEFINITION_ID {
    DEFINITION_ID_USER_OBJECT,
};

enum REQUEST_ID {
    REQUEST_ID_USER_OBJECT_DATA,
};

static enum GROUP_ID10 {
    GROUP_10,
};

static enum EVENT_ID10 {
    EVENT_BRAKES_10,
};


//========== STRUCT FOR RETURN DATA ===========//

struct TelemetryDataStruct
{
    // Telemetry data is declared in this struct to be pulled from Prepar3D
    char    title[256];     //Used for security check
    
    int    ZuluYear;
    int    ZuluMonth;
    int   ZuluDayofMonth;
    float  ZuluTime;
    
    
    double  latitude;
    double  longitude;
    double  altitude;
    double  bank;
    double  pitch;
    double  heading;
};

 //========== HELPER FUNCTIONS ===========//

string ConvertToZuluTime(float ZuluSeconds) {  
    
    // Take in Zulu Time (UTC Time) in Seconds and returns string of Zulu time in  YYYY-MM-DDTHH:MM:SSZ
    
    int Seconds = (int)ZuluSeconds;
    double milliseconds = ZuluSeconds - Seconds;
    int RemainingSeconds = Seconds % 60;
    double SecondsAndMilliseconds = RemainingSeconds + milliseconds;

    int Minutes = Seconds / 60;
    int RemainingMinutes = Minutes % 60;
    int Hours = Minutes / 60;

    return (Hours < 10 ? "0" + to_string(Hours): to_string(Hours)) 
        + ":" + (RemainingMinutes < 10 ? "0" + to_string(RemainingMinutes) : to_string(RemainingMinutes)) 
        + ":" + (SecondsAndMilliseconds < 10 ? "0" + to_string(SecondsAndMilliseconds) : to_string(SecondsAndMilliseconds));
}




//========== ZEROMQ AND SIMCONNECT FUNCTIONS ===========//

void init(HRESULT &hr) {
    //Attempts to establish a connection with SimConnect API. If successful, will define data that is to be pulled from Simulator

    while (true) { // If connection is unsuccessful, will attempt to reconnect after 5 seconds
        if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Client Event", NULL, 0, NULL, 0))) {

            // Set up the data definition
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "title", NULL, SIMCONNECT_DATATYPE_STRING256);
            
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "ZULU YEAR", "Number", SIMCONNECT_DATATYPE_INT32);
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Zulu Month of Year", "Number", SIMCONNECT_DATATYPE_INT32);
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Zulu Day of Month", "Number", SIMCONNECT_DATATYPE_INT32);
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Zulu Time", "Seconds", SIMCONNECT_DATATYPE_FLOAT32);
            
            
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Plane Latitude", "degrees", SIMCONNECT_DATATYPE_FLOAT64);
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Plane Longitude", "degrees", SIMCONNECT_DATATYPE_FLOAT64);
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Plane Altitude", "feet", SIMCONNECT_DATATYPE_FLOAT64);

            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Plane Bank Degrees", "degrees", SIMCONNECT_DATATYPE_FLOAT64);
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Plane Pitch Degrees", "degrees", SIMCONNECT_DATATYPE_FLOAT64); 
            hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_ID_USER_OBJECT, "Plane Heading Degrees True", "degrees", SIMCONNECT_DATATYPE_FLOAT64);

            // Request an event every 4 secs
            hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_ID_4_SEC, "4sec");
            hr = SimConnect_SetNotificationGroupPriority(hSimConnect, GROUP_10, SIMCONNECT_GROUP_PRIORITY_HIGHEST);
            break;
        }

        else {
            cout << "\nFailed to Establish Connection with SimConnect" << endl;
            cout << "Retrying in  5 seconds" << endl;
        }
        Sleep(5000);
    }
}


void ReceivedIdEvent(SIMCONNECT_RECV *pData, time_t &current_time, HRESULT &hr) {
    
    SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;
    switch (evt->uEventID) {
        // EVENT_ID_4_SEC is written in a switch class to accommodate for possible additional cases such as parking brakes, crash events, etc.
        // Can add more cases/events as needed. SimConnect events/cases are defined in documentation.
        case EVENT_ID_4_SEC:
            // Request information on the user aircraft
            hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, REQUEST_ID_USER_OBJECT_DATA, DEFINITION_ID_USER_OBJECT, 0, SIMCONNECT_SIMOBJECT_TYPE_USER);
            break;
        default:
            break;
    }

}


string ReceivedSimObjectData(SIMCONNECT_RECV* pData) {
    
    // Receive requested Sim object data, return timestamp and 6DOF data as string in Fuse GEL format.

    SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;
    switch (pObjData->dwRequestID) {
        case REQUEST_ID_USER_OBJECT_DATA: {
            DWORD ObjectID = pObjData->dwObjectID;
            TelemetryDataStruct* pS = (TelemetryDataStruct*)&pObjData->dwData;

            if (SUCCEEDED(StringCbLengthA(&pS->title[0], sizeof(pS->title), NULL))) // security check 
            {
                string sixdof = to_string(pS->ZuluYear)
                    + "-" + (pS->ZuluMonth < 10 ? "0" + to_string(pS->ZuluMonth) : to_string(pS->ZuluMonth))
                    + "-" + (pS->ZuluDayofMonth < 10 ? "0" + to_string(pS->ZuluDayofMonth) : to_string(pS->ZuluDayofMonth))
                    + "T" + ConvertToZuluTime(pS->ZuluTime)
                    + "Z," + to_string(pS->latitude)
                    + "," + to_string(pS->longitude)
                    + "," + to_string(pS->altitude)
                    + "," + to_string(pS->bank)
                    + "," + to_string(pS->pitch)
                    + "," + to_string(pS->heading);
                cout << sixdof << endl;
                return sixdof;
            }
            break;
        }

        default:
            break;
    }

}


string RequestUpdateFromSimConnect(HRESULT& hr, SIMCONNECT_RECV* pData, DWORD& cbData) {

    // Request event ID update from SimConnect.

    time_t current_time;
    while (0 == quit) {  // Break from the loop when Prepar3D is closed or when object data is returned
        hr = SimConnect_GetNextDispatch(hSimConnect, &pData, &cbData);
        if (SUCCEEDED(hr)) {
            switch (pData->dwID) {

            case SIMCONNECT_RECV_ID_OPEN: {
                cout << "SimConnect: Successfully Connected!" << endl;
                break;
            }

            case SIMCONNECT_RECV_ID_EXCEPTION: {
                SIMCONNECT_RECV_EXCEPTION* except = (SIMCONNECT_RECV_EXCEPTION*)pData;
                cout << "Exception Received: " << except->dwException << " (Refer to SIMCONNECT_EXCEPTION in documentation)" << endl;
                break;
            }

            case SIMCONNECT_RECV_ID_EVENT: {
                ReceivedIdEvent(pData, current_time, hr);
                break;
            }

            case SIMCONNECT_RECV_ID_QUIT: { // EVENT: Prepar3D is closed and quit event ID was received
                quit = 1;
                break;
            }
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE: { return ReceivedSimObjectData(pData); } // Prepar3D object Data was received by SimConnect

            default: { // Unhandled Event IDs are caught here
                current_time = time(NULL);
                cout << "[" << current_time << "] Received Unhandled Event ID: " << pData->dwID << endl;
                break;
            }
            }
            Sleep(1);
        }
    }
}



//========== MAIN FUNCTION ===========//

int main() {

    // Request user input for TCP address
    string tcpaddress;
    
    cout << "Input TCP address (e.g. tcp://localhost:5555)" << endl;
    cin >> tcpaddress;

    // Establish connection with SimConnect API
    init(hr);
    
    // Initialize ZeroMQ context with single IO thread
    zmq::context_t context{ 1 };

    // Construct request socket and connect to interface
    zmq::socket_t socket(context, zmq::socket_type::req);
    socket.connect(tcpaddress);

    while (0 == quit) {
        // Send the request message
        cout << "Sending 6DOF" << "..." << endl;

        // Request updates from SimConnect
        telemetryData = RequestUpdateFromSimConnect(hr, pData, cbData);
        socket.send(zmq::buffer(telemetryData), zmq::send_flags::none);

        // Wait for reply/acknowledgement from server
        zmq::message_t reply{};
        socket.recv(reply, zmq::recv_flags::none);

        cout << "Received '" << reply.to_string() << "' from Server" << endl;
    }
    return 0;
}
