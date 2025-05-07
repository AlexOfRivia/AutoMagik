#include "Worker.h"
#include "Task.h" 
#include <string> 

//Constructor
Worker::Worker()
{
    this->clockedIn = false;
    this->workerID = 0; 
    this->workerSalary = 0;
    this->workerExperience = 0;
    this->workerAge = 0;
    this->hasTask = false;
}

//Destructor
Worker::~Worker()
{
    //Nothing dynamic to delete currently
}

void Worker::setWorkerID(int id) {
    this->workerID = id;
}

void Worker::setWorkerName(const std::string& name) {
    this->workerName = name;
}

void Worker::setPosition(const std::string& pos) {
    this->position = pos;
}

void Worker::setWorkerSalary(int salary) {
    this->workerSalary = salary;
}

void Worker::setWorkerExperience(int experience) {
    this->workerExperience = experience;
}

void Worker::setWorkerAge(int age) {
    this->workerAge = age;
}

void Worker::setClockedIn(bool status) {
    this->clockedIn = status;
}

void Worker::assignTask(const Task& task) {
    this->assignedTask = task;
    this->hasTask = true;
}

void Worker::clearTask() {
    this->assignedTask = Task(); 
    this->hasTask = false;
}

int Worker::getWorkerID() const {
    return this->workerID;
}

std::string Worker::getWorkerName() const {
    return this->workerName;
}

std::string Worker::getPosition() const {
    return this->position;
}

int Worker::getWorkerSalary() const {
    return this->workerSalary;
}

int Worker::getWorkerExperience() const {
    return this->workerExperience;
}

int Worker::getWorkerAge() const {
    return this->workerAge;
}

bool Worker::isClockedIn() const {
    return this->clockedIn;
}

std::string Worker::getAssignedTaskInfo() const {
    if (hasTask) 
    {
        return assignedTask.getTaskCar() + ", " + assignedTask.getTaskInstructions();
    }
    else {
        return "None";
    }
}