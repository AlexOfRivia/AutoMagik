#pragma once

#include <QtWidgets/QMainWindow>
#include <vector>
#include "ui_AutoMagik.h"
#include "Firebase.h"
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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject> 
#include <QJsonArray>
#include <QUrlQuery>


class AutoMagik : public QMainWindow
{
    Q_OBJECT

public:
    AutoMagik(QWidget* parent = nullptr);
    ~AutoMagik();

private slots: /* slots are special member functions in Qt that can be connected to signals, when a signal is emitted,
               any connected slots are automatically invoked, imo useful */

    void addCar();
    void addWorker();
    void addTask();

    //Tasks
    void editSelectedTask();    //Placeholder
    void assignReassignTask();  //Placeholder
    void deleteTask();          //Placeholder

    //Cars
    void editSelectedCar();
    void deleteSelectedCar();

    //Workers
    void editSelectedWorker();
    void deleteSelectedWorker();


    void updateManagerTables();
    void updateWorkerDashboard();
    void updateWorkerDashboardSelection();
    void showCarInfo();         //Slot to trigger API calls
    void handleNhtsaReplyFinished();
    void handleApiNinjaReplyFinished();


private:
    Ui::AutoMagikClass ui;

    QNetworkAccessManager* networkManager; //Main manager
    QString apiKey;
    std::vector<Car> cars;
    std::vector<Task> tasks;
    std::vector<Worker> workers;
    int currentWorkerTaskIndex = -1; //Track selected task for worker

    QJsonObject lastNhtsaResult;
    QJsonObject lastApiNinjaResult;
    bool nhtsaRequestPending = false;
    bool apiNinjaRequestPending = false;
    QString lastMakeForDialog;      //Store details for the dialog/image search
    QString lastModelForDialog;
    int lastYearForDialog;

    void checkAndShowCombinedInfoDialog(); //Checks if requests are done, then calls display
    void displayCarInfoDialog();           //Creates and displays the actual info dialog

    Firebase firebase; //Creates firebase class for backend functionality
};