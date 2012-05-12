#include "../include/EyeClient.h"

// static members
string EyeClient::hostName = "";
EyeClient* EyeClient::clientInstance = NULL;
int EyeClient::tcpPortnumber = 0;
const string EyeClient::DEFAULT_HOSTNAME ="127.0.0.1";
const int EyeClient::DEFAULT_PORTNUMBER = 3000;

EyeClient* EyeClient::getInstance(){
    if(clientInstance==NULL){
        clientInstance = new EyeClient();
    }
    return clientInstance;
}

EyeClient::EyeClient()
{
    this->hostName = DEFAULT_HOSTNAME;
}

EyeClient::EyeClient(string hostName){
    this->hostName = hostName;
    inetAddress.setPort(DEFAULT_PORTNUMBER);
    inetAddress.setHost(hostName);
}

EyeClient::~EyeClient()
{

}

string EyeClient::getHost(){
    return hostName;
}

void EyeClient::setHost(string hostName){
    this->hostName = hostName;
    //inetAddress.setHost(hostName);
}

void EyeClient::connect(){
    if (!inetAddress.setHost(hostName))
    {
            std::stringstream error_msg;
            error_msg << "Host argument, " << hostName << ", is invalid";
            throw std::runtime_error(error_msg.str());
    }
    tcpClient.connect(inetAddress);
}

void EyeClient::disconnect(){
    tcpClient.disconnect();
    cout<<"disconnected to the server"<<endl;
}

bool EyeClient::isConnected(){
    return tcpClient.isConnected();
}

std::vector<float>& EyeClient::getRotation(){
    dataPtr = tcpClient.receive(500);
    if(dataPtr){
        if(dataPtr->headOutputData()){
            directionX = dataPtr->headOutputData()->headRotation().at(X_DIRECTION);
            directionY = dataPtr->headOutputData()->headRotation().at(Y_DIRECTION);
            directionZ = dataPtr->headOutputData()->headRotation().at(Z_DIRECTION);
            printf("rotation data: x: %f  y: %f  z: %f\n",directionX,directionY,directionZ);
        }
    }
    float degreeX, degreeY, degreeZ;
    degreeX = directionX*180/M_PI;
    degreeY = directionY*180/M_PI;
    degreeZ = directionZ*180/M_PI;

    degreeVector.push_back(degreeX);
    degreeVector.push_back(degreeY);
    degreeVector.push_back(degreeZ);

    int x,y;
    x=degreeX/30*800+800;
    y=degreeY/30*450+450;
    printf("%i %i",x,y);
    return degreeVector;
}

float* EyeClient::getCoord(){
    dataPtr = tcpClient.receive(500);
    if(dataPtr){
        if(dataPtr->headOutputData()){
            directionX = dataPtr->headOutputData()->headRotation().at(X_DIRECTION);
            directionY = dataPtr->headOutputData()->headRotation().at(Y_DIRECTION);
            directionZ = dataPtr->headOutputData()->headRotation().at(Z_DIRECTION);
            printf("rotation data: x: %f  y: %f  z: %f\n",directionX,directionY,directionZ);
        }
    }
    float degreeX, degreeY, degreeZ;
    degreeX = directionX*180/M_PI;
    degreeY = directionY*180/M_PI;
    degreeZ = directionZ*180/M_PI;

    coord[1]=degreeX/30*ResolutionY/2+ResolutionY/2; // coord y
    coord[0]=degreeY/30*ResolutionX/2+ResolutionX/2; // coord x
    printf("%i %i",(int)coord[0],(int)coord[1]);
    return coord;
}

void EyeClient::setResolution(int x,int y){
    ResolutionX = x;
    ResolutionY = y;
}

void EyeClient::smooth(int degree){
    float* x = new float[degree];
    float* y = new float[degree];
    float* coord;
    for(int i=0;i<degree;i++){
        coord = getCoord();
        x[i] = coord[0];
        y[i] = coord[1];
    }
}
