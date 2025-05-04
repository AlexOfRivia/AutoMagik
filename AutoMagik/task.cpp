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

//Setting the status of the task
void Task::setTaskStatus(status stat)
{
	this->taskStatus = stat;
}

//Setting the priority of the task
void Task::setTaskPriority(priority prio)
{
	this->taskPriority = prio;
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

//Getting the actual car object for the task
const Car& Task::getCarObject() const
{
	return this->taskCar;
}

//Getting the status of the task
std::string Task::getTaskStatus() const
{
	switch (this->taskStatus) 
	{
	case NEW: return "New";
	case IN_PROGRESS: return "In Progress";
	case COMPLETED: return "Completed";
	case CANCELLED: return "Cancelled";
	default: return "Unknown";
	}
}

//Getting the priority of the task
std::string Task::getTaskPriority() const
{
	switch (this->taskPriority)
	{
	case LOW: return "Low";
	case MEDIUM: return "Medium";
	case HIGH: return "High";
	default: return "Unknown";
	}
}