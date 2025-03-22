#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <vector>
#include "header/car.h"          
#include "header/task.h"
#include "header/worker.h"

class AutoMagik : public QMainWindow
{
    Q_OBJECT

public:
    AutoMagik(QWidget *parent = nullptr);
    ~AutoMagik();

private:
    Ui::AutoMagikClass ui;

    std::vector<car> cars; // Vector of cars
    std::vector<task> taks; // Vector of tasks
    std::vector<worker> workers; // Vector of workers
};
