#include "AutoMagik.h"
#include "ui_AutoMagik.h"
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
#include <QUrl>
#include <QUrlQuery>
#include <QWebEngineView>
#include <QDebug>
#include <QListWidgetItem>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QNetworkAccessManager> 
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTableWidgetItem> 
#include <limits> 

#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
#include <QRegularExpressionValidator>
#else
#include <QRegExpValidator>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtCore5Compat/QRegExp> 
#endif

/*TODO
FIX THE BUG WHEN A WORKER HAS TWO TASKS ASSIGNED, AND ONE GETS DELETED,
IT SHOW THAT THE WORKER HAS NONE ASSIGNED TASKS !!!!!*/


AutoMagik::AutoMagik(QWidget* parent)
    : QMainWindow(parent), ui()
{

    ui.setupUi(this);

    firebase.setAPIKey("AIzaSyB-IpfsuvwFD8b_fNBbOCM9eRoTKfODR9w"); //setting the API key to our data base ( we dont know how to use env vars yet so for now itll look like this )

    //API key loading
    QByteArray apiKeyEnvVar = qgetenv("API_NINJAS_KEY");
    if (!apiKeyEnvVar.isEmpty()) {
        apiKey = QString::fromUtf8(apiKeyEnvVar);
        qDebug() << "API Ninjas Key loaded successfully from environment variable.";
    }
    else {
        apiKey = "YOUR_API_KEY_HERE";
        qWarning() << "WARNING: Environment variable 'API_NINJAS_KEY' not set or empty."
            << "Using placeholder key. API Ninjas functionality may fail.";
    }

    networkManager = new QNetworkAccessManager(this); //Initialize network manager

    ui.stackedWidget->setCurrentIndex(0); //Setting the current index to 0 (Home Menu)


    //Mode Selection (heavy lambda usage from here on XD)
    QObject::connect(ui.managerModeButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(3); }); //Manager Login Page
    QObject::connect(ui.workerModeButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(1); });   //Worker Login Page


    //Manager
    //Manager Login Page
    QObject::connect(ui.backButton1, &QPushButton::clicked, [this]()
        {
            ui.stackedWidget->setCurrentIndex(0);
            // Clear manager login fields
            ui.managerLoginEmail->clear();
            ui.managerPasswordLogin->clear();
            // Also clear worker fields just in case
            ui.workerLoginEmail->clear();
            ui.workerPasswordLogin->clear();
        });//Back to Mode Selection


    //when login fails:
    connect(&firebase, &Firebase::loginFailed, this, [this](const QString& error)
        {
            QMessageBox::warning(this, "Login Error", error);
        });

    //Updated login handlers for managers:
    QObject::connect(ui.managerLoginButton, &QPushButton::clicked, [this]()
        {
            QString email = ui.managerLoginEmail->toPlainText();
            QString password = ui.managerPasswordLogin->toPlainText();

            // turn off previous connections
            QObject::disconnect(&firebase, &Firebase::managerSignedIn, nullptr, nullptr);
            QObject::disconnect(&firebase, &Firebase::workerSignedIn, nullptr, nullptr);
            QObject::disconnect(&firebase, &Firebase::loginFailed, nullptr, nullptr);


            connect(&firebase, &Firebase::managerSignedIn, this, [this]() {
                ui.stackedWidget->setCurrentIndex(4); // Manager panel
                });
            connect(&firebase, &Firebase::loginFailed, this, [this](const QString& error) {
                QMessageBox::warning(this, "Login Error", error);

                });

            firebase.signUserIn(email, password);
        });

    QObject::connect(ui.createManagerAccButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(2); }); //To Manager Register Page
    //Manager Register Page



    //Back to Manager Login:
    QObject::connect(ui.backButton3, &QPushButton::clicked, [this]() {
        ui.stackedWidget->setCurrentIndex(3);
        ui.managerRegisterEmail->clear();   //clear an input
        ui.managerRegisterPassword->clear(); //clear an input               
        });


    // registering a new manager and putting them in the db:
    QObject::connect(ui.registerButton, &QPushButton::clicked, [this]() {
        QString email = ui.managerRegisterEmail->toPlainText();
        QString password = ui.managerRegisterPassword->toPlainText();
        // Email validation
        QRegularExpression emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
        if (!emailRegex.match(email).hasMatch()) {
            QMessageBox::warning(this, "Invalid Email", "Please enter a valid email address");
            return;
        }
        //Password must have at least 6 symbols
        if (password.length() < 6) {
            QMessageBox::warning(this, "Weak Password", "Password must be at least 6 characters long");
            return;
        }
        // Turning off all previous connections
        QObject::disconnect(&firebase, &Firebase::managerSignedIn, nullptr, nullptr);
        QObject::disconnect(&firebase, &Firebase::managerAccountCreated, nullptr, nullptr);
        QObject::disconnect(&firebase, &Firebase::registrationFailed, nullptr, nullptr);
        // Succesfull registration handler
        connect(&firebase, &Firebase::managerAccountCreated, this, [this, email]() {
            QMessageBox::information(this, "Success", "Account created successfully!");
            ui.managerRegisterEmail->clear();   //clear an input
            ui.managerRegisterPassword->clear(); //clear an input
            // After successfull registration
            });
        // Rwgistration error handler
        connect(&firebase, &Firebase::registrationFailed, this, [this](const QString& error) {
            QMessageBox::critical(this, "Registration Error", error);
            //staying on registration page
            ui.managerRegisterEmail->clear();   //clear an input
            ui.managerRegisterPassword->clear(); //clear an input
            });
        ui.managerRegisterEmail->setFocus();
        // If registration was succesfull we can login in
        connect(&firebase, &Firebase::managerSignedIn, this, [this]() {
            ui.stackedWidget->setCurrentIndex(4);

            });
        //add manager to firebase
        firebase.addManagerAccount(email, password);
        });



    //fetches data for our manager:
    connect(&firebase, &Firebase::managerSignedIn, this, [this]() 
        {
            // TODO: gets data from the firebase and puts it in our dashboard once we log in
        });


    //Manager Cars Page
    QObject::connect(ui.managerBackButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); }); //Sign Out -> Mode Selection
    QObject::connect(ui.tasksButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(4); });       //To Tasks Page
    QObject::connect(ui.workersButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(7); });    //To Workers Page
    QObject::connect(ui.addCarButton, &QPushButton::clicked, this, &AutoMagik::addCar);                             //Add Car Action
    QObject::connect(ui.editCarButton, &QPushButton::clicked, this, &AutoMagik::editSelectedCar);                   //Edit Car Action
    QObject::connect(ui.deleteCarButton, &QPushButton::clicked, this, &AutoMagik::deleteSelectedCar);                 //Delete Car Action
    //Manager Tasks Page
    QObject::connect(
        ui.managerBackButton, &QPushButton::clicked, [this]() {
            ui.stackedWidget->setCurrentIndex(0);
            // Clear manager login fields
            ui.managerLoginEmail->clear();
            ui.managerPasswordLogin->clear();
            // Also clear worker fields just in case
            ui.workerLoginEmail->clear();
            ui.workerPasswordLogin->clear();
        }
    );//Returning to the main page - manager
    QObject::connect(ui.carsButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(6); });
    QObject::connect(ui.workersButton, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(7); });
    QObject::connect(ui.addTaskButton, &QPushButton::clicked, this, &AutoMagik::addTask);
    QObject::connect(ui.editTaskButton, &QPushButton::clicked, this, &AutoMagik::editSelectedTask);                   //Edit Task Action
    QObject::connect(ui.deleteTaskButton, &QPushButton::clicked, this, &AutoMagik::deleteTask);                       //Delete Task Action
    QObject::connect(ui.assignTaskButton, &QPushButton::clicked, this, &AutoMagik::assignReassignTask);               //Assign Task Action
    //Manager Workers Page
    QObject::connect(ui.managerBackButton3, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(0); });
    QObject::connect(ui.carsButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(6); });
    QObject::connect(ui.tasksButton2, &QPushButton::clicked, [this]() { ui.stackedWidget->setCurrentIndex(4); });
    QObject::connect(ui.addWorkerButton, &QPushButton::clicked, this, &AutoMagik::addWorker);
    QObject::connect(ui.editWorkerButton, &QPushButton::clicked, this, &AutoMagik::editSelectedWorker);               //Edit Worker Action
    QObject::connect(ui.deleteWorkerButton, &QPushButton::clicked, this, &AutoMagik::deleteSelectedWorker);           //Delete Worker Action



    //Worker:
    //Worker Login Page
    QObject::connect(ui.backButton2, &QPushButton::clicked, [this]() {
        ui.stackedWidget->setCurrentIndex(0);
        // Clear manager login fields
        ui.managerLoginEmail->clear();
        ui.managerPasswordLogin->clear();
        // Also clear worker fields just in case
        ui.workerLoginEmail->clear();
        ui.workerPasswordLogin->clear();
        });//Back to Mode Selection 


    // logging in as worker:
    QObject::connect(ui.workerLoginButton, &QPushButton::clicked, [this]()
        {
            QString email = ui.workerLoginEmail->toPlainText();
            QString password = ui.workerPasswordLogin->toPlainText();

            // turn off previous connection
            QObject::disconnect(&firebase, &Firebase::managerSignedIn, nullptr, nullptr);
            QObject::disconnect(&firebase, &Firebase::workerSignedIn, nullptr, nullptr);

            firebase.signUserIn(email, password);
            QObject::connect(&firebase, &Firebase::workerSignedIn, [this]()
                {
                    ui.stackedWidget->setCurrentIndex(5);
                }
            );
        });


    //Worker Dashboard Page
    QObject::connect(
        ui.workerBackButton, &QPushButton::clicked, [this]() {
            ui.stackedWidget->setCurrentIndex(0);
            // Clear worker login fields
            ui.workerLoginEmail->clear();
            ui.workerPasswordLogin->clear();
            // Also clear manager fields just in case
            ui.managerLoginEmail->clear();
            ui.managerPasswordLogin->clear();
        }
    );  //Sign Out -> Mode Selection


    //Worker dashboard connections
    //When the selected item in the task list changes, update the details view
    QObject::connect(ui.workerTaskListWidget, &QListWidget::currentItemChanged,
        this, &AutoMagik::updateWorkerDashboardSelection);
    //Connect "Car Info" button
    QObject::connect(ui.carInfoButton, &QPushButton::clicked, this, &AutoMagik::showCarInfo);


    //Initial button states and table refresh
    updateManagerTables();
    updateWorkerDashboard(); //Initial population of worker dashboard (can be empty)
    //Hide selection-based buttons initially
    ui.markCompleteButton->setEnabled(false);
    ui.addCommentButton->setEnabled(false);
    ui.carInfoButton->setEnabled(false); //Initially, the car info button should be disabled

    //NOTE: Edit and Delete buttons are handled by updateManagerTables (for enabling/disabling)
}


