#pragma once
#include <string>
#include "Task.h" 

class Worker
{
public:
    Worker();
    ~Worker();

    //Setters
    void setWorkerID(int id);
    void setWorkerName(const std::string& name);
    void setPosition(const std::string& pos);
    void setWorkerSalary(int salary);
    void setWorkerExperience(int experience);
    void setWorkerAge(int age);
    void setClockedIn(bool status);
    void assignTask(const Task& task); //For assigning later
    void clearTask();                  //For clearing task assignment
    void setFirebaseKey(const std::string& key);
    void setEmail(const std::string& email);

    //Getters 
    int getWorkerID() const;
    std::string getWorkerName() const;
    std::string getPosition() const;
    int getWorkerSalary() const;
    int getWorkerExperience() const;
    int getWorkerAge() const;
    bool isClockedIn() const;
    std::string getAssignedTaskInfo() const; //Get task info
    std::string getFirebaseKey() const;
    std::string getEmail() const;


private:
    int workerID = 0; //ID of the worker
    std::string email;
    bool clockedIn; //Clocked in status
    std::string workerName; //Name of the worker
    std::string position; //Position of the worker
    int workerSalary; //Salary of the worker
    int workerExperience; //Experience of the worker (years)
    int workerAge; //Age of the worker
    Task assignedTask; //Task assigned to the worker
    bool hasTask = false; //Flag to check if a task is assigned
    std::string firebaseKey;
};