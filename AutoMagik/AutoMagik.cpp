#include "AutoMagik.h"
#include <QTextEdit>    
#include <QLabel>       
#include <QFormLayout>  
#include <QLineEdit>     
#include <QSpinBox>     
#include <QVBoxLayout>   
#include <QHBoxLayout>   
#include <QComboBox>    
#include <QDialogButtonBox>
#include <QMessageBox>   
#include <QDate>         

/*
TODO: 
- In the task widget on the left, add a display for task ID and car make and model
- Add firebase functionality
- Add other button functionality (added "add worker" button functionality)
- Add setting tasks, add worker dashboard "timeline" for tasks 
- Implement register and login logic (you can register as anything now - even typing nothing in the textbox, adding regex for checking if email is valid would also be great)
- Implement selection logic for buttons (edit, delete, etc.)
- Implement buttons for things from api (and succesively api) in worker dashboard
- In the workers screen, maybe add a display of how many tasks the worker has in total
- In the cars screen, maybe add an option to see in how many tasks have to be performed on the car
*/


AutoMagik::AutoMagik(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	ui.stackedWidget->setCurrentIndex(0); //Setting the current index to 0 (Home Menu)


    //Mode Selection
    QObject::connect(ui.managerModeButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(3); }); //Manager Login Page
    QObject::connect(ui.workerModeButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(1); });   //Worker Login Page


//Manager
    //Manager Login Page
    QObject::connect(ui.backButton1, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); });          //Back to Mode Selection
    QObject::connect(ui.managerLoginButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(4); });   //To Manager Tasks Page (need to implement login logic)
    QObject::connect(ui.createManagerAccButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(2); }); //To Manager Register Page
    //Manager Register Page
    QObject::connect(ui.backButton3, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(3); });         //Back to Manager Login
    QObject::connect(ui.registerButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(4); });      //To Manager Tasks Page (need to implement register logic)
    //Manager Cars Page
    QObject::connect(ui.managerBackButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); }); //Sign Out -> Mode Selection
    QObject::connect(ui.tasksButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(4); });       //To Tasks Page
    QObject::connect(ui.workersButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(7); });    //To Workers Page
    QObject::connect(ui.addCarButton, &QPushButton::clicked, this, &AutoMagik::addCar);                             //Add Car Action
    //Manager Tasks Page
    QObject::connect(ui.managerBackButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); });  //Blah blah blah you get it, im not gonna comment this much XD
    QObject::connect(ui.carsButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(6); });       
    QObject::connect(ui.workersButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(7); });     
    QObject::connect(ui.addTaskButton, &QPushButton::clicked, this, &AutoMagik::addTask);                              
    //Manager Workers Page
    QObject::connect(ui.managerBackButton3, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); }); 
    QObject::connect(ui.carsButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(6); });     
    QObject::connect(ui.tasksButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(4); });    
    QObject::connect(ui.addWorkerButton, &QPushButton::clicked, this, &AutoMagik::addWorker);                      


//Worker
    //Worker Login Page
    QObject::connect(ui.backButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); });         //Back to Mode Selection
    QObject::connect(ui.workerLoginButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(5); });   //To Worker Dashboard (need to implement login logic)
    //Worker Dashboard Page
    QObject::connect(ui.workerBackButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); });  //Sign Out -> Mode Selection
   
   

    //Initial button states and table refresh
    updateManagerTables();

    //Hide selection-based buttons initially
    ui.markCompleteButton->setEnabled(false);
    ui.addCommentButton->setEnabled(false);


    //Also, Edit and Delete buttons are handled by updateManagerTables
}

AutoMagik::~AutoMagik()
{

}

