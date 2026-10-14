#ifndef VEHICLE_H
#define VEHICLE_H

#include <memory>
#include "TrafficObject.h"
#include <atomic>

// forward declarations to avoid include cycle
class Street;
class Intersection;

class Vehicle : public TrafficObject, 
  // std::enable_shared_from_this<Vehicle> enables a Vehicle object 
  // to obtain a std::shared_ptr<Vehicle> that shares ownership via "shared_from_this();"
  public std::enable_shared_from_this<Vehicle> 
                                               
  /*
    auto self = shared_from_this(); // Auto is a shared_pointer to this Vehicle object 
  */

{
public:
    // constructor / desctructor
    Vehicle();
    ~Vehicle() { _stopRequested = true; } // Destructor will set the flag to True 

    // getters / setters
    void setCurrentStreet(std::shared_ptr<Street> street) { _currStreet = street; };
    void setCurrentDestination(std::shared_ptr<Intersection> destination);

    // typical behaviour methods
    void simulate();

    // miscellaneous
    std::shared_ptr<Vehicle> get_shared_this() { return shared_from_this(); }

    // request drive thread to stop
    void stop() { _stopRequested = true; }

private:
    // typical behaviour methods
    void drive();

    std::shared_ptr<Street> _currStreet;            // street on which the vehicle is currently on
    std::shared_ptr<Intersection> _currDestination; // destination to which the vehicle is currently driving
    double _posStreet;                              // position on current street
    double _speed;                                  // ego speed in m/s
    std::atomic<bool> _stopRequested{false};
};

#endif