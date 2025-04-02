#pragma once
#include <string>
#include "Car.h"

class Task
{
public:
	Task(); //Constructor
	~Task(); //Destructor

	void setTaskInstructions(std::string instructions); //Set the instructions for the task
	void setPartsNeeded(std::string parts); //Set the parts needed for the task
	void setComments(std::string comments); //Set the comments for the task
	void setTaskID(int ID); //Set the ID for the task
	void setTaskCar(Car car); //Set the car for the task

	std::string getTaskInstructions(); //Get the instructions for the task
	std::string getPartsNeeded(); //Get the parts needed for the task
	std::string getComments(); //Get the comments for the task
	int getTaskID(); //Get the ID for the task
	Car getTaskCar(); //Get the car for the task


private:
	std::string taskInstructions; //Instructions for the task
	std::string partsNeeded; //Parts needed for the task
	std::string comments; //Comments on the task
	int taskID = 0; //ID of the task
	Car taskCar; //Car to be worked on
};

