#pragma once
#include <string>
#include "Car.h"

class Task
{
public:
	Task(); //Constructor
	~Task(); //Destructor
private:
	std::string taskInstructions; //Instructions for the task
	std::string partsNeeded; //Parts needed for the task
	std::string comments; //Comments on the task
	int taskID = 0; //ID of the task
	Car taskCar; //Car to be worked on
};