AutoMagik::~AutoMagik()
{
    //Destructor automatically handles deletion of child QObjects like networkManager
    //No explicit delete ui; needed as ui object is part of the class
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
        ui.workersTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(worker.getWorkerExperience()) + QLatin1String(" yrs")));
        ui.workersTableWidget->setItem(i, 4, new QTableWidgetItem(QStringLiteral("$") + QString::number(worker.getWorkerSalary())));
        ui.workersTableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(worker.getWorkerAge())));
        //Column 6 needs real logic - currently placeholder in Worker class
        ui.workersTableWidget->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(worker.getAssignedTaskInfo())));
    }
    ui.workersTableWidget->resizeColumnsToContents();

    //Update Tasks Table
    ui.tasksTableWidget->setRowCount(0);
    ui.tasksTableWidget->setRowCount(static_cast<int>(tasks.size()));
    for (int i = 0; i < static_cast<int>(tasks.size()); ++i)
    {
        const Task& task = tasks[i];
        ui.tasksTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(task.getTaskID())));
        //Combine Make and Model for Car column
        QString carDisplay = QString::fromStdString(task.getCarObject().getMake() + " " + task.getCarObject().getModel());
        ui.tasksTableWidget->setItem(i, 1, new QTableWidgetItem(carDisplay));
        ui.tasksTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(task.getTaskInstructions())));
        ui.tasksTableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(task.getTaskStatus()))); //setting status

        //Setting task worker
        if (task.getTaskWorkerID() != 0) //If worker ID is not 0, set the worker name
        {
            for (const auto& worker : workers)
            {
                if (worker.getWorkerID() == task.getTaskWorkerID())
                {
                    ui.tasksTableWidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(worker.getWorkerName())));
                    break;
                }
            }
        }
        else {
            ui.tasksTableWidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString("Unsigned"))); //Setting to unsigned if no worker assigned
        }
        ui.tasksTableWidget->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(task.getTaskPriority()))); //Priority
    }
    ui.tasksTableWidget->resizeColumnsToContents();


    //Update Cars Table
    ui.carsTableWidget->setRowCount(0);
    ui.carsTableWidget->setRowCount(static_cast<int>(cars.size()));
    for (int i = 0; i < static_cast<int>(cars.size()); ++i)
    {
        const Car& car = cars[i];
        ui.carsTableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(car.getMake())));
        ui.carsTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(car.getModel())));
        ui.carsTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(car.getEngineType())));
        ui.carsTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(car.getProductionYear())));
        ui.carsTableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(car.getCarMileage()) + QLatin1String(" km")));
        ui.carsTableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(car.getClientPhoneNumber())));
    }
    ui.carsTableWidget->resizeColumnsToContents();


    //Update button states based on existence of items
    bool hasWorkers = !workers.empty();
    bool hasTasks = !tasks.empty();
    bool hasCars = !cars.empty();

    //Enable edit/delete only if items exist (selection logic needs to be added separately)
    ui.editWorkerButton->setEnabled(hasWorkers);
    ui.deleteWorkerButton->setEnabled(hasWorkers);

    ui.editTaskButton->setEnabled(hasTasks);
    ui.deleteTaskButton->setEnabled(hasTasks);

    ui.editCarButton->setEnabled(hasCars);
    ui.deleteCarButton->setEnabled(hasCars);

    //Other buttons
    ui.assignTaskButton->setEnabled(hasWorkers && hasTasks); //Need both workers and tasks to assign
    ui.addTaskButton->setEnabled(hasCars); //Can only add task if cars exist
}

//Update Worker Task List
void AutoMagik::updateWorkerDashboard() {
    ui.workerTaskListWidget->clear();

    //Placeholder Logic: List all tasks
    //TODO: Filter tasks based on the actual logged-in worker
    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto& task = tasks[i];
        QString carDesc = QString::fromStdString(task.getCarObject().getMake() + " " + task.getCarObject().getModel());
        QString instructionPreview = QString::fromStdString(task.getTaskInstructions());
        if (instructionPreview.length() > 30) {
            instructionPreview = instructionPreview.left(30) + QLatin1String("...");
        }

        QString itemText = QString("ID: %1 - %2 (%3)")
            .arg(task.getTaskID())          //Task ID
            .arg(carDesc)                   //Car Make + Model
            .arg(instructionPreview);       //Short Instructions
        QListWidgetItem* item = new QListWidgetItem(itemText);
        //Store the original index from the tasks vector in the item's data
        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<int>(i)));
        ui.workerTaskListWidget->addItem(item);
    }

    updateWorkerDashboardSelection(); //Update details based on current selection (or lack thereof)
}

//Add New Worker
void AutoMagik::addWorker()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QLatin1String("Add New Worker"));
    dialog.setMinimumWidth(400);

    QFormLayout* formLayout = new QFormLayout(&dialog);

    QLineEdit* nameInput = new QLineEdit(&dialog);
    QLineEdit* positionInput = new QLineEdit(&dialog);
    QSpinBox* experienceInput = new QSpinBox(&dialog);
    experienceInput->setSuffix(QLatin1String(" years"));
    experienceInput->setRange(0, 60);
    QSpinBox* salaryInput = new QSpinBox(&dialog);
    salaryInput->setPrefix(QLatin1String("$ "));
    salaryInput->setRange(0, 500000);
    salaryInput->setSingleStep(1000);
    QSpinBox* ageInput = new QSpinBox(&dialog);
    ageInput->setRange(18, 100);
    QLineEdit* emailInput = new QLineEdit(&dialog);
    QLineEdit* passwordInput = new QLineEdit(&dialog);

    formLayout->addRow(new QLabel(QLatin1String("Worker Name:"), &dialog), nameInput);
    formLayout->addRow(new QLabel(QLatin1String("Position:"), &dialog), positionInput);
    formLayout->addRow(new QLabel(QLatin1String("Experience:"), &dialog), experienceInput);
    formLayout->addRow(new QLabel(QLatin1String("Salary:"), &dialog), salaryInput);
    formLayout->addRow(new QLabel(QLatin1String("Age:"), &dialog), ageInput);
    formLayout->addRow(new QLabel(QLatin1String("Email:"), &dialog), emailInput);
    formLayout->addRow(new QLabel(QLatin1String("Password:"), &dialog), passwordInput);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout->addRow(&buttonBox);

    

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() 
        {
            //regex for email auth
        QRegularExpression emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
        QRegularExpressionMatch match = emailRegex.match(emailInput->text().trimmed());

        if (nameInput->text().trimmed().isEmpty() || positionInput->text().trimmed().isEmpty() || passwordInput->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Worker Name, Position and Password cannot be empty."));
            //Keep dialog open
        }
        else if (!match.hasMatch()) {
            QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Email form is incorrect."));
        }
        else {
            dialog.accept(); //Close dialog
        }
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) 
    {
        Worker newWorker;
        //Simple sequential ID - implement UUIDs or database IDs later
        newWorker.setWorkerID(static_cast<int>(workers.size()) + 1001);
        newWorker.setWorkerName(nameInput->text().trimmed().toStdString());
        newWorker.setPosition(positionInput->text().trimmed().toStdString());
        newWorker.setWorkerExperience(experienceInput->value());
        newWorker.setWorkerSalary(salaryInput->value());
        newWorker.setWorkerAge(ageInput->value());
        //clockedIn false by default, assignedTask empty by default

        workers.push_back(newWorker);
        updateManagerTables(); //Refresh the UI

        //Adding worker to database
            QVariantMap data;
            data["w_id"] = QVariant(newWorker.getWorkerID()); 
            data["name"] = QVariant(QString::fromStdString(newWorker.getWorkerName()));
            data["position"] = QVariant(QString::fromStdString(newWorker.getPosition()));
            data["experience"] = QVariant(newWorker.getWorkerExperience());
            data["salary"] = QVariant(newWorker.getWorkerSalary());
            data["age"] = QVariant(newWorker.getWorkerAge());
            data["manager"] = QVariant(firebase.m_uid);  
            //delete [] newWorker - THIS WILL ONLY BE NEEDED ONCE WE START DOWNLOAD FROM THE DB
            firebase.addWorkerAccount(emailInput->text().trimmed(), data ,passwordInput->text().trimmed());    
    }
}