//Helper function to update tables and buttons
void AutoMagik::updateManagerTables()
{
    //Update Workers Table
    ui.workersTableWidget->setRowCount(0); //Clear existing rows first
    ui.workersTableWidget->setRowCount(static_cast<int>(workers.size()));
    for (int i = 0; i < static_cast<int>(workers.size()); ++i)
    {
        const Worker& worker = workers[i]; 
        ui.workersTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(worker.getWorkerID())));
        ui.workersTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(worker.getWorkerName())));
        ui.workersTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(worker.getPosition())));
        ui.workersTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(worker.getWorkerExperience()) + " yrs"));
        ui.workersTableWidget->setItem(i, 4, new QTableWidgetItem("$ " + QString::number(worker.getWorkerSalary())));
        ui.workersTableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(worker.getWorkerAge())));
        ui.workersTableWidget->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(worker.getAssignedTaskInfo()))); //Display task info
    }
    ui.workersTableWidget->resizeColumnsToContents();

    //Update Tasks Table
    //Clear and re-add tasks similar to workers, for example when worker assignment changes
    ui.tasksTableWidget->setRowCount(0);
    ui.tasksTableWidget->setRowCount(static_cast<int>(tasks.size()));
    for (int i = 0; i < static_cast<int>(tasks.size()); ++i)
    {
        const Task& task = tasks[i];
        ui.tasksTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(task.getTaskID())));
        ui.tasksTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(task.getTaskCar())));
        ui.tasksTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(task.getTaskInstructions())));
        ui.tasksTableWidget->setItem(i, 3, new QTableWidgetItem("New")); //Placeholder Status
        ui.tasksTableWidget->setItem(i, 4, new QTableWidgetItem("Unassigned")); //Placeholder Worker
        ui.tasksTableWidget->setItem(i, 5, new QTableWidgetItem("Normal")); //Placeholder Priority
    }
    ui.tasksTableWidget->resizeColumnsToContents();


    //Update Cars Table
    //Clear and re-add if needed
    ui.carsTableWidget->setRowCount(0);
    ui.carsTableWidget->setRowCount(static_cast<int>(cars.size()));
    for (int i = 0; i < static_cast<int>(cars.size()); ++i)
    {
        const Car& car = cars[i];
        ui.carsTableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(car.getMake())));
        ui.carsTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(car.getModel())));
        ui.carsTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(car.getEngineType())));
        ui.carsTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(car.getProductionYear())));
        ui.carsTableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(car.getCarMileage()) + " km"));
        ui.carsTableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(car.getClientPhoneNumber())));
    }
    ui.carsTableWidget->resizeColumnsToContents(); 


    //Update button states
    bool hasWorkers = !workers.empty();
    bool hasTasks = !tasks.empty();
    bool hasCars = !cars.empty();

    //Buttons requiring selection (implement selection logic later)
    ui.editWorkerButton->setEnabled(hasWorkers); //Enable if workers exist
    ui.deleteWorkerButton->setEnabled(hasWorkers); 

    ui.editTaskButton->setEnabled(hasTasks);     //Enable if tasks exist
    ui.deleteTaskButton->setEnabled(hasTasks);    

    ui.editCarButton->setEnabled(hasCars);       //Enable if cars exist
    ui.deleteCarButton->setEnabled(hasCars);      

    //Other buttons
    ui.assignTaskButton->setEnabled(hasWorkers && hasTasks); //Need both workers and tasks to assign
    ui.addTaskButton->setEnabled(hasCars); //Can only add task if cars exist
}

//Add New Worker
void AutoMagik::addWorker()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Worker");
    dialog.setMinimumWidth(400); 

    //Dialog content layout
    QFormLayout* formLayout = new QFormLayout(&dialog);

    //Inputs
    QLineEdit* nameInput = new QLineEdit(&dialog);
    QLineEdit* positionInput = new QLineEdit(&dialog);
    QSpinBox* experienceInput = new QSpinBox(&dialog);
    experienceInput->setSuffix(" years");
    experienceInput->setRange(0, 60); //Example/placeholder range, change if needed (1)
    QSpinBox* salaryInput = new QSpinBox(&dialog);
    salaryInput->setPrefix("$ ");
    salaryInput->setRange(0, 500000); //Example/placeholder range, change if needed (2)
    salaryInput->setSingleStep(1000);
    QSpinBox* ageInput = new QSpinBox(&dialog);
    ageInput->setRange(18, 100); //Example/placeholder range, change if needed (3)

    //Rows in the form layout
    formLayout->addRow(new QLabel("Worker Name:", &dialog), nameInput);
    formLayout->addRow(new QLabel("Position:", &dialog), positionInput);
    formLayout->addRow(new QLabel("Experience:", &dialog), experienceInput);
    formLayout->addRow(new QLabel("Salary:", &dialog), salaryInput);
    formLayout->addRow(new QLabel("Age:", &dialog), ageInput);

    //OK/Cancel buttons
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout->addRow(&buttonBox); 

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        //Checks
        if (nameInput->text().trimmed().isEmpty() || positionInput->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Input Error", "Worker Name and Position cannot be empty.");
            //Don't accept yet
        }
        else {
            dialog.accept(); //Checks passed
        }
        });
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    //Dialog execution and user inputs processing
    if (dialog.exec() == QDialog::Accepted)
    {
        Worker newWorker; 

        //Set details from the dialog inputs
        //Generate a simple ID (consider a better method, like mentioned somewhere else in the code - using UIDs)
        newWorker.setWorkerID(static_cast<int>(workers.size()) + 1);
        newWorker.setWorkerName(nameInput->text().toStdString());
        newWorker.setPosition(positionInput->text().toStdString());
        newWorker.setWorkerExperience(experienceInput->value());
        newWorker.setWorkerSalary(salaryInput->value());
        newWorker.setWorkerAge(ageInput->value());
        //clockedIn false by default, assignedTask empty by default

        workers.push_back(newWorker); 

		updateManagerTables(); //Refresh the UI
    }
}

