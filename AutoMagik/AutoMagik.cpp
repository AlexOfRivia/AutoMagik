#include "AutoMagik.h"

/*NOTES
- clicking the add comment button will add a new string into the comment string in the task object
- In the task widget on the left, there will be a task ID and car make and model
- In the cars screen, maybe add an option to see in how many task the car is currently in (?) 

TODO: 
- Create a Manager Car and Workers panel (stacked Widget probably)
- Add other button functionality*/

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
    QDialog dialog(this); //Initializing the dialog box
    dialog.setWindowTitle("Add Task"); //Setting name
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog); //Creating main layout

    QComboBox* carSelection = new QComboBox(&dialog); //Car selection combo box

    for (auto& car : cars) //for each car loop
    {
        carSelection->addItem(QString::fromStdString(car.getMake() + " " + car.getModel())); //Adding the car make and model to the combo box 
    }

    //Task instructions input
    QTextEdit* instructionsInput = new QTextEdit(&dialog);
    instructionsInput->setPlaceholderText("Enter task instructions");

    //Parts needed input
    QTextEdit* partsInput = new QTextEdit(&dialog);
    partsInput->setPlaceholderText("Enter needed parts");

    //Initial comments input
    QTextEdit* initialCommentsInput = new QTextEdit(&dialog);
    initialCommentsInput->setPlaceholderText("Enter comment:");

    //Car selection layout
    QVBoxLayout* carSelectionLayout = new QVBoxLayout();
    carSelectionLayout->addWidget(new QLabel("Select a Car to Work On:", &dialog));
    carSelectionLayout->addWidget(carSelection);

    //instructions and parts label layout
    QHBoxLayout* labelLayout1 = new QHBoxLayout(&dialog);
    labelLayout1->addWidget(new QLabel("Task Instructions:", &dialog));
    labelLayout1->addWidget(new QLabel("Parts Needed:", &dialog));

    //instructions and parts input layout
    QHBoxLayout* inputLayout1 = new QHBoxLayout(&dialog);
    inputLayout1->addWidget(instructionsInput);
    inputLayout1->addWidget(partsInput);

    //comments info
    QHBoxLayout* labelLayout2 = new QHBoxLayout(&dialog);
    labelLayout2->addWidget(new QLabel("Initial Comments:", &dialog));

    //make and model input layout
    QHBoxLayout* inputLayout2 = new QHBoxLayout(&dialog);
    inputLayout2->addWidget(initialCommentsInput);

    //Adding everything into the main layout
    mainLayout->addLayout(carSelectionLayout);
    mainLayout->addLayout(labelLayout1);
    mainLayout->addLayout(inputLayout1);
    mainLayout->addLayout(labelLayout2);
    mainLayout->addLayout(inputLayout2);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog); // Creating a button box
    mainLayout->addWidget(&buttonBox); // Adding the button box to the layout
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, [&]() 
        {
        if (instructionsInput->toPlainText().isEmpty() || partsInput->toPlainText().isEmpty()) //Checking for empty inputs
        {
            QMessageBox::warning(&dialog, "Input Error", "All fields must be filled out.");
        }
        else {
            dialog.accept();
        }
        });
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); //Connecting the button box to reject the dialog
    if (dialog.exec() == QDialog::Accepted) //Executing the dialog and checking if it was accepted
    {
        //Creatint new task
        Task* newTask = new Task;

        //Setting task parameters
        newTask->setTaskID(int(tasks.size()) + 1);
        newTask->setTaskInstructions(instructionsInput->toPlainText().toStdString());
        newTask->setPartsNeeded(partsInput->toPlainText().toStdString());
        newTask->setComments(initialCommentsInput->toPlainText().toStdString());
        newTask->setTaskCar(cars[carSelection->currentIndex()]);

        this->tasks.push_back(*newTask);

        ui.tasksTableWidget->setRowCount(static_cast<int>(tasks.size()));
        for (int i = 0; i < static_cast<int>(tasks.size()); i++)
        {
			//Setting items in the table
            ui.tasksTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(tasks[i].getTaskID())));
            ui.tasksTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(tasks[i].getTaskCar())));
            ui.tasksTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(tasks[i].getTaskInstructions())));
        }

        delete newTask; //Clearing memory
    }

    if (!tasks.empty())
    {
        //Set buttons as visible
        ui.deleteTaskButton->setEnabled(true);
        ui.editTaskButton->setEnabled(true);
        ui.assignTaskButton->setEnabled(true);
    }
    else {
        //Hide buttons
        ui.deleteTaskButton->setEnabled(false);
        ui.editTaskButton->setEnabled(false);
        ui.assignTaskButton->setEnabled(false);
    }
}

