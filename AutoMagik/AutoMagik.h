#pragma once

#include <QtWidgets/QMainWindow>
#include <vector>
#include "ui_AutoMagik.h"
#include "car.h"
#include "task.h"
#include "worker.h"

class AutoMagik : public QMainWindow
{
    Q_OBJECT

public:
    AutoMagik(QWidget *parent = nullptr);
    ~AutoMagik();

private:
    Ui::AutoMagikClass ui;

	std::vector<car> cars; //Vector of cars
	std::vector<task> taks; //Vector of tasks
    std::vector<worker> workers; //Vector of workers
};
