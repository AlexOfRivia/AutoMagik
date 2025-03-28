#pragma once

#include <QtWidgets/QMainWindow>
#include <vector>
#include "ui_AutoMagik.h"
#include "Car.h"
#include "Task.h"
#include "Worker.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QSpacerItem>
#include <QComboBox>
#include <QException>

class AutoMagik : public QMainWindow
{
    Q_OBJECT

public:
    AutoMagik(QWidget *parent = nullptr);
    ~AutoMagik();

private:
    Ui::AutoMagikClass ui;

    void addTask(); //Adding task to table
	void addCar(); //Adding car to table
	void editSelectedTask(); //Editing selected task
	void assignReassignTask(); //Assigning or reassigning task
    void deleteTask(); //Deleting tasks from table

	std::vector<Car> cars; //Vector of cars
	std::vector<Task> tasks; //Vector of tasks
    std::vector<Worker> workers; //Vector of workers
};
