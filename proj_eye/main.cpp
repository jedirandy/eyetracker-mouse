#include <iostream>
#include <sstream>
#include "include/EyeClient.h"
#include "include/Mouse.h"

using namespace std;

using namespace sm::eod;
using namespace sm::eod::io;
using namespace sm::eod::utils;

const static string HOST_NAME = "172.21.3.199";
const static int RESOLUTION[2] = {1600,900};

int main()
{
    float* coord;
    EyeClient* client = new EyeClient(HOST_NAME);
    Mouse* mouse = Mouse::getInstance();
    client->setResolution(RESOLUTION[0],RESOLUTION[1]);
    client->connect();
    if(client->isConnected())
    cout<<"connected to the server: "<<client->getHost()<<endl;
    while(client->isConnected()){
        coord = client->getCoord();
        mouse->move(coord[0],coord[1]);
    }
    delete mouse;
    delete client;
    //test
    //cout<<client->getServerAddress()<<endl;
}
