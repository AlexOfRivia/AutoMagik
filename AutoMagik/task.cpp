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
std::string Task::getTaskInstructions()
{
	return std::string();
}

//Getting the parts needed for the task
std::string Task::getPartsNeeded()
{
	return std::string();
}

//Getting the comments for the task
std::string Task::getComments()
{
	return std::string();
}

//Getting the ID for the task
int Task::getTaskID()
{
	return 0;
}

//Getting the car for the task
Car Task::getTaskCar()
{
	return Car();
}
