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

//Setters
std::string Car::getMake() const
{
    return this->carMake;
}

std::string Car::getModel() const
{
    return this->carModel;
}

std::string Car::getEngineType() const
{
    return this->engineType;
}

int Car::getProductionYear() const
{
    return this->productionYear;
}

int Car::getCarMileage() const
{
    return this->carMileage;
}

int Car::getClientPhoneNumber() const
{
    return this->clientPhoneNumber;
}

bool Car::operator==(const Car& other) const
{
    return carMake == other.carMake &&
        carModel == other.carModel &&
        engineType == other.engineType &&
        productionYear == other.productionYear &&
        carMileage == other.carMileage &&
        clientPhoneNumber == other.clientPhoneNumber;
}
