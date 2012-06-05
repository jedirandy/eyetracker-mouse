#include <iostream>
#include <sstream>
#include <vector>
#include "include/EyeClient.h"
#include "include/Mouse.h"
#include <boost/thread.hpp>
#include <sys/time.h>


using namespace std;

using namespace sm::eod;
using namespace sm::eod::io;
using namespace sm::eod::utils;

const static string HOST_NAME = "172.21.3.199";
const static int RESOLUTION[2] = {1280,1024};

int main()
{
    EyeClient* client = new EyeClient(HOST_NAME);
    // initialization of Eye Client
    client->setResolution(RESOLUTION[0],RESOLUTION[1]);
    client->connect();
    if(client->isConnected()){
        cout<<"connected to the server: "<<client->getHost()<<endl;
        client->startEngine();
    }
    delete client;
}