//Add New Task
void AutoMagik::addTask()
{
    if (cars.empty())
    {
        QMessageBox::information(this, QLatin1String("No Cars Available"), QLatin1String("Please add a car before adding a task."));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QLatin1String("Add New Task"));
    dialog.setMinimumWidth(450);
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    QComboBox* carSelection = new QComboBox(&dialog);
    for (size_t i = 0; i < cars.size(); ++i)
    {
        const auto& car = cars[i];
        carSelection->addItem(QString::fromStdString(car.getMake() + " " + car.getModel() + " (" + std::to_string(car.getProductionYear()) + ")"),
            QVariant::fromValue(static_cast<int>(i))); //Store index in data role
    }

    QComboBox* workerComboBox = new QComboBox(&dialog);
    if (!this->workers.empty())
    {
        for (const auto& worker : workers)
        {
            workerComboBox->addItem(QString::fromStdString(worker.getWorkerName() + ", " + worker.getPosition()));
        }
    }
    else {
        workerComboBox->addItem(QLatin1String("No Workers Available"));
        workerComboBox->setDisabled(true); //Disabling the combobox if there are no workers
    }

    //Priority combo box
    QComboBox* priorityComboBox = new QComboBox(&dialog);
    priorityComboBox->addItem(QLatin1String("Low"), QVariant::fromValue(LOW));
    priorityComboBox->addItem(QLatin1String("Medium"), QVariant::fromValue(MEDIUM));
    priorityComboBox->addItem(QLatin1String("High"), QVariant::fromValue(HIGH));

    QTextEdit* instructionsInput = new QTextEdit(&dialog);
    instructionsInput->setPlaceholderText(QLatin1String("Enter task instructions (required)"));
    instructionsInput->setMinimumHeight(80);
    QTextEdit* partsInput = new QTextEdit(&dialog);
    partsInput->setPlaceholderText(QLatin1String("Enter parts needed (optional)"));
    partsInput->setMinimumHeight(60);
    QTextEdit* initialCommentsInput = new QTextEdit(&dialog);
    initialCommentsInput->setPlaceholderText(QLatin1String("Enter initial comments (optional)"));
    initialCommentsInput->setMinimumHeight(60);

    QFormLayout* form = new QFormLayout(); //Form layout
    form->addRow(new QLabel(QLatin1String("Select Priority:"), &dialog)); //Priority selection
    form->addWidget(priorityComboBox);

    form->addRow(new QLabel(QLatin1String("Select Car:"), &dialog), carSelection); //Car selection
    mainLayout->addLayout(form);

    form->addRow(new QLabel(QLatin1String("Select Worker:"), &dialog), workerComboBox); //Worker selection
    mainLayout->addLayout(form);

    mainLayout->addWidget(new QLabel(QLatin1String("Instructions:"), &dialog));
    mainLayout->addWidget(instructionsInput);
    mainLayout->addWidget(new QLabel(QLatin1String("Parts Needed:"), &dialog));
    mainLayout->addWidget(partsInput);
    mainLayout->addWidget(new QLabel(QLatin1String("Initial Comments:"), &dialog));
    mainLayout->addWidget(initialCommentsInput);

    mainLayout->addStretch();

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        if (instructionsInput->toPlainText().trimmed().isEmpty())
        {
            QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Task Instructions cannot be empty."));
        }
        else {
            dialog.accept();
        }
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        Task newTask;
        newTask.setTaskID(static_cast<int>(tasks.size()) + 5001); //Simple temporary ID
        newTask.setTaskInstructions(instructionsInput->toPlainText().trimmed().toStdString());
        newTask.setPartsNeeded(partsInput->toPlainText().trimmed().toStdString());
        newTask.setComments(initialCommentsInput->toPlainText().trimmed().toStdString());

        priority selectedPriority = static_cast<priority>(priorityComboBox->currentData().toInt());
        newTask.setTaskPriority(selectedPriority); //Set selected priority;

        int selectedCarDataIndex = carSelection->currentData().toInt(); //Get stored index
        if (selectedCarDataIndex >= 0 && selectedCarDataIndex < static_cast<int>(cars.size()))
        {
            newTask.setTaskCar(cars[selectedCarDataIndex]); //Assign selected car by reference/copy
        }
        else {
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Invalid car selection index."));
            return; //Should not happen if populated correctly
        }
        int selectedWorkerIndex = workerComboBox->currentIndex();
        if (selectedWorkerIndex >= 0 && selectedWorkerIndex < static_cast<int>(workers.size()))
        {
            this->workers[selectedWorkerIndex].assignTask(newTask); //Assign task to selected worker
            newTask.setTaskWorkerID(workers[selectedWorkerIndex].getWorkerID()); //Set worker ID in task
        } /*else {
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Invalid worker selection index."));
            return; //Should not happen if populated correctly
        }*/

        newTask.setTaskStatus(NEW); //Set default status to NEW

        tasks.push_back(newTask);

        updateManagerTables(); //Refresh UI
        updateWorkerDashboard(); //Also refresh worker view
    }
}

//Add New Car
void AutoMagik::addCar()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QLatin1String("Add New Car"));
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    QLineEdit* makeInput = new QLineEdit(&dialog);
    makeInput->setPlaceholderText(QLatin1String("e.g., Opel"));
    QLineEdit* modelInput = new QLineEdit(&dialog);
    modelInput->setPlaceholderText(QLatin1String("e.g., Astra"));
    QSpinBox* mileageInput = new QSpinBox(&dialog);
    mileageInput->setSuffix(QLatin1String(" km"));
    mileageInput->setRange(0, 3000000);
    mileageInput->setSingleStep(10000);
    mileageInput->setMaximum(1000000); //Set a reasonable maximum mileage for those old folks in opel kadett e cabrio 
    QLineEdit* engineInput = new QLineEdit(&dialog);
    engineInput->setPlaceholderText(QLatin1String("e.g., 2.5L I4"));
    QComboBox* yearInput = new QComboBox(&dialog);
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear; i >= 1900; i--) { yearInput->addItem(QString::number(i)); }
    QLineEdit* phoneNumberInput = new QLineEdit(&dialog);
    phoneNumberInput->setPlaceholderText(QLatin1String("Owner's contact number (digits only)"));

    //Strict phone number validation: Allows optional leading '+' and digits only
    //Allows spaces during input but they will be removed before storing

#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    QRegularExpression phoneRegex(QStringLiteral("^\\+?[\\d\\s]+$")); //Allows digits, spaces, optional leading +
    phoneNumberInput->setValidator(new QRegularExpressionValidator(phoneRegex, phoneNumberInput));
#else
    QRegExp phoneRegExp("^\\+?[\\d\\s]+$");
    phoneNumberInput->setValidator(new QRegExpValidator(phoneRegExp, phoneNumberInput));
#endif

    //Used QFormLayout for better alignment
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(QLatin1String("Make:"), makeInput);
    formLayout->addRow(QLatin1String("Model:"), modelInput);
    formLayout->addRow(QLatin1String("Year:"), yearInput);
    formLayout->addRow(QLatin1String("Engine:"), engineInput);
    formLayout->addRow(QLatin1String("Mileage:"), mileageInput);
    formLayout->addRow(QLatin1String("Owner Phone:"), phoneNumberInput);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]()
        {
            QString phoneText = phoneNumberInput->text().trimmed();
            QString makeText = makeInput->text().trimmed();
            QString modelText = modelInput->text().trimmed();

            if (makeText.isEmpty() || modelText.isEmpty())
            {
                QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Car Make and Model cannot be empty."));
                return; //Keep dialog open
            }
            if (phoneText.isEmpty())
            {
                QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Owner Phone number cannot be empty."));
                return; //Keep dialog open
            }

            //Further phone validation (check if after removing spaces/plus it's just digits)
            QString phoneCleaned = phoneText;
            phoneCleaned.remove(' ').remove('+');
            bool isNumeric;
            phoneCleaned.toLongLong(&isNumeric); //Use long long for numbers as long as my pp

            if (!isNumeric || phoneCleaned.isEmpty())
            {
                QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Invalid characters in Owner Phone number. Please use digits, spaces, and optionally a leading '+'."));
                return; //Keep dialog open
            }

            //All checks passed
            dialog.accept();
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        Car newCar;

        newCar.setCarMake(makeInput->text().trimmed().toStdString());
        newCar.setCarModel(modelInput->text().trimmed().toStdString());
        newCar.setEngineType(engineInput->text().trimmed().toStdString());
        newCar.setCarMileage(mileageInput->value());
        newCar.setProductionYear(yearInput->currentText().toInt());

        QString phoneCleaned = phoneNumberInput->text().trimmed();
        phoneCleaned.remove(' ').remove('+'); //Remove spaces and '+' for storage

        //Store as string or try conversion if needed, handle potential overflow
        bool conversionOk;
        qlonglong phoneLong = phoneCleaned.toLongLong(&conversionOk);
        if (conversionOk) {
            //Decide how to store phone: as string or number? Sticking to int for now
            if (phoneLong > std::numeric_limits<int>::max() || phoneLong < std::numeric_limits<int>::min())
            {
                QMessageBox::warning(this, QLatin1String("Warning"), QLatin1String("Phone number too long, storing truncated value or 0."));
                newCar.setClientPhoneNumber(0);
            }
            else {
                newCar.setClientPhoneNumber(static_cast<int>(phoneLong));
            }
        }
        else {
            QMessageBox::warning(this, QLatin1String("Warning"), QLatin1String("Phone number format error during conversion, storing 0."));
            newCar.setClientPhoneNumber(0); //Store 0 or default on error
        }

        cars.push_back(newCar);
        updateManagerTables(); //Refresh UI
        //Also potentially update car selection dropdowns if they exist elsewhere

        if (!firebase.getIdToken().isEmpty()) {
            firebase.addCarToDatabase(
                QString::fromStdString(newCar.getMake()),
                QString::fromStdString(newCar.getModel()),
                QString::fromStdString(newCar.getEngineType()),
                newCar.getProductionYear(),
                newCar.getCarMileage(),
                newCar.getClientPhoneNumber(),
                firebase.getIdToken()
            );
        }
        else
        {
            QMessageBox::warning(this, "Error", "Not authenticated");
        }
    }
}


