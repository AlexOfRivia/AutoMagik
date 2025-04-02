#include "Car.h"


//Constructor
Car::Car()
{

}

//Destructor
Car::~Car()
{

}

//Setting the car make
void Car::setCarMake(std::string make)
{
    this->carMake = make;
}

//Setting the car model
void Car::setCarModel(std::string model)
{
    this->carModel = model;
}

//Setting the engine type
void Car::setEngineType(std::string engine)
{
    this->engineType = engine;
}

//Setting the production year
void Car::setProductionYear(int year)
{
    this->productionYear = year;
}

//Setting the car mileage
void Car::setCarMileage(int mileage)
{
    this->carMileage = mileage;
}

//Setting the client phone number
void Car::setClientPhoneNumber(int phoneNumber)
{
    this->clientPhoneNumber = phoneNumber;
}

std::string Car::getMake()
{
    return this->carMake;
}

std::string Car::getModel()
{
    return this->carModel;
}
