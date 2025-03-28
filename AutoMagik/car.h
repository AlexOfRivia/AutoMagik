#pragma once
#include <string>

class Car
{
public:
	Car(); //Constructor
	~Car(); //Destructor

	void setCarMake(std::string make); //Setting the make of the car
	void setCarModel(std::string model); //Setting the model of the car
	void setEngineType(std::string engine); //Setting the engine type of the car
	void setProductionYear(int year); //Setting the production year of the car
	void setCarMileage(int mileage); //Setting the mileage of the car
	void setClientPhoneNumber(int phoneNumber); //Setting the client phone number


private:
	std::string carMake; //Make of the car
	std::string carModel; //Model of the car
	std::string engineType; //Type of engine
	int productionYear; //Year the car was produced
	int carMileage; //Mileage of the car
	int clientPhoneNumber; //Phone number of the client



};

