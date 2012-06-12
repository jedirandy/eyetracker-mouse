//#ifndef EYECLIENT_H
//#define EYECLIENT_H

#include <sstream>
#include <eod/sdk.h>
#include <vector>
#include <boost/thread.hpp>
#include "Mouse.h"
#include <cmath>

using namespace std;
using namespace sm::eod;
using namespace sm::eod::io;
using namespace sm::eod::utils;
using namespace boost;

class EngineOutputDataTcpClient;

class EyeClient
{
    public:
        EyeClient(string serverAddress);
        EyeClient* getInstance();
        virtual ~EyeClient();
        string getHost();
        void setHost(string address);
        void setResolution(int x,int y);
        // connection functions
        void connect();
        void disconnect();
        bool isConnected();
        void updateStatus();
        void startEngine();
        void stopEngine();
    protected:
    private:
        EyeClient();
        const static string DEFAULT_HOSTNAME;
        const static int DEFAULT_PORTNUMBER;
        static EyeClient* clientInstance;
        static string hostName;
        static int tcpPortnumber;
        float* coord;
        float directionX;
        float directionY;
        float directionZ;
        int ResolutionX;
        int ResolutionY;
        std::vector<float> degreeVector;
        sm::eod::io::EngineOutputDataTcpClient tcpClient;
        InetAddress inetAddress;
        // engine output data
        EngineOutputDataPtr dataPtr;
        std::vector<float>& getRotation();
        void getPosition();
        float* getCoord();
        Mouse* mouse;
        void setAction();
        float* filter(float* coord, int order);
        static float filterData[3][2];
        // test blink
        bool testBlink(float leftEye, float rightEye);
        static long timeStamp[3];
        static long bTimeStamp[2];
        double convertTimeToMs(struct timeval& tv);
        static int blinkActionCount;
        // action
        bool triggerAction();
        static int actionCount;
};

//#endif // EYECLIENT_H
