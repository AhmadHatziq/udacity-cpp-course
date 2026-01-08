#include <iostream>
#include <fstream>
#include <string>

int main() {
    /////////////////////////
    //////READ A TXT/////////
    /////////////////////////
    /*File content:
    001 -1.8956 31.4567 -0.7419
    002 -1.0057 45.2234 -0.6998
    003 -1.2235 32.4488 0.0241
    */
    std::ifstream file("camera_values.txt");
    if (!file) {
    	std::cerr << "Error: Could not open file." << std::endl;
    	return 1;
	}

    // Cameras class
    // camera objects
    std::string serialNumber, roll, pitch, yaw;

    std::cout << "Serial Number | Roll | Pitch | Yaw" << std::endl;

    // Read each line from the file and process is
    while (file >> serialNumber >> roll >> pitch >> yaw ) {
        std::cout << serialNumber << " | " << roll << " | " << pitch << " | " << yaw << std::endl;
    }
}