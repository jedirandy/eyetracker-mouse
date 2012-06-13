#include "../include/EyeClient.h"

// static members
string EyeClient::hostName = "";
EyeClient* EyeClient::clientInstance = NULL;
int EyeClient::tcpPortnumber = 0;
const string EyeClient::DEFAULT_HOSTNAME ="127.0.0.1";
const int EyeClient::DEFAULT_PORTNUMBER = 3000;
bool EyeClient::bEngineStarted = false;
float EyeClient::filterData[3][2];

long EyeClient::timeStamp[3];
long EyeClient::bTimeStamp[2];
int EyeClient::blinkActionCount = 0;
int EyeClient::actionCount = 0;
bool EyeClient::bFiltered = true;

// constant
static const int BLINK_TIMEVAL = 500; // in milliseconds

// deprecated
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
    coord = new float[2];
    this->hostName = hostName;
    inetAddress.setPort(DEFAULT_PORTNUMBER);
    inetAddress.setHost(hostName);
    mouse = Mouse::getInstance();
}

EyeClient::~EyeClient()
{
    delete coord;
}

// Establishing of connection to the Facelab server
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

// set screen resolution
void EyeClient::setResolution(int x,int y){
    ResolutionX = x;
    ResolutionY = y;
}

void EyeClient::startEngine(){
    bEngineStarted = true;
    while(bEngineStarted){
        updateStatus();
    }
}

void EyeClient::stopEngine(){
    bEngineStarted = false;
}

// remote information
string EyeClient::getHost(){
    return hostName;
}

void EyeClient::setHost(string hostName){
    this->hostName = hostName;
    //inetAddress.setHost(hostName);
}

// get rotation data from facelab engine
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

// get the instant coordinates
float* EyeClient::getCoord(){
    dataPtr = tcpClient.receive(500);
    if(dataPtr){
        if(dataPtr->headOutputData()){
            directionX = dataPtr->headOutputData()->headRotation().at(X_DIRECTION);
            directionY = dataPtr->headOutputData()->headRotation().at(Y_DIRECTION);
            directionZ = dataPtr->headOutputData()->headRotation().at(Z_DIRECTION);
//            printf("rotation data: x: %f  y: %f  z: %f\n",directionX,directionY,directionZ);
        }
    }
    float degreeX, degreeY, degreeZ;
    degreeX = directionX*180/M_PI;
    degreeY = directionY*180/M_PI;
    degreeZ = directionZ*180/M_PI;

    // degrees to coordinates
    coord[1]=-degreeX/10*ResolutionY/2+ResolutionY/2; // coord Y
    coord[0]=-degreeY/10*ResolutionX/2+ResolutionX/2; // coord X
    if(coord[0]<=0){
        coord[0]=0;
    }else if(coord[0]>=ResolutionX){
        coord[0]=ResolutionX;
    }

    printf("coordinate X: %i  Y: %i\n",(int)coord[0],(int)coord[1]);
    return coord;
}

// update status and move the pointer
void EyeClient::updateStatus(){
    dataPtr = tcpClient.receive(500);
    EyeOutputDataLatestPtr eyeDataPtr = dataPtr->eyeOutputData();
    HeadTrackerStateOutputDataPtr headTrackerPtr = dataPtr->engineStateOutputData()->headtrackerStateOutputData();
    if(dataPtr){
            float leftEyeClosure = eyeDataPtr->eyeClosureOutputData()->eyeClosure(sm::eod::LEFT_EYE);
            float rightEyeClosure = eyeDataPtr->eyeClosureOutputData()->eyeClosure(sm::eod::RIGHT_EYE);
            if(testBlink(leftEyeClosure,rightEyeClosure)){
                cout<<"blinked"<<endl;
                if(triggerAction())
                    mouse->action(SINGLE_CLICK_LEFT);
            }
            if(eyeDataPtr->gazeOutputData()->gazeQualityLevel(sm::eod::RIGHT_EYE)!=0){ // if not tracking, gaze quality level = 0
                float * coord = new float(2);
                if(isFiltered())
                    coord = filter(getCoord(),3);
                else
                    coord = getCoord();
                mouse->move(coord[0],coord[1]);
           }
    }
}

// test the status of blink
bool EyeClient::testBlink(float l,float r){
    timeval time;
    if(blinkActionCount==0 && l>0.5 && r>0.5){
        gettimeofday(&time,NULL);
        bTimeStamp[blinkActionCount++] = convertTimeToMs(time);
        return false;
    }
    if(blinkActionCount==1 && l<0.5 && r<0.5){
        gettimeofday(&time,NULL);
        bTimeStamp[blinkActionCount] = convertTimeToMs(time);
        if(bTimeStamp[blinkActionCount]-bTimeStamp[blinkActionCount-1]<BLINK_TIMEVAL){
            blinkActionCount = 0;
            return true;
        }
    }
    return false;
}

// trigger the actions
// 2 blinks within 1000 ms will trigger single click left
bool EyeClient::triggerAction(){
    timeval time;
    if(actionCount==0){
        gettimeofday(&time,NULL);
        timeStamp[actionCount++] = convertTimeToMs(time);
        cout<<"timeStamp 0 : "<<timeStamp[actionCount]<<endl;
        return false;
    }
    if(actionCount==1){
        gettimeofday(&time,NULL);
        timeStamp[actionCount] = convertTimeToMs(time);
        cout<<"timeStamp 1 : "<<timeStamp[actionCount]<<endl;
        if(timeStamp[1]-timeStamp[0]>1000){
            cout<<"timed out"<<endl;
            actionCount = 0;
            return false;
        }else if(timeStamp[1]-timeStamp[0]<=1000 && timeStamp[1]-timeStamp[0]>=100){
            cout<<timeStamp[1]-timeStamp[0]<<endl;
           // usleep(100000);
            actionCount = 0;
            mouse->action(SINGLE_CLICK_LEFT);
            return true;
        }
    }
    return false;
}

// Filter functions
//
void EyeClient::setFilter(bool f){
    bFiltered = f;
}

bool EyeClient::isFiltered(){
    return bFiltered;
}

// n order low-pass filter
float * EyeClient::filter(float * coord,int order){
    int time_cst=5;

    filterData[0][0]=(coord[0]-filterData[0][0])/time_cst+filterData[0][0];
    filterData[0][1]=(coord[1]-filterData[0][1])/time_cst+filterData[0][1];

    for(int i=0;i<order-1;i++)
    {
        filterData[i+1][0]=(filterData[i][0]-filterData[i+1][0])/time_cst+filterData[i+1][0];
        filterData[i+1][1]=(filterData[i][1]-filterData[i+1][1])/time_cst+filterData[i+1][1];
    }
    return filterData[order-1];
}

// get current time in milliseconds
double EyeClient::convertTimeToMs(struct timeval& tv){
    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}
