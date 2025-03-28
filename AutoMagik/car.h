#pragma once
#include <string>

class Car
{
public:
	Car(); //Constructor
	~Car(); //Destructor

private:
	std::string carMake; //Make of the car
	std::string carModel; //Model of the car
	std::string engineType; //Type of engine
	int productionYear; //Year the car was produced
	int carMileage; //Mileage of the car
	int clientPhoneNumber; //Phone number of the client



};

