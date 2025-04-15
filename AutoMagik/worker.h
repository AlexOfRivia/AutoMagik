#pragma once
#include <string>
#include "Task.h"
class Worker
{
public:
	Worker(); //Constructor
	~Worker(); //Destructor
	void setTask(); //Set the task for the worker

private:
	int workerID=0; //ID of the worker
	bool clockedIn; //Clocked in status
	std::string workerName; //Name of the worker
	std::string position; //Position of the worker
	int workerSalary; //Salary of the worker
	int workerExperience; //Experience of the worker
	int workerAge; //Age of the worker
	Task assignedTask; //Task assigned to the worker
};

