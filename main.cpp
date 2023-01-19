
#include <iostream>
#include <string>
#include <algorithm>
#include <windows.h>

using namespace std;

static const char *enumStrings[] = { 
    "low",
    "high",
    "emergency",
    "pedestrian"
};

string enum_to_string (int state)
{
    string My_String(enumStrings[state]);
    return My_String;
}



class Sensors{

    friend class Intersection;
    friend class Vehicle;

    protected:
    int in_line[4]; // the line that comes into intersection (1 to 4)
    int out_line[4]; // the line that leaves the intersection (5 to 8)
    bool pedestrian; // 1 if pedestrian crosses
    bool emergency; // 1 if emergency vehicle crosses

    public:
    Sensors();
    Sensors(int*, int*, bool, bool); // constructor
    bool pedestrianCross(); // if pedestrian is seen at crossing point
    bool emergencyCross(); // if emergency vehicle is crossing in 500 meter radius
    bool measureCongestion(); // if the congestion in coming line is more than 20, return 1
    int* getInLine();
};

class Vehicle{

    friend class Intersection;
    friend class Sensors;

    protected: 
    int id; // 1 if emergency, 0 if not
    int distance; // distance from intersection
    int speed;
    int currentPosition; // 1 to 4 clockwise for lanes
    int destination; // 1 to 4 clockwise for lanes

    public:
    Vehicle();
    Vehicle(int, int, int, int, int); // constructor
    ~Vehicle(); // destructor
    bool getId();
    int getSpeed();
    int getCurrentPosition();
    int getDestination();
};

class Intersection{

    friend class Sensors;
    friend class Vehicle;

    protected:
    int newSpeed;
    Sensors sensor;
    Vehicle vehicle;

    public:
    Intersection(int, Sensors, Vehicle); // constructor
    string status(); // shows the states of our system
    void setNewSpeed();
    int getNewSpeed();
    void message(); // the message that is sent to vehicle, includes speed 

};

Intersection::Intersection(int ns, Sensors s, Vehicle v){
    newSpeed = ns;
    sensor = s;
    vehicle = v;
}

void Intersection::setNewSpeed(){

    status();
    
    if(status()=="low"){

        if(vehicle.getId() == 1){
            newSpeed = 80;
        }

        else if(vehicle.getDestination() == vehicle.getCurrentPosition()+3)
        newSpeed = vehicle.getSpeed();

        else if(sensor.getInLine()[vehicle.getDestination()]>=1){
        newSpeed = (3/4)*vehicle.getSpeed();
        }

    }
    
    if(status()=="high"){

        int max_index = std::distance(sensor.getInLine(), max_element(sensor.getInLine(), sensor.getInLine()+4));
        int timer[4];
        copy(sensor.getInLine(),sensor.getInLine()+4,timer);
        for(int i=0;i<4;i++){
            timer[i] = timer[i] * 3;
            if(timer[i]>= 50){
                timer[i] = 50;
            }
        }

        int index_number = vehicle.getCurrentPosition();

        if(vehicle.getId() == 1){
            newSpeed = 80;
        }

        else if(index_number == max_index){
            newSpeed = 80;
        }

        else if(index_number-1 == max_index){
            newSpeed = 0;
            cout << "wait for " << timer[index_number] << " seconds";
        }

        else if(index_number-2 == max_index){
            newSpeed = 0;
            cout << "wait for " << timer[index_number] << " seconds";
        }

        else if(index_number-3 == max_index){
            newSpeed = 0;
            cout << "wait for " << timer[index_number] << " seconds";
        }

        else{
            newSpeed = 0;
        }
        
    }

    if(status()=="emergency"){
        newSpeed = 80;
    }
    
    if(status()=="pedestrian"){
        newSpeed = 0;
    }

    message();
}

int Intersection::getNewSpeed(){

    return newSpeed;

}

