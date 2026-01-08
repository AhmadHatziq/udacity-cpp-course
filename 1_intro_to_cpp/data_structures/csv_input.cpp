#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

int main() {
    /////////////////////////
    //////READ A CSV/////////
    /////////////////////////
    /*File content:
    001, -1.8956, 31.4567, -0.7419
    002, -1.0057, 45.2234, -0.6998
    003, -1.2235, 32.4488, 0.0241*/

    std::ifstream file("camera_values.csv"); 
    if (!file) {
            std::cerr << "Error: Could not open file." << std::endl;
            return 1;
        }

    std::string line;
    std::string serialNumber, roll, pitch, yaw;
    
    std::cout << "Serial Number | Roll | Pitch | Yaw" << std::endl;
    
    while(std::getline(file, line)) {
        std::stringstream ss(line); 

        std::getline(ss, serialNumber, ',');
        std::getline(ss, roll, ',');
        std::getline(ss, pitch, ',');
        std::getline(ss, yaw, ',');

        std::cout << serialNumber << " | " << roll << " | " << pitch << " | " << yaw << std::endl; 

    }
}