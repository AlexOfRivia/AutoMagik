#pragma once

#include <QtWidgets/QMainWindow>
#include <vector>
#include "ui_AutoMagik.h"
#include "Car.h"
#include "Task.h"
#include "Worker.h"

class AutoMagik : public QMainWindow
{
    Q_OBJECT

public:
    AutoMagik(QWidget *parent = nullptr);
    ~AutoMagik();

private:
    Ui::AutoMagikClass ui;

	std::vector<Car> cars; //Vector of cars
	std::vector<Task> taks; //Vector of tasks
    std::vector<Worker> workers; //Vector of workers
};