//Add New Task
void AutoMagik::addTask()
{
    if (cars.empty()) {
        QMessageBox::information(this, "No Cars Available", "Please add a car before adding a task.");
        return; //No car == no task
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Add New Task");
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    //Car selection
    QComboBox* carSelection = new QComboBox(&dialog);
    for (const auto& car : cars) {
        //Use make and model for display
        carSelection->addItem(QString::fromStdString(car.getMake() + " " + car.getModel()));
    }

    //Inputs
    QTextEdit* instructionsInput = new QTextEdit(&dialog);
    instructionsInput->setPlaceholderText("Enter task instructions (required)");
    QTextEdit* partsInput = new QTextEdit(&dialog);
    partsInput->setPlaceholderText("Enter parts needed (optional)");
    QTextEdit* initialCommentsInput = new QTextEdit(&dialog);
    initialCommentsInput->setPlaceholderText("Enter initial comments (optional)");

    //Layouts
    QVBoxLayout* carLayout = new QVBoxLayout();
    carLayout->addWidget(new QLabel("Select Car:", &dialog));
    carLayout->addWidget(carSelection);

    QHBoxLayout* labels1 = new QHBoxLayout();
    labels1->addWidget(new QLabel("Instructions:", &dialog));
    labels1->addWidget(new QLabel("Parts Needed:", &dialog));

    QHBoxLayout* inputs1 = new QHBoxLayout();
    inputs1->addWidget(instructionsInput);
    inputs1->addWidget(partsInput);

    QVBoxLayout* commentsLayout = new QVBoxLayout();
    commentsLayout->addWidget(new QLabel("Initial Comments:", &dialog));
    commentsLayout->addWidget(initialCommentsInput);

    mainLayout->addLayout(carLayout);
    mainLayout->addLayout(labels1);
    mainLayout->addLayout(inputs1);
    mainLayout->addLayout(commentsLayout);
    mainLayout->addStretch(); //Space the buttons nicely

    //Buttons
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);

    //Checks, same as for worker name and position
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        if (instructionsInput->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Input Error", "Task Instructions cannot be empty.");
        }
        else {
            dialog.accept();
        }
        });
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    //Dialog execution and user inputs processing, same as above
    if (dialog.exec() == QDialog::Accepted)
    {
        Task newTask;
        newTask.setTaskID(static_cast<int>(tasks.size()) + 1); //Simple temporary ID
        newTask.setTaskInstructions(instructionsInput->toPlainText().toStdString());
        newTask.setPartsNeeded(partsInput->toPlainText().toStdString());
        newTask.setComments(initialCommentsInput->toPlainText().toStdString());

        int selectedCarIndex = carSelection->currentIndex();
        if (selectedCarIndex >= 0 && selectedCarIndex < cars.size()) {
            newTask.setTaskCar(cars[selectedCarIndex]); //Assign selected car
        }
        else {
            QMessageBox::critical(this, "Error", "Invalid car selection.");
            return; //Shouldnt happen if Cars were added correctly
        }

        tasks.push_back(newTask);

        updateManagerTables(); //Refresh UI
    }
}

