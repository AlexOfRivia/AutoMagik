#include "AutoMagik.h"

/*NOTES
- Car Info in the dashboard will be all car object info in the task object
- task info will be instructions and parts needed from the object
- clicking the add comment button will add a new string into the comment string in the task object
- In the task widget on the left, there will be a task ID and car make and model*/

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

//Adding task to table
void AutoMagik::addTask()
{

}

//Editing selected task
void AutoMagik::editSelectedTask()
{

}

//Assigning or reassigning task
void AutoMagik::assignReassignTask()
{

}

//Deleting selected task
void AutoMagik::deleteTask()
{

}
