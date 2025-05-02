#include "Task.h"


//Constructor
Task::Task()
{

}

//Destructor
Task::~Task()
{

}

//Setting the instructions for the task
void Task::setTaskInstructions(std::string instructions)
{
	this->taskInstructions = instructions;
}

//Setting the parts needed for the task
void Task::setPartsNeeded(std::string parts)
{
	this->partsNeeded = parts;
}

//Setting the comments for the task
void Task::setComments(std::string comments)
{
	this->comments = comments;
}

//Setting the ID for the task
void Task::setTaskID(int ID)
{
	this->taskID = ID;
}

//Setting the car for the task
void Task::setTaskCar(Car car)
{
	this->taskCar = car;
}

//Getting the instructions for the task
std::string Task::getTaskInstructions() const
{
	return this->taskInstructions;
}

//Getting the parts needed for the task
std::string Task::getPartsNeeded() const
{
	return this->partsNeeded;
}

//Getting the comments for the task
std::string Task::getComments() const
{
	return this->comments;
}

//Getting the ID for the task
int Task::getTaskID() const
{
	return this->taskID;
}

//Getting the car for the task
std::string Task::getTaskCar() const
{
	return this->taskCar.getMake() + " " + this->taskCar.getModel();
}