//Worker dashboard task selection handling
void AutoMagik::updateWorkerDashboardSelection()
{
    QListWidgetItem* selectedItem = ui.workerTaskListWidget->currentItem();

    //Clear details if no item is selected
    auto clearDetails = [this]()
        {
            ui.carInfoGroupBox->setTitle(QLatin1String("Car Info"));
            qDeleteAll(ui.carInfoGroupBox->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
            if (ui.carInfoGroupBox->layout()) delete ui.carInfoGroupBox->layout();

            ui.taskInfoGroupBox->setTitle(QLatin1String("Task Info"));
            qDeleteAll(ui.taskInfoGroupBox->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
            if (ui.taskInfoGroupBox->layout()) delete ui.taskInfoGroupBox->layout();

            ui.commentsTextEdit->clear();
            ui.newCommentLineEdit->clear();

            ui.markCompleteButton->setEnabled(false);
            ui.addCommentButton->setEnabled(false);
            ui.carInfoButton->setEnabled(false);
            currentWorkerTaskIndex = -1;
        };

    if (!selectedItem) {
        clearDetails();
        return;
    }

    //Item selected, get task index
    bool ok;
    int taskIndex = selectedItem->data(Qt::UserRole).toInt(&ok);

    if (!ok || taskIndex < 0 || taskIndex >= static_cast<int>(tasks.size()))
    {
        qWarning() << "Invalid task index stored in list item data:" << selectedItem->data(Qt::UserRole);
        clearDetails();
        return;
    }

    //Valid task selected
    currentWorkerTaskIndex = taskIndex; //Store the index
    const Task& selectedTask = tasks[currentWorkerTaskIndex];
    const Car& associatedCar = selectedTask.getCarObject();

    //Populate Car Info
    ui.carInfoGroupBox->setTitle(QString("Car Info: %1 %2")
        .arg(QString::fromStdString(associatedCar.getMake()))
        .arg(QString::fromStdString(associatedCar.getModel())));
    //Clear previous widgets and layout
    qDeleteAll(ui.carInfoGroupBox->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
    if (ui.carInfoGroupBox->layout()) delete ui.carInfoGroupBox->layout();
    QFormLayout* carLayout = new QFormLayout();

    //Create labels for car info values
    QLabel* makeLabel = new QLabel(QString::fromStdString(associatedCar.getMake()), ui.carInfoGroupBox);
    QLabel* modelLabel = new QLabel(QString::fromStdString(associatedCar.getModel()), ui.carInfoGroupBox);
    QLabel* yearLabel = new QLabel(QString::number(associatedCar.getProductionYear()), ui.carInfoGroupBox);
    QLabel* engineLabel = new QLabel(QString::fromStdString(associatedCar.getEngineType()), ui.carInfoGroupBox);
    QLabel* mileageLabel = new QLabel(QString::number(associatedCar.getCarMileage()) + QLatin1String(" km"), ui.carInfoGroupBox);
    QLabel* ownerLabel = new QLabel(QString::number(associatedCar.getClientPhoneNumber()), ui.carInfoGroupBox);

    engineLabel->setWordWrap(true);


    //Add rows to form layout
    carLayout->addRow(QLatin1String("Make:"), makeLabel);
    carLayout->addRow(QLatin1String("Model:"), modelLabel);
    carLayout->addRow(QLatin1String("Year:"), yearLabel);
    carLayout->addRow(QLatin1String("Engine:"), engineLabel);
    carLayout->addRow(QLatin1String("Mileage:"), mileageLabel);
    carLayout->addRow(QLatin1String("Owner Contact:"), ownerLabel);

    carLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    ui.carInfoGroupBox->setLayout(carLayout); //Set the new layout


    //Populate Task Info
    ui.taskInfoGroupBox->setTitle(QString("Task Info (ID: %1)").arg(selectedTask.getTaskID()));
    //Clear previous widgets and layout
    qDeleteAll(ui.taskInfoGroupBox->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
    if (ui.taskInfoGroupBox->layout()) delete ui.taskInfoGroupBox->layout();
    QFormLayout* taskLayout = new QFormLayout();

    QLabel* instructionsDisplay = new QLabel(QString::fromStdString(selectedTask.getTaskInstructions()), ui.taskInfoGroupBox);
    instructionsDisplay->setWordWrap(true); //Enable word wrap
    instructionsDisplay->setAlignment(Qt::AlignTop); //Align text to top

    QLabel* partsDisplay = new QLabel(ui.taskInfoGroupBox);
    partsDisplay->setWordWrap(true); //Enable word wrap
    partsDisplay->setAlignment(Qt::AlignTop); //Align text to top
    QString partsText = QString::fromStdString(selectedTask.getPartsNeeded());
    if (partsText.isEmpty()) {
        partsDisplay->setText(QLatin1String("<i>None specified</i>"));
    }
    else {
        partsDisplay->setText(partsText);
    }

    taskLayout->addRow(QLatin1String("Instructions:"), instructionsDisplay);
    taskLayout->addRow(QLatin1String("Parts Needed:"), partsDisplay);


    //Add status, priority etc. here when implemented in Task class

    taskLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow); //Allows multiline labels to take space needed

    ui.taskInfoGroupBox->setLayout(taskLayout); //Set the new layout


    //Populate Comments
    ui.commentsTextEdit->setPlainText(QString::fromStdString(selectedTask.getComments()));
    ui.newCommentLineEdit->clear();

    //Enable Buttons
    ui.markCompleteButton->setEnabled(true); //TODO: Add logic based on task status
    ui.addCommentButton->setEnabled(true);
    ui.carInfoButton->setEnabled(true);
}


//-----------------------API Call Trigger-----------------------------
void AutoMagik::showCarInfo()
{
    if (currentWorkerTaskIndex < 0 || currentWorkerTaskIndex >= static_cast<int>(tasks.size()))
    {
        QMessageBox::warning(this, QLatin1String("No Task Selected"), QLatin1String("Please select a task from the list first."));
        return;
    }

    //Check if requests are already running for this button
    if (nhtsaRequestPending || apiNinjaRequestPending)
    {
        qDebug() << "Car info request already in progress.";
        return; //Prevent multiple simultaneous requests from the same button click
    }

    const Task& selectedTask = tasks[currentWorkerTaskIndex];
    const Car& associatedCar = selectedTask.getCarObject();

    //Store details for use when replies come back
    lastMakeForDialog = QString::fromStdString(associatedCar.getMake());
    lastModelForDialog = QString::fromStdString(associatedCar.getModel());
    lastYearForDialog = associatedCar.getProductionYear();

    //Clean model for API calls (basic split)
    QString modelForApi = lastModelForDialog.split(' ').first();

    //Reset results and flags before starting new requests
    lastNhtsaResult = QJsonObject();
    lastApiNinjaResult = QJsonObject();
    nhtsaRequestPending = false; //Will be set true below if request starts
    apiNinjaRequestPending = false; //Will be set true below if request starts

    ui.carInfoButton->setEnabled(false);
    ui.carInfoButton->setText(QLatin1String("Loading Info..."));

    //Start NHTSA Request
    QUrl nhtsaUrl("https://vpic.nhtsa.dot.gov/api/vehicles/GetModelsForMakeYear/make/" + lastMakeForDialog + "/modelyear/" + QString::number(lastYearForDialog));
    QUrlQuery nhtsaQuery;
    nhtsaQuery.addQueryItem("format", "json");
    nhtsaUrl.setQuery(nhtsaQuery);
    QNetworkRequest nhtsaRequest(nhtsaUrl);
    qDebug() << "Requesting NHTSA vPIC API:" << nhtsaRequest.url().toString();
    QNetworkReply* nhtsaReply = networkManager->get(nhtsaRequest);
    nhtsaRequestPending = true; //Mark as pending
    connect(nhtsaReply, &QNetworkReply::finished, this, &AutoMagik::handleNhtsaReplyFinished);


    //Start API Ninjas request (if key is valid)
    if (!apiKey.isEmpty() && apiKey != QLatin1String("YOUR_API_KEY_HERE")) {
        QUrl ninjaUrl("https://api.api-ninjas.com/v1/cars");
        QUrlQuery ninjaQuery;
        ninjaQuery.addQueryItem("model", modelForApi); //Use cleaned model
        ninjaQuery.addQueryItem("year", QString::number(lastYearForDialog));
        ninjaUrl.setQuery(ninjaQuery);

        QNetworkRequest ninjaRequest(ninjaUrl);
        ninjaRequest.setRawHeader("X-Api-Key", apiKey.toUtf8());

        qDebug() << "Requesting API Ninjas API:" << ninjaRequest.url().toString();
        QNetworkReply* ninjaReply = networkManager->get(ninjaRequest);
        apiNinjaRequestPending = true; //Mark as pending
        connect(ninjaReply, &QNetworkReply::finished, this, &AutoMagik::handleApiNinjaReplyFinished);
    }
    else {
        qDebug() << "API Ninjas key missing or placeholder. Skipping API Ninjas request.";
        //No request started, so it's not pending. apiNinjaRequestPending remains false
        //Check immediately in case NHTSA also finishes instantly or fails before sending
        //This ensures the dialog appears even if only one API was attempted and it finished
        checkAndShowCombinedInfoDialog();
    }
}

//NHTSA reply handler
void AutoMagik::handleNhtsaReplyFinished()
{
    QNetworkReply* nhtsaReply = qobject_cast<QNetworkReply*>(sender());
    if (!nhtsaReply)
    {
        qWarning() << "Received finished signal but sender is not a QNetworkReply!";
        nhtsaRequestPending = false; //Mark as finished anyway to avoid blocking
        checkAndShowCombinedInfoDialog();
        return;
    }

    qDebug() << "NHTSA reply finished. Status:" << nhtsaReply->error();
    lastNhtsaResult = QJsonObject(); //Clear previous/default

    if (nhtsaReply->error() != QNetworkReply::NoError)
    {
        qWarning() << "NHTSA API Request Error:" << nhtsaReply->errorString();
        lastNhtsaResult["error"] = nhtsaReply->errorString(); //Store error string
    }
    else {
        QByteArray nhtsaResponseData = nhtsaReply->readAll();
        QJsonDocument nhtsaJsonDoc = QJsonDocument::fromJson(nhtsaResponseData);
        if (nhtsaJsonDoc.isObject())
        {
            QJsonObject nhtsaRootObject = nhtsaJsonDoc.object();
            //Check for "Count" and "Results" array
            if (nhtsaRootObject.contains("Count") && nhtsaRootObject.value("Count").toInt() > 0 &&
                nhtsaRootObject.contains("Results") && nhtsaRootObject["Results"].isArray())
            {
                QJsonArray nhtsaResultsArray = nhtsaRootObject["Results"].toArray();
                QString targetModelClean = lastModelForDialog.split(' ').first().toLower();
                bool modelFound = false;
                for (const QJsonValue& value : nhtsaResultsArray)
                {
                    if (!value.isObject()) continue;
                    QJsonObject modelObject = value.toObject();
                    QString apiModelName = modelObject.value("Model_Name").toString("").toLower();
                    if (apiModelName == targetModelClean) {
                        lastNhtsaResult = modelObject; //Store the specific found model object
                        modelFound = true;
                        qDebug() << "NHTSA model found:" << modelObject.value("Model_Name").toString();
                        break;
                    }
                }
                if (!modelFound)
                {
                    qDebug() << "NHTSA Results received for Make/Year, but specific model '" << targetModelClean << "' not found in the list.";
                    //Store message indicating make/year was valid but model mismatch
                    lastNhtsaResult["message"] = QString("Make/Year found, but specific model '%1' not listed.").arg(targetModelClean);
                }
            }
            else {
                QString message = nhtsaRootObject.value("Message").toString("Unknown NHTSA response format");
                qWarning() << "NHTSA API Response missing 'Results' array or Count is zero. Message:" << message;
                lastNhtsaResult["error"] = message; //Store error/message from API
            }
        }
        else {
            qWarning() << "NHTSA API Response is not a JSON object:" << nhtsaResponseData.left(200); //Log start of data
            lastNhtsaResult["error"] = QLatin1String("Invalid JSON format received from NHTSA");
        }
    }

    nhtsaRequestPending = false; //Mark as finished
    nhtsaReply->deleteLater();   //Clean up reply object
    checkAndShowCombinedInfoDialog(); //Check if the other request (if any) is also done
}

//API Ninjas reply handler
void AutoMagik::handleApiNinjaReplyFinished()
{
    QNetworkReply* ninjaReply = qobject_cast<QNetworkReply*>(sender());
    if (!ninjaReply)
    {
        qWarning() << "Received finished signal but sender is not a QNetworkReply!";
        apiNinjaRequestPending = false; //Mark as finished anyway to avoid blocking
        checkAndShowCombinedInfoDialog();
        return;
    }

    qDebug() << "API Ninjas reply finished. Status:" << ninjaReply->error();
    lastApiNinjaResult = QJsonObject(); //Clear previous/default

    if (ninjaReply->error() != QNetworkReply::NoError)
    {
        qWarning() << "API Ninjas Request Error:" << ninjaReply->errorString();
        //Attempt to read potential error message from body
        QByteArray errorBody = ninjaReply->readAll();
        QJsonDocument errorDoc = QJsonDocument::fromJson(errorBody);
        QString errorMsg = ninjaReply->errorString();
        if (errorDoc.isObject() && errorDoc.object().contains("error"))
        {
            errorMsg = errorDoc.object()["error"].toString(errorMsg); //Use JSON error if available
        }
        else if (errorDoc.isObject() && errorDoc.object().contains("message"))
        {
            errorMsg = errorDoc.object()["message"].toString(errorMsg); //Use message if available
        }
        qWarning() << "API Ninjas Error Body (if any):" << errorBody;
        lastApiNinjaResult["error"] = errorMsg; //Store error

    }
    else {
        QByteArray ninjaResponseData = ninjaReply->readAll();
        QJsonDocument ninjaJsonDoc = QJsonDocument::fromJson(ninjaResponseData);
        if (ninjaJsonDoc.isArray())
        {
            QJsonArray ninjaArray = ninjaJsonDoc.array();
            if (!ninjaArray.isEmpty())
            {
                //Assume first result is the best/only match for now
                if (ninjaArray.first().isObject())
                {
                    lastApiNinjaResult = ninjaArray.first().toObject();
                    qDebug() << "API Ninjas result found and stored.";
                }
                else {
                    qWarning() << "API Ninjas array element is not an object.";
                    lastApiNinjaResult["error"] = QLatin1String("Invalid result format in API Ninjas array");
                }
            }
            else {
                qDebug() << "API Ninjas returned an empty array (no match found for criteria).";
                lastApiNinjaResult["message"] = QLatin1String("No match found by API Ninjas");
            }
        }
        else {
            qWarning() << "API Ninjas Response was not a JSON array:" << ninjaResponseData.left(200);
            lastApiNinjaResult["error"] = QLatin1String("Invalid JSON format from API Ninjas (expected array)");
        }
    }

    apiNinjaRequestPending = false; //Mark as finished
    ninjaReply->deleteLater();    //Clean up reply object
    checkAndShowCombinedInfoDialog(); //Check if the other request (if any) is also done
}


//Helper to check completion and trigger Dialog
void AutoMagik::checkAndShowCombinedInfoDialog()
{
    //Only proceed if "both" requests are no longer pending
    if (nhtsaRequestPending || apiNinjaRequestPending)
    {
        qDebug() << "Waiting for other API request(s) to finish... (NHTSA:" << nhtsaRequestPending << ", Ninja:" << apiNinjaRequestPending << ")";
        return; //Not ready yet
    }

    qDebug() << "Both API requests finished (or were not started). Proceeding to show dialog.";

    //Re-enable button (check task index validity again just in case UI changed)
    ui.carInfoButton->setEnabled(currentWorkerTaskIndex >= 0 && currentWorkerTaskIndex < static_cast<int>(tasks.size()));
    ui.carInfoButton->setText(QLatin1String("Car Info"));

    //Now call the function to actually create and show the dialog
    displayCarInfoDialog();
}

//Function to display dialog (uses member variables)
void AutoMagik::displayCarInfoDialog()
{
    //Use the stored make/model/year from when the request was initiated
    QString displayMake = lastMakeForDialog;
    QString displayModel = lastModelForDialog;
    int displayYear = lastYearForDialog;

    //Check if make/model/year are valid before proceeding
    if (displayMake.isEmpty() || displayModel.isEmpty() || displayYear == 0)
    {
        qWarning() << "Cannot display dialog, invalid make/model/year stored.";
        QMessageBox::critical(this, QLatin1String("Internal Error"), QLatin1String("Could not retrieve car details for display."));
        return;
    }

    QDialog* carInfoDialog = new QDialog(this);
    carInfoDialog->setAttribute(Qt::WA_DeleteOnClose); //Ensure cleanup
    carInfoDialog->setWindowTitle(QString("Car Info: %1 %2 (%3)").arg(displayMake, displayModel, QString::number(displayYear)));
    carInfoDialog->setMinimumWidth(550);
    carInfoDialog->setMinimumHeight(400);

    QVBoxLayout* dialogLayout = new QVBoxLayout(carInfoDialog);
    QTextEdit* infoText = new QTextEdit(carInfoDialog);
    infoText->setReadOnly(true);

    QString displayText = "";
    //Check if we stored a valid NHTSA model object (contains essential keys)
    bool nhtsaModelConfirmed = !lastNhtsaResult.isEmpty() && lastNhtsaResult.contains("Model_Name") && lastNhtsaResult.contains("Make_Name");

    //Build display text with NHTSA data
    displayText += QLatin1String("<h3>NHTSA vPIC Result:</h3>");
    if (nhtsaModelConfirmed)
    {
        displayText += QString("<b>Make:</b> %1 (ID: %2)<br>")
            .arg(lastNhtsaResult.value("Make_Name").toString().toHtmlEscaped())
            .arg(lastNhtsaResult.value("Make_ID").toInt());
        displayText += QString("<b>Model:</b> %1 (ID: %2)<br>")
            .arg(lastNhtsaResult.value("Model_Name").toString().toHtmlEscaped())
            .arg(lastNhtsaResult.value("Model_ID").toInt());
        displayText += QString("<b>Year:</b> %1<br>").arg(displayYear);
        displayText += QLatin1String("<i>(Specific model confirmed by NHTSA for this Make/Year)</i><br><br>");
    }
    else {
        //Check if there was an error or message stored
        QString nhtsaMsg;
        if (!lastNhtsaResult.isEmpty() && lastNhtsaResult.contains("error"))
        {
            nhtsaMsg = "Error retrieving NHTSA data: " + lastNhtsaResult.value("error").toString().toHtmlEscaped();
        }
        else if (!lastNhtsaResult.isEmpty() && lastNhtsaResult.contains("message"))
        {
            nhtsaMsg = "NHTSA Result: " + lastNhtsaResult.value("message").toString().toHtmlEscaped();
        }
        else {
            nhtsaMsg = QString("Failed to retrieve or parse NHTSA confirmation for %1 / %2.")
                .arg(displayMake.toHtmlEscaped())
                .arg(displayYear);
        }
        displayText += QString("<i>%1</i><br><br>").arg(nhtsaMsg);
    }

    //Build display text with API Ninjas data
    displayText += QLatin1String("<h3>API Ninjas Details:</h3>");
    //Check if we stored a valid result object (not empty and doesn't contain only an error)
    bool ninjaDataAvailable = !lastApiNinjaResult.isEmpty() && !lastApiNinjaResult.contains("error");

    if (ninjaDataAvailable)
    {
        QStringList preferredOrder = { "make", "model", "year", "fuel_type", "cylinders", "displacement", "transmission", "drive", "class" };
        auto formatLine = [&](const QString& key, const QJsonValue& val) -> QString
            {
                QString valueStr;
                if (val.isDouble()) valueStr = QString::number(val.toDouble(), 'f', 1);
                else if (val.isString()) valueStr = val.toString();
                else valueStr = val.toVariant().toString();

                if (valueStr.isEmpty()) return QString(); //Skip empty values

                QString displayKey = key;
                displayKey = displayKey.replace('_', ' ').replace(0, 1, key.at(0).toUpper());
                if (key.endsWith("mpg")) displayKey += QLatin1String(" (MPG)");
                else if (key == QLatin1String("displacement")) displayKey += QLatin1String(" (L)"); //Add units if known

                return QString("<b>%1:</b> %2<br>").arg(displayKey.toHtmlEscaped()).arg(valueStr.toHtmlEscaped());
            };

        displayText += QLatin1String("<i>(Showing best match found - details may vary from specific vehicle)</i><br>");
        for (const QString& key : preferredOrder)
        {
            if (lastApiNinjaResult.contains(key))
            {
                displayText += formatLine(key, lastApiNinjaResult.value(key));
            }
        }
        displayText += QLatin1String("<br>");
    }
    else {
        QString ninjaMsg;
        //Check if the request was skipped due to missing key
        if (apiKey.isEmpty() || apiKey == QLatin1String("YOUR_API_KEY_HERE"))
        {
            ninjaMsg = QLatin1String("API Ninjas request skipped (API key missing).");
        }
        else if (!lastApiNinjaResult.isEmpty() && lastApiNinjaResult.contains("error"))
        {
            ninjaMsg = "Error retrieving API Ninjas data: " + lastApiNinjaResult.value("error").toString().toHtmlEscaped();
        }
        else if (!lastApiNinjaResult.isEmpty() && lastApiNinjaResult.contains("message"))
        {
            //Show message if stored (e.g., "No match found")
            ninjaMsg = "API Ninjas: " + lastApiNinjaResult.value("message").toString().toHtmlEscaped();
        }
        else {
            //General failure message
            ninjaMsg = QLatin1String("No additional details found via API Ninjas or request failed.");
        }
        displayText += QString("<i>%1</i><br><br>").arg(ninjaMsg);
    }

    displayText += QLatin1String("<hr><i>Note: API Ninjas data provides general specs. VIN decoding is needed for exact vehicle specifications.</i>");

    infoText->setHtml(displayText);
    dialogLayout->addWidget(infoText);

    //Image Button (Always render, enabled if make/model/year are valid)
    QPushButton* imageButton = new QPushButton(QLatin1String("Find Image"), carInfoDialog);
    imageButton->setMinimumHeight(40);
    imageButton->setEnabled(!displayMake.isEmpty() && !displayModel.isEmpty() && displayYear != 0); //Enable based on valid data
    dialogLayout->addWidget(imageButton);

    //Connect Wikimedia image button
    QObject::connect(imageButton, &QPushButton::clicked, this, [this, carInfoDialog, displayMake, displayModel, displayYear, imageButton]() mutable {

        QString searchTerm = QString("%1 %2 %3").arg(displayMake, displayModel.split(' ').first(), QString::number(displayYear));

        //Creating the image search URL
        QUrl url("https://commons.wikimedia.org/w/api.php");
        QUrlQuery query;
        query.addQueryItem("action", "query");
        query.addQueryItem("generator", "search");
        query.addQueryItem("gsrsearch", searchTerm + " filetype:jpg"); //Prioritize JPEGs
        query.addQueryItem("gsrnamespace", "6");
        query.addQueryItem("gsrlimit", "1");     //Get the top image result
        query.addQueryItem("prop", "imageinfo"); //Get image details
        query.addQueryItem("iiprop", "url|size|mime");
        query.addQueryItem("format", "json");
        query.addQueryItem("formatversion", "2");
        url.setQuery(query);

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "Dummy");
        qDebug() << "Requesting Wikimedia Commons API:" << request.url().toString();

        //Create a temporary manager parented to the dialog for auto-cleanup
        QNetworkAccessManager* tempManager = new QNetworkAccessManager(carInfoDialog);

        imageButton->setEnabled(false);
        imageButton->setText(QLatin1String("Loading Image..."));

        QObject::connect(tempManager, &QNetworkAccessManager::finished,
            [carInfoDialog, imageButton, searchTerm, tempManager](QNetworkReply* imageReply) mutable
            {

                //Auto-delete reply when done
                imageReply->deleteLater();
                //TempManager will be deleted when carInfoDialog (its parent) is deleted

                //Check if carInfoDialog still exists before using it
                if (!carInfoDialog)
                {
                    qWarning() << "Car info dialog was closed before Wikimedia reply finished.";
                    return;
                }

                imageButton->setEnabled(true); //Re-enable button regardless of outcome
                imageButton->setText(QLatin1String("Find Image"));

                if (imageReply->error() != QNetworkReply::NoError)
                {
                    qWarning() << "Wikimedia API Request Error:" << imageReply->errorString();
                    QMessageBox::warning(carInfoDialog, QLatin1String("Wikimedia API Error"), QLatin1String("Failed to fetch image info: ") + imageReply->errorString());
                }
                else {
                    QByteArray imageData = imageReply->readAll();
                    qDebug() << "Wikimedia Response:" << imageData.left(500) << "..."; //Log start of response
                    QJsonDocument imageDoc = QJsonDocument::fromJson(imageData);
                    QString imageUrl;

                    //Parse the Wikimedia JSON 
                    if (imageDoc.isObject() && imageDoc.object().contains("query"))
                    {
                        QJsonObject queryObj = imageDoc.object()["query"].toObject();
                        if (queryObj.contains("pages") && queryObj["pages"].isArray())
                        {
                            QJsonArray pages = queryObj["pages"].toArray();
                            if (!pages.isEmpty() && pages[0].isObject()) {
                                QJsonObject page = pages[0].toObject();
                                //Check if imageinfo exists and is an array
                                if (page.contains("imageinfo") && page["imageinfo"].isArray())
                                {
                                    QJsonArray imageInfoArr = page["imageinfo"].toArray();
                                    if (!imageInfoArr.isEmpty() && imageInfoArr[0].isObject())
                                    {
                                        QJsonObject imageInfo = imageInfoArr[0].toObject();
                                        //Prioritize 'url'
                                        if (imageInfo.contains("url"))
                                        {
                                            imageUrl = imageInfo["url"].toString();
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (!imageUrl.isEmpty())
                    {
                        //Use the existing carInfoDialog as parent for the webDialog
                        QDialog* webDialog = new QDialog(carInfoDialog); //Parent ensures it closes if info dialog closes
                        webDialog->setWindowTitle("Image Preview: " + searchTerm);
                        webDialog->setMinimumSize(800, 600);
                        webDialog->setAttribute(Qt::WA_DeleteOnClose); //Delete when closed
                        QVBoxLayout* webLayout = new QVBoxLayout(webDialog);
                        QWebEngineView* webView = new QWebEngineView(webDialog);
                        qDebug() << "Loading image URL:" << imageUrl;
                        webView->setUrl(QUrl(imageUrl));
                        webLayout->addWidget(webView);
                        QPushButton* closeButton = new QPushButton(QLatin1String("Close Preview"), webDialog);
                        connect(closeButton, &QPushButton::clicked, webDialog, &QDialog::accept);
                        webLayout->addWidget(closeButton);
                        webDialog->open();
                    }
                    else {
                        QMessageBox::information(carInfoDialog, QLatin1String("Image Not Found"), QString("No suitable image found on Wikimedia Commons for '%1'.").arg(searchTerm));
                    }
                }
            });

        tempManager->get(request); //Send the Wikimedia request
        });

    //OK Button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, carInfoDialog);
    dialogLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, carInfoDialog, &QDialog::accept); //Close dialog on OK

    carInfoDialog->open();
}

//Function for adding tasks
void AutoMagik::editSelectedTask()
{
    int rowIndex = ui.tasksTableWidget->currentRow(); //Get selected task index
    if (rowIndex < 0 || rowIndex >= static_cast<int>(tasks.size())) //Checking if the selection is valid
    {
        QMessageBox::warning(this, QLatin1String("No Task Selected"), QLatin1String("Please select a task from the list first."));
        return;
    }
    //Get the selected task
    Task& selectedTask = tasks[rowIndex];

    QDialog dialog(this);
    dialog.setWindowTitle(QLatin1String("Edit Task"));
    dialog.setMinimumWidth(450);
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    QComboBox* carSelection = new QComboBox(&dialog);
    for (size_t i = 0; i < cars.size(); ++i)
    {
        const auto& car = cars[i];
        carSelection->addItem(QString::fromStdString(car.getMake() + " " + car.getModel() + " (" + std::to_string(car.getProductionYear()) + ")"),
            QVariant::fromValue(static_cast<int>(i))); //Store index in data role
    }


    //Priority combo box
    QComboBox* priorityComboBox = new QComboBox(&dialog);
    priorityComboBox->addItem(QLatin1String("Low"), QVariant::fromValue(LOW));
    priorityComboBox->addItem(QLatin1String("Medium"), QVariant::fromValue(MEDIUM));
    priorityComboBox->addItem(QLatin1String("High"), QVariant::fromValue(HIGH));
    priorityComboBox->setCurrentText(QString::fromStdString(selectedTask.getTaskPriority())); //Set current index to task's priority

    QTextEdit* instructionsInput = new QTextEdit(&dialog);
    instructionsInput->setText(QString::fromStdString(selectedTask.getTaskInstructions()));
    instructionsInput->setMinimumHeight(80);
    QTextEdit* partsInput = new QTextEdit(&dialog);
    partsInput->setPlaceholderText(QLatin1String("Enter parts needed (optional)"));
    if (selectedTask.getPartsNeeded() != "") //Check if parts are needed
    {
        partsInput->setText(QString::fromStdString(selectedTask.getPartsNeeded()));
    }
    partsInput->setMinimumHeight(60);
    QTextEdit* initialCommentsInput = new QTextEdit(&dialog);
    initialCommentsInput->setPlaceholderText(QLatin1String("Enter initial comments (optional)"));
    if (selectedTask.getComments() != "") //Check if there are any comments
    {
        initialCommentsInput->setText(QString::fromStdString(selectedTask.getComments()));
    }
    initialCommentsInput->setMinimumHeight(60);

    QFormLayout* form = new QFormLayout(); //Form layout
    form->addRow(new QLabel(QLatin1String("Select Priority:"), &dialog)); //Priority selection
    form->addWidget(priorityComboBox);

    form->addRow(new QLabel(QLatin1String("Select Car:"), &dialog), carSelection); //Car selection
    mainLayout->addLayout(form);

    mainLayout->addWidget(new QLabel(QLatin1String("Instructions:"), &dialog));
    mainLayout->addWidget(instructionsInput);
    mainLayout->addWidget(new QLabel(QLatin1String("Parts Needed:"), &dialog));
    mainLayout->addWidget(partsInput);
    mainLayout->addWidget(new QLabel(QLatin1String("Initial Comments:"), &dialog));
    mainLayout->addWidget(initialCommentsInput);

    mainLayout->addStretch();

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        if (instructionsInput->toPlainText().trimmed().isEmpty())
        {
            QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Task Instructions cannot be empty."));
        }
        else {
            dialog.accept();
        }
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        Task newTask;
        newTask.setTaskID(static_cast<int>(tasks.size()) + 5001); //Simple temporary ID
        newTask.setTaskInstructions(instructionsInput->toPlainText().trimmed().toStdString());
        newTask.setPartsNeeded(partsInput->toPlainText().trimmed().toStdString());
        newTask.setComments(initialCommentsInput->toPlainText().trimmed().toStdString());

        priority selectedPriority = static_cast<priority>(priorityComboBox->currentData().toInt());
        newTask.setTaskPriority(selectedPriority); //Set selected priority;

        int selectedCarDataIndex = carSelection->currentData().toInt(); //Get stored index
        if (selectedCarDataIndex >= 0 && selectedCarDataIndex < static_cast<int>(cars.size()))
        {
            newTask.setTaskCar(cars[selectedCarDataIndex]); //Assign selected car by reference/copy
        }
        else {
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Invalid car selection index."));
            return; //Should not happen if populated correctly
        }

        newTask.setTaskStatus(NEW); //Set default status to NEW

        tasks[rowIndex] = newTask; //Update the task in the vector

        updateManagerTables(); //Refresh UI
        updateWorkerDashboard(); //Also refresh worker view
    }

}


//Function for assigning/reassigning tasks to workers
void AutoMagik::assignReassignTask()
{
    int selectedTaskIndex = ui.tasksTableWidget->currentRow(); //Getting the selected row index
    if (selectedTaskIndex < 0 || selectedTaskIndex >= static_cast<int>(tasks.size())) //Checking if the selection is valid
    {
        QMessageBox::warning(this, QLatin1String("No Task Selected"), QLatin1String("Please select a task from the list first."));
        return;
    }

    int currentWorkerID = this->tasks[selectedTaskIndex].getTaskWorkerID(); //Getting the current worker ID

    QDialog* dialog = new QDialog(this); //Creating a dialog box
    dialog->setWindowTitle(QLatin1String("Assign/Reassign Task"));
    dialog->setMinimumWidth(450);
    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    QFormLayout* form = new QFormLayout(); //Form layout

    QLabel* label = new QLabel(QLatin1String("Select Worker:"), dialog);
    QComboBox* workerSelection = new QComboBox(dialog);
    for (const auto& worker : workers)
    {
        workerSelection->addItem(QString::number(worker.getWorkerID()), QVariant(worker.getWorkerID())); //Adding a worker to the combo box
    }

    if (currentWorkerID != 0) //Checking if there is a worker assigned to the task
    {
        workerSelection->setCurrentText(QString::number(currentWorkerID)); //Setting the current combobox text to the assigned worker
    }
    else
    {
        workerSelection->setCurrentIndex(-1); //No worker assigned
    }

    form->addRow(label, workerSelection); //Adding the label and combobox to the form
    mainLayout->addLayout(form);
    mainLayout->addStretch();

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() { dialog->accept(); });
    connect(&buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    if (dialog->exec() == QDialog::Accepted)
    {
        int selectedWorkerID = workerSelection->currentData().toInt(); //Getting the selected worker ID

        if (selectedWorkerID <= 0) //Checking if the selection is valid
        {
            QMessageBox::warning(this, QLatin1String("No Worker Selected"), QLatin1String("Please select a worker from the list first."));
            return;
        }

        //Clearing the task from the previous worker
        if (currentWorkerID != 0)
        {
            for (auto& worker : workers)
            {
                if (worker.getWorkerID() == currentWorkerID)
                {
                    worker.clearTask(); //Clearing the task from the previous worker
                    break;
                }
            }
        }

        //Assigning the task to the new worker
        for (auto& worker : workers)
        {
            if (worker.getWorkerID() == selectedWorkerID)
            {
                worker.assignTask(this->tasks[selectedTaskIndex]); //Assign the task to the selected worker
                this->tasks[selectedTaskIndex].setTaskWorkerID(worker.getWorkerID()); //Updating the task's worker ID
                break;
            }
        }

        updateManagerTables(); // Refreshing UI
        updateWorkerDashboard(); // Also refreshing worker view
    }
}


//Function for deleting tasks
void AutoMagik::deleteTask()
{
    int selectedTaskIndex = ui.tasksTableWidget->currentRow(); //Getting the selected row index
    if (selectedTaskIndex < 0 || selectedTaskIndex >= static_cast<int>(tasks.size())) //Checking if the selection is valid
    {
        QMessageBox::warning(this, QLatin1String("No Task Selected"), QLatin1String("Please select a task from the list first."));
        return;
    }

    QDialog* dialog = new QDialog(this); //Creating a dialog box
    dialog->setWindowTitle(QLatin1String("Delete Task"));
    dialog->setMinimumWidth(450);

    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);

    QFormLayout* form = new QFormLayout(); //Form layout

    form->addRow(new QLabel(QLatin1String("Are you sure you want to delete this task?")));
    mainLayout->addLayout(form);
    mainLayout->addStretch();

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() { dialog->accept(); });
    connect(&buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    if (dialog->exec() == QDialog::Accepted)
    {
        if (this->tasks[selectedTaskIndex].getTaskWorkerID() != 0)
        {
            for (auto& worker : workers)
            {
                if (this->tasks[selectedTaskIndex].getTaskWorkerID() == worker.getWorkerID())
                {
                    worker.clearTask();
                }
            }
        }

        //Removing the task from the task vector
        this->tasks.erase(tasks.begin() + selectedTaskIndex);

        //Removing the row from the table
        ui.tasksTableWidget->removeRow(selectedTaskIndex);



        updateManagerTables(); //Refreshing UI
        updateWorkerDashboard(); //Also refreshing worker view
    }
}

//Function for editing cars
void AutoMagik::editSelectedCar()
{
    int selectedCarIndex = ui.carsTableWidget->currentRow(); //Getting the selected row index
    if (selectedCarIndex < 0 || selectedCarIndex >= static_cast<int>(cars.size())) {
        QMessageBox::warning(this, QLatin1String("No Car Selected"), QLatin1String("Please select a car from the list first."));
        return;
    }

    QDialog dialog(this); //Oppening a dialog box
    dialog.setWindowTitle(QLatin1String("Edit Car"));
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    //Inputs:
    QLineEdit* makeInput = new QLineEdit(&dialog);
    makeInput->setText(QString::fromStdString(cars[selectedCarIndex].getMake()));
    QLineEdit* modelInput = new QLineEdit(&dialog);
    modelInput->setText(QString::fromStdString(cars[selectedCarIndex].getModel()));
    QLineEdit* engineInput = new QLineEdit(&dialog);
    engineInput->setText(QString::fromStdString(cars[selectedCarIndex].getEngineType()));
    QComboBox* yearInput = new QComboBox(&dialog);
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear; i >= 1900; i--) {
        yearInput->addItem(QString::number(i));
    }
    yearInput->setCurrentText(QString::number(cars[selectedCarIndex].getProductionYear()));
    QLineEdit* phoneNumberInput = new QLineEdit(&dialog);
    phoneNumberInput->setText(QString::number(cars[selectedCarIndex].getClientPhoneNumber()));

    //Phone number validation
    QRegularExpression phoneRegex(QStringLiteral("^\\+?[\\d\\s]+$"));
    phoneNumberInput->setValidator(new QRegularExpressionValidator(phoneRegex, phoneNumberInput));

    QFormLayout* formLayout = new QFormLayout(); //Creating a form layout
    //Adding everything to the form input
    formLayout->addRow(QLatin1String("Make:"), makeInput);
    formLayout->addRow(QLatin1String("Model:"), modelInput);
    formLayout->addRow(QLatin1String("Year:"), yearInput);
    formLayout->addRow(QLatin1String("Engine:"), engineInput);
    formLayout->addRow(QLatin1String("Owner Phone:"), phoneNumberInput);

    //Rolling back mileage
    QPushButton* reverseMileageButton = new QPushButton(QLatin1String("Rollback Mileage"), &dialog);
    connect(reverseMileageButton, &QPushButton::clicked, this, [this, selectedCarIndex]() {

        QDialog mileageDialog(this);
        mileageDialog.setWindowTitle(QLatin1String("Rollback Mileage"));
        QVBoxLayout mileageLayout(&mileageDialog);

        //Mileage spinbox
        QSpinBox* mileageInput = new QSpinBox(&mileageDialog);
        int currentMileage = cars[selectedCarIndex].getCarMileage();
        mileageInput->setRange(0, currentMileage);
        mileageInput->setValue(currentMileage);
        mileageInput->setSingleStep(10000);
        mileageLayout.addWidget(mileageInput);

        QDialogButtonBox mileageButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &mileageDialog);
        mileageLayout.addWidget(&mileageButtonBox);

        connect(&mileageButtonBox, &QDialogButtonBox::accepted, [&]() {
            int newMileage = mileageInput->value();
            cars[selectedCarIndex].setCarMileage(newMileage);
            updateManagerTables();
            mileageDialog.accept();
            });
        connect(&mileageButtonBox, &QDialogButtonBox::rejected, &mileageDialog, &QDialog::reject);

        mileageDialog.exec();
        });

    formLayout->addRow(reverseMileageButton);
    mainLayout->addLayout(formLayout);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        QString phoneText = phoneNumberInput->text().trimmed();
        QString makeText = makeInput->text().trimmed();
        QString modelText = modelInput->text().trimmed();

        if (makeText.isEmpty() || modelText.isEmpty()) {
            QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Car Make and Model cannot be empty."));
            return;
        }

        QString phoneCleaned = phoneText;
        phoneCleaned.remove(' ').remove('+');
        bool isNumeric;
        qlonglong phoneLong = phoneCleaned.toLongLong(&isNumeric);

        if (!isNumeric || phoneCleaned.isEmpty()) {
            QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Invalid phone number."));
            return;
        }

        //Setting the inputs to the car object
        cars[selectedCarIndex].setCarMake(makeInput->text().trimmed().toStdString());
        cars[selectedCarIndex].setCarModel(modelInput->text().trimmed().toStdString());
        cars[selectedCarIndex].setEngineType(engineInput->text().trimmed().toStdString());
        cars[selectedCarIndex].setProductionYear(yearInput->currentText().toInt());
        cars[selectedCarIndex].setClientPhoneNumber(static_cast<int>(phoneLong));

        updateManagerTables();
        dialog.accept();
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.exec();
}


//Function for deleting cars
void AutoMagik::deleteSelectedCar()
{
    int selectedCarIndex = ui.carsTableWidget->currentRow(); //Getting the selected row index
    if (selectedCarIndex < 0 || selectedCarIndex >= static_cast<int>(cars.size())) //Checking if the selection is valid
    {
        QMessageBox::warning(this, QLatin1String("No Car Selected"), QLatin1String("Please select a car from the list first."));
        return;
    }

    //Checking if the car is assigned to any task
    for (const auto& task : tasks)
    {
        if (task.getCarObject() == cars[selectedCarIndex])
        {
            QMessageBox::warning(this, QLatin1String("Car Assigned"), QLatin1String("This car is assigned to a task. Please delete the task first."));
            return; //Exiting if the car is assigned to a task
        }
    }

    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(QLatin1String("Delete Car"));
    dialog->setMinimumWidth(450);
    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    QFormLayout* form = new QFormLayout();
    form->addRow(new QLabel(QLatin1String("Are you sure you want to delete this car?")));
    mainLayout->addLayout(form);
    mainLayout->addStretch();

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    if (dialog->exec() == QDialog::Accepted)
    {
        //Delete the selected car
        cars.erase(cars.begin() + selectedCarIndex); //Remove the car from the vector
        ui.carsTableWidget->removeRow(selectedCarIndex); //Remove the row from the table

        updateManagerTables(); //Refresh the UI
    }
}

//Function for editing workers
void AutoMagik::editSelectedWorker()
{
    int selectedWorkerIndex = ui.workersTableWidget->currentRow(); //Getting the selected row index
    QDialog dialog(this);
    dialog.setWindowTitle(QLatin1String("Edit Worker"));
    dialog.setMinimumWidth(400);

    QFormLayout* formLayout = new QFormLayout(&dialog);

    QLineEdit* nameInput = new QLineEdit(&dialog);
    nameInput->setText(QString::fromStdString(workers[selectedWorkerIndex].getWorkerName()));
    QLineEdit* positionInput = new QLineEdit(&dialog);
    positionInput->setText(QString::fromStdString(workers[selectedWorkerIndex].getPosition()));
    QSpinBox* experienceInput = new QSpinBox(&dialog);
    experienceInput->setSuffix(QLatin1String(" years"));
    experienceInput->setRange(0, 60);
    experienceInput->setValue(workers[selectedWorkerIndex].getWorkerExperience());
    QSpinBox* salaryInput = new QSpinBox(&dialog);
    salaryInput->setPrefix(QLatin1String("$ "));
    salaryInput->setRange(0, 500000);
    salaryInput->setSingleStep(1000);
    salaryInput->setValue(workers[selectedWorkerIndex].getWorkerSalary());
    QSpinBox* ageInput = new QSpinBox(&dialog);
    ageInput->setRange(18, 100);
    ageInput->setValue(workers[selectedWorkerIndex].getWorkerAge());

    formLayout->addRow(new QLabel(QLatin1String("Worker Name:"), &dialog), nameInput);
    formLayout->addRow(new QLabel(QLatin1String("Position:"), &dialog), positionInput);
    formLayout->addRow(new QLabel(QLatin1String("Experience:"), &dialog), experienceInput);
    formLayout->addRow(new QLabel(QLatin1String("Salary:"), &dialog), salaryInput);
    formLayout->addRow(new QLabel(QLatin1String("Age:"), &dialog), ageInput);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout->addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        if (nameInput->text().trimmed().isEmpty() || positionInput->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Worker Name and Position cannot be empty."));
            //Keep dialog open
        }
        else {
            dialog.accept(); //Close dialog
        }
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        workers[selectedWorkerIndex].setWorkerName(nameInput->text().trimmed().toStdString());
        workers[selectedWorkerIndex].setPosition(positionInput->text().trimmed().toStdString());
        workers[selectedWorkerIndex].setWorkerExperience(experienceInput->value());
        workers[selectedWorkerIndex].setWorkerSalary(salaryInput->value());
        workers[selectedWorkerIndex].setWorkerAge(ageInput->value());

        updateManagerTables(); //Refresh the UI
    }
}

//Function for deleting workers
void AutoMagik::deleteSelectedWorker()
{
    int selectedWorkerIndex = ui.workersTableWidget->currentRow(); //Getting the selected row index
    if (selectedWorkerIndex < 0 || selectedWorkerIndex >= static_cast<int>(workers.size())) //Checking if the selection is valid
    {
        QMessageBox::warning(this, QLatin1String("No Car Selected"), QLatin1String("Please select a worker from the list first."));
        return;
    }

    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(QLatin1String("Delete Worker"));
    dialog->setMinimumWidth(450);
    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    QFormLayout* form = new QFormLayout();

    form->addRow(new QLabel(QLatin1String("Are you sure you want to delete this worker?")));

    mainLayout->addLayout(form);
    mainLayout->addStretch();

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    if (dialog->exec() == QDialog::Accepted)
    {
        //Delete the selected worker
        workers.erase(workers.begin() + selectedWorkerIndex); //Remove the worker from the vector
        ui.workersTableWidget->removeRow(selectedWorkerIndex); //Remove the row from the table

        updateManagerTables(); //Refresh the UI
    }
}