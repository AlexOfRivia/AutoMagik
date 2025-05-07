#pragma once
#include <string>
#include "Car.h"


//Status enum
enum status
{
	NEW,
	IN_PROGRESS,
	COMPLETED,
	CANCELLED
};

//Priority status
enum priority
{
	LOW,
	MEDIUM,
	HIGH
};

class Task
{
public:
	Task(); //Constructor
	~Task(); //Destructor
	
	//Setters
	void setTaskInstructions(std::string instructions); //Set the instructions for the task
	void setPartsNeeded(std::string parts); //Set the parts needed for the task
	void setComments(std::string comments); //Set the comments for the task
	void setTaskID(int ID); //Set the ID for the task
	void setTaskCar(Car car); //Set the car for the task
	void setTaskWorkerID(int id); //Set the ID of the worker assigned to the task
	void setTaskStatus(status stat); //Set the status of the task;
	void setTaskPriority(priority prio); //Set the priority of the task 

	//Getters
	std::string getTaskInstructions() const; //Get the instructions for the task
	std::string getPartsNeeded() const; //Get the parts needed for the task
	std::string getComments() const; //Get the comments for the task
	int getTaskID() const; //Get the ID for the task
	int getTaskWorkerID() const; //Get the ID of the worker assigned to the task
	std::string getTaskCar() const; //Get the car for the task (Make + Model string)
	const Car& getCarObject() const; //Get the actual Car object
	std::string getTaskStatus() const; //Get the status of the task
	std::string getTaskPriority() const; //Get the priority of the task


private:
	std::string taskInstructions; //Instructions for the task
	std::string partsNeeded; //Parts needed for the task
	std::string comments; //Comments on the task
	int taskID = 0; //ID of the task
	Car taskCar; //Car to be worked on
	int workerID=0; //ID of the worker assigned to the task
	status taskStatus; //Status of the task
	priority taskPriority; //Priority of the task
};