//Adding car to table
void AutoMagik::addCar()
{
    QDialog dialog(this); //Creating dialog box
    dialog.setWindowTitle("Add Task"); //Setting dialog name
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog); //Creating main layout


    //THIS COMBO BOX IS FOR THE API USAGE
	QComboBox* carSelection = new QComboBox(&dialog); //Car selection combo box

    //Make info input
    QTextEdit* makeInput = new QTextEdit(&dialog);
	makeInput->setPlaceholderText("Enter car make");
    makeInput->setStyleSheet("max-height: 35px");

    //Model info input
    QTextEdit* modelInput = new QTextEdit(&dialog);
    modelInput->setPlaceholderText("Enter car make");
    modelInput->setStyleSheet("max-height: 35px");

    //Mileage info input:
	QTextEdit* mileageInput = new QTextEdit(&dialog);
	mileageInput->setPlaceholderText("Enter car mileage");
    mileageInput->setStyleSheet("max-height: 35px");

    //Engine info input
    QTextEdit* engineInput = new QTextEdit(&dialog);
    engineInput->setPlaceholderText("Enter engine version");
    engineInput->setStyleSheet("max-height: 35px");

    //Year info input:
    QComboBox* yearInput = new QComboBox(&dialog);
    for (int i = 2025; i >= 1900; i--)
    {
        yearInput->addItem(QString::number(i));
    }
	
    //Phone number info input:
    QTextEdit* phoneNumberInput = new QTextEdit(&dialog);
	phoneNumberInput->setPlaceholderText("Enter client phone number");
    phoneNumberInput->setStyleSheet("max-height: 35px;");

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
	mainLayout->addWidget(new QLabel("Select a Car to Work On:", &dialog));
	mainLayout->addWidget(carSelection);
    mainLayout->addLayout(labelLayout1);
    mainLayout->addLayout(inputLayout1);
    mainLayout->addLayout(labelLayout2);
    mainLayout->addLayout(inputLayout2);
    mainLayout->addLayout(labelLayout3);
    mainLayout->addLayout(inputLayout3);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog); //Creating a button box
    mainLayout->addWidget(&buttonBox); //Adding the button box to the layout
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        if (makeInput->toPlainText().isEmpty() || modelInput->toPlainText().isEmpty() || mileageInput->toPlainText().isEmpty() || engineInput->toPlainText().isEmpty() || phoneNumberInput->toPlainText().isEmpty()) 
        {
            QMessageBox::warning(&dialog, "Input Error", "All fields must be filled out.");
        }
        else {
            dialog.accept();
        }
        });   
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); //Connecting the button box to reject the dialog
    if (dialog.exec() == QDialog::Accepted) //Executing the dialog and checking if it was accepted
    {
        Car *newCar = new Car(); //Initializing a new car object

        //Setting new car object parameters
        newCar->setCarMake(makeInput->toPlainText().toStdString());
        newCar->setCarModel(modelInput->toPlainText().toStdString());
        newCar->setEngineType(engineInput->toPlainText().toStdString());
        newCar->setCarMileage(mileageInput->toPlainText().toInt());
        newCar->setProductionYear(yearInput->currentText().toInt());
        newCar->setClientPhoneNumber(phoneNumberInput->toPlainText().toInt());

        this->cars.push_back(*newCar); //Adding new car object to the cars vector

        delete newCar; //Deleting the temporary new car object
    }

    if (!cars.empty())
    {
        //Show add task button
        ui.addTaskButton->setEnabled(true);
	} else {
        //Hide -||-
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

    if (tasks.empty())
    {
        //Hide all buttons task-related buttons when task vector is empty
        ui.deleteTaskButton->setEnabled(false);
        ui.editTaskButton->setEnabled(false);
        ui.assignTaskButton->setEnabled(false);
    } 
}