//Add New Car
void AutoMagik::addCar()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Car");
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    //Input Fields with placeholder texts
    QLineEdit* makeInput = new QLineEdit(&dialog);
    makeInput->setPlaceholderText("e.g., Opel"); 
    QLineEdit* modelInput = new QLineEdit(&dialog);
    modelInput->setPlaceholderText("e.g., Astra");
    QSpinBox* mileageInput = new QSpinBox(&dialog);
    mileageInput->setSuffix(" km");
    mileageInput->setRange(0, 3000000);
    mileageInput->setSingleStep(10000);
    QLineEdit* engineInput = new QLineEdit(&dialog);
    engineInput->setPlaceholderText("e.g., 2.5L I4");
    QComboBox* yearInput = new QComboBox(&dialog);
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear; i >= 1900; i--) { yearInput->addItem(QString::number(i)); }
    QLineEdit* phoneNumberInput = new QLineEdit(&dialog);
    phoneNumberInput->setPlaceholderText("Owner's contact number");

    //Layouts
    QHBoxLayout* row1Layout = new QHBoxLayout();
    QVBoxLayout* makeLayout = new QVBoxLayout();
    makeLayout->addWidget(new QLabel("Make:", &dialog)); makeLayout->addWidget(makeInput);
    QVBoxLayout* modelLayout = new QVBoxLayout();
    modelLayout->addWidget(new QLabel("Model:", &dialog)); modelLayout->addWidget(modelInput);
    row1Layout->addLayout(makeLayout); row1Layout->addLayout(modelLayout);

    QHBoxLayout* row2Layout = new QHBoxLayout();
    QVBoxLayout* engineLayout = new QVBoxLayout();
    engineLayout->addWidget(new QLabel("Engine:", &dialog)); engineLayout->addWidget(engineInput);
    QVBoxLayout* mileageLayout = new QVBoxLayout();
    mileageLayout->addWidget(new QLabel("Mileage:", &dialog)); mileageLayout->addWidget(mileageInput);
    row2Layout->addLayout(engineLayout); row2Layout->addLayout(mileageLayout);

    QHBoxLayout* row3Layout = new QHBoxLayout();
    QVBoxLayout* yearLayout = new QVBoxLayout();
    yearLayout->addWidget(new QLabel("Year:", &dialog)); yearLayout->addWidget(yearInput);
    QVBoxLayout* phoneLayout = new QVBoxLayout();
    phoneLayout->addWidget(new QLabel("Owner Phone:", &dialog)); phoneLayout->addWidget(phoneNumberInput);
    row3Layout->addLayout(yearLayout); row3Layout->addLayout(phoneLayout);

    mainLayout->addLayout(row1Layout);
    mainLayout->addLayout(row2Layout);
    mainLayout->addLayout(row3Layout);
    mainLayout->addStretch();

    //Buttons
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        //Checks
        if (makeInput->text().trimmed().isEmpty() || modelInput->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Input Error", "Car Make and Model cannot be empty.");
        }
        else {
            dialog.accept();
        }
        });
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        Car newCar;
        newCar.setCarMake(makeInput->text().toStdString());
        newCar.setCarModel(modelInput->text().toStdString());
        newCar.setEngineType(engineInput->text().toStdString());
        newCar.setCarMileage(mileageInput->value());
        newCar.setProductionYear(yearInput->currentText().toInt());
        newCar.setClientPhoneNumber(phoneNumberInput->text().toInt()); //Potential problem if not numeric, add handling later

        cars.push_back(newCar);

        updateManagerTables(); //Refresh UI
    }
}

//Placeholder functions for future implementation
void AutoMagik::editSelectedTask()
{
    QMessageBox::information(this, "Not Implemented", "Editing tasks is not yet implemented.");
    //TODO: Get selected row, populate dialog, update task in vector, refresh table
}

void AutoMagik::assignReassignTask()
{
    QMessageBox::information(this, "Not Implemented", "Assigning tasks is not yet implemented.");
    //TODO: Get selected task, show worker selection dialog, update task & worker, refresh tables
}

void AutoMagik::deleteTask()
{
    QMessageBox::information(this, "Not Implemented", "Deleting tasks is not yet implemented.");
    //TODO: Get selected row, confirm deletion, remove from tasks vector, refresh tables
}
