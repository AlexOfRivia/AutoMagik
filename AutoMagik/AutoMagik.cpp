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

    QObject::connect(
		ui.addTaskButton, &QPushButton::clicked, this, &AutoMagik::addTask
    );

    QObject::connect(
        ui.addCarButton, &QPushButton::clicked, this, &AutoMagik::addCar
    );
}

AutoMagik::~AutoMagik()
{

}

//Adding task to table
void AutoMagik::addTask()
{
    QDialog dialog(this);
	dialog.setWindowTitle("Add Task");
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);


}

//Adding car to table
void AutoMagik::addCar()
{
    QDialog dialog(this); //Creating dialog box
    dialog.setWindowTitle("Add Task"); //Setting dialog name
    dialog.setFixedSize(425,250);
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog); //Creating main layout

    //Make info input
    QTextEdit* makeInput = new QTextEdit(&dialog);
	makeInput->setPlaceholderText("Enter car make");
    makeInput->setStyleSheet("max-height: 30px");

    //Model info input
    QTextEdit* modelInput = new QTextEdit(&dialog);
    modelInput->setPlaceholderText("Enter car make");
    modelInput->setStyleSheet("max-height: 30px");

    //Mileage info input:
	QTextEdit* mileageInput = new QTextEdit(&dialog);
	mileageInput->setPlaceholderText("Enter car mileage");
    mileageInput->setStyleSheet("max-height: 30px");

    //Engine info input
    QTextEdit* engineInput = new QTextEdit(&dialog);
    engineInput->setPlaceholderText("Enter engine version");
    engineInput->setStyleSheet("max-height: 30px");

    //Year info input:
    QComboBox* yearInput = new QComboBox(&dialog);
    for (int i = 2025; i >= 1900; i--)
    {
        yearInput->addItem(QString::number(i));
    }
	
    //Phone number info input:
    QTextEdit* phoneNumberInput = new QTextEdit(&dialog);
	phoneNumberInput->setPlaceholderText("Enter client phone number");
    phoneNumberInput->setStyleSheet("max-height: 30px;max-width: 195px");

    //Spcaer
	QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    //make and model label layout
    QHBoxLayout* labelLayout1 = new QHBoxLayout(&dialog);
    labelLayout1->addWidget(new QLabel("Car make:", &dialog));
    labelLayout1->addWidget(new QLabel("Car model:", &dialog));

    //make and model input layout
    QHBoxLayout* inputLayout1 = new QHBoxLayout(&dialog);
    inputLayout1->addWidget(makeInput);
    inputLayout1->addWidget(modelInput);

    //engine and mileage info
	QHBoxLayout* labelLayout2 = new QHBoxLayout(&dialog);
    labelLayout2->addWidget(new QLabel("Engine:", &dialog));
    labelLayout2->addWidget(new QLabel("Mileage (in km):", &dialog));

    //make and model input layout
    QHBoxLayout* inputLayout2 = new QHBoxLayout(&dialog);
    inputLayout2->addWidget(engineInput);
    inputLayout2->addWidget(mileageInput);

    //Car year and client phone number info
    QHBoxLayout* labelLayout3 = new QHBoxLayout(&dialog);
    labelLayout3->addWidget(new QLabel("Production year:", &dialog));
    labelLayout3->addWidget(new QLabel("Owner phone number:", &dialog));

    //Car year and client phone number input layout
    QHBoxLayout* inputLayout3 = new QHBoxLayout(&dialog);
    inputLayout3->addWidget(yearInput);
	inputLayout3->addItem(spacer);
    inputLayout3->addWidget(phoneNumberInput);
   
    //Adding everything into the main layout
    mainLayout->addLayout(labelLayout1);
    mainLayout->addLayout(inputLayout1);
    mainLayout->addLayout(labelLayout2);
    mainLayout->addLayout(inputLayout2);
    mainLayout->addLayout(labelLayout3);
    mainLayout->addLayout(inputLayout3);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog); // Creating a button box
    mainLayout->addWidget(&buttonBox); // Adding the button box to the layout
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept); // Connecting the button box to accept the dialog
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); // Connecting the button box to reject the dialog
    if (dialog.exec() == QDialog::Accepted) // Executing the dialog and checking if it was accepted
    {
        return;
    }

    if (!cars.empty())
    {
        ui.addTaskButton->setEnabled(true);
	} else {
        ui.addTaskButton->setEnabled(false);
    }


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
