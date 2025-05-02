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
#include <QMessageBox>
#include <QLineEdit>     
#include <QFormLayout>   

class AutoMagik : public QMainWindow
{
    Q_OBJECT

public:
    AutoMagik(QWidget* parent = nullptr);
    ~AutoMagik();

private slots: /*slots are special member functions in Qt that can be connected to signals, when a signal is emitted,
               any connected slots are automatically invoked, imo useful*/

    void addCar();
    void addWorker(); 
	void addTask();
    void editSelectedTask(); //Placeholder
    void assignReassignTask(); //Placeholder
    void deleteTask(); //Placeholder
    void updateManagerTables(); 

private:
    Ui::AutoMagikClass ui;

    std::vector<Car> cars;
    std::vector<Task> tasks;
    std::vector<Worker> workers;
};