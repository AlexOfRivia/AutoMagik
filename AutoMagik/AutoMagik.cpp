#include "AutoMagik.h"

AutoMagik::AutoMagik(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //Showing the managerDashboardPage 
    QObject::connect(
        ui.managerModeButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(1); }
    );

	//Showing the workerDashboardPage
    QObject::connect(
        ui.workerModeButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(2); }
    );

	//Returning to the main page
    QObject::connect(
        ui.workerBackButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); }
    );

    //Returning to the main page
    QObject::connect(
        ui.managerBackButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); }
    );
}

AutoMagik::~AutoMagik()
{

}