string Intersection::status(){

enum state{
    low,
    high,
    emergency,
    pedestrian
}mystate;

switch (mystate){
    
    case emergency:
    if(!sensor.emergencyCross()){
        mystate = low;
    }
    break;

    case pedestrian:
    if(!sensor.pedestrianCross()){
        mystate = low;
    }
    break;

    case high:
    if(sensor.pedestrianCross()){
        mystate = pedestrian;
    }
    
    if(sensor.emergencyCross()){
        mystate = emergency;
    }
    if(!sensor.measureCongestion()){
        mystate = low;
    }
    break;
    
    case low:
    if(sensor.pedestrianCross()){
        mystate = pedestrian;
    }
    if(sensor.emergencyCross()){
        mystate = emergency;
    }
    if(sensor.measureCongestion()){
        mystate = high;
    }
    break;

    default:
    mystate = low;

}

    return enum_to_string(mystate);

}

void Intersection::message(){

    cout << "---the car on lane " << vehicle.getCurrentPosition() << " with speed " << vehicle.getSpeed() << " wants to go to lane " << 
    vehicle.getDestination() << endl;
    cout << "change your speed to " << getNewSpeed() << endl;

}

Sensors::Sensors(){
  std::fill(in_line, in_line+4, 0); 
  std::fill(out_line, out_line+4, 0);
  pedestrian = false;
  emergency = false;
}

Sensors::Sensors(int il[4],int ol[4], bool p, bool e){
    copy(il,il+4,in_line);
    copy(il,il+4,in_line);
    pedestrian = p;
    emergency = e;
}

bool Sensors::measureCongestion(){

    int sum = 0;
    
    for(int i=0;i<4;i++){
        sum += in_line[i];
    }

    if(sum>=20){
        return 1;
    }
    else{
        return 0;
    }

}


bool Sensors::pedestrianCross(){
    
    return pedestrian;
}

bool Sensors::emergencyCross(){

    return emergency;

}

int* Sensors::getInLine(){
    return in_line;
}

Vehicle::Vehicle(){

id = 0;
distance = 0;
speed = 0;
currentPosition = 0;
destination = 0;

}

Vehicle::Vehicle(int i, int dis, int s, int cp, int d){

id = i;
distance = dis;
speed = s;
currentPosition = cp;
destination = d;

}

Vehicle::~Vehicle(){}

bool Vehicle::getId(){

    return id;
}

int Vehicle::getSpeed(){

    return speed;
}

int Vehicle::getCurrentPosition(){

    return currentPosition;
}

int Vehicle::getDestination(){

    return destination;
}

int main(){

    int ilarr[4]={2,1,1,1};
    int olarr[4]={2,0,3,0};
    Sensors sensor(ilarr, olarr, false, false);

    sensor.emergencyCross();
    sensor.pedestrianCross();
    sensor.measureCongestion();
    if(sensor.measureCongestion()){
        cout << "the intersection is congested" << endl;
    }
    else{
        cout << "the intersection is not congested" << endl;
    }
    sensor.getInLine();

    Vehicle vehicle1(0, 10, 50, 1, 7);
    Vehicle vehicle2(0, 20, 80, 3, 8);
    Vehicle vehicle3(0, 10, 60, 4, 5);
    Vehicle vehicle4(1, 20, 30, 1, 6);
    Vehicle vehicle5(0, 10, 70, 2, 8);

    vehicle1.getId();
    vehicle1.getSpeed();
    vehicle1.getCurrentPosition();
    vehicle1.getDestination();

    Intersection intersection1 (0,sensor,vehicle1);
    intersection1.setNewSpeed();

    vehicle2.getId();
    vehicle2.getSpeed();
    vehicle2.getCurrentPosition();
    vehicle2.getDestination();

    Intersection intersection2 (0,sensor,vehicle2);
    intersection2.setNewSpeed();

    vehicle3.getId();
    vehicle3.getSpeed();
    vehicle3.getCurrentPosition();
    vehicle3.getDestination();

    Intersection intersection3 (0,sensor,vehicle3);
    intersection3.setNewSpeed();

    vehicle4.getId();
    vehicle4.getSpeed();
    vehicle4.getCurrentPosition();
    vehicle4.getDestination();

    Intersection intersection4 (0,sensor,vehicle4);
    intersection4.setNewSpeed();

    vehicle5.getId();
    vehicle5.getSpeed();
    vehicle5.getCurrentPosition();
    vehicle5.getDestination();

    Intersection intersection5 (0,sensor,vehicle5);
    intersection5.setNewSpeed();

}