//#ifndef EYECLIENT_H
//#define EYECLIENT_H

#include <sstream>
#include <eod/sdk.h>
#include <vector>

using namespace std;
using namespace sm::eod;
using namespace sm::eod::io;
using namespace sm::eod::utils;

//class EngineOutputDataTcpClient;

class EyeClient
{
    public:
        EyeClient(string serverAddress);
        EyeClient* getInstance();
        virtual ~EyeClient();
        string getHost();
        void setHost(string address);
        void setResolution(int x,int y);
        void connect();
        void disconnect();
        bool isConnected();
        std::vector<float>& getRotation();
        void getPosition();
        float* getCoord();
    protected:
    private:
        EyeClient();
        static EyeClient* clientInstance;
        static string hostName;
        static int tcpPortnumber;
        float directionX;
        float directionY;
        float directionZ;
        float coord[2];
        int ResolutionX;
        int ResolutionY;
        std::vector<float> degreeVector;
        EngineOutputDataTcpClient tcpClient;
        InetAddress inetAddress;
        EngineOutputDataPtr dataPtr;
        const static string DEFAULT_HOSTNAME;
        const static int DEFAULT_PORTNUMBER;
        void smooth();
};

//#endif // EYECLIENT_H
