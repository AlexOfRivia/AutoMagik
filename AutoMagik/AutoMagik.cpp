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
#include <QWebEngineProfile>
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

    //Same thing for the firebase key
    QByteArray firebaseAPIKey = qgetenv("FIREBASE_KEY");
    if (!firebaseAPIKey.isEmpty()) {
        firebase.setAPIKey(QString::fromUtf8(firebaseAPIKey));
        qDebug() << "Firebase Key loaded successfully from environment variable.";
    }
    else {
        firebase.setAPIKey("YOUR_API_KEY_HERE_2");
        qWarning() << "WARNING: Environment variable 'FIREBASE_KEY' not set or empty."
            << "Using placeholder key. Firebase functionality may fail.";
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
                updateManagerTables();
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
    QObject::connect(ui.workerLoginButton, &QPushButton::clicked, [this]() {
        QString email = ui.workerLoginEmail->toPlainText();
        QString password = ui.workerPasswordLogin->toPlainText();

        QObject::disconnect(&firebase, &Firebase::managerSignedIn, nullptr, nullptr);
        QObject::disconnect(&firebase, &Firebase::workerSignedIn, nullptr, nullptr);


        connect(&firebase, &Firebase::workerSignedIn, [this](const QString& workerName) {

            QString workerUrl = QString("https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/workers.json?auth=%1")
                .arg(firebase.getIdToken());

            QNetworkRequest workerRequest((QUrl(workerUrl)));
            QNetworkReply* workerReply = firebase.getNetworkAccessManager()->get(workerRequest);

            connect(workerReply, &QNetworkReply::finished, [this, workerReply]() {
                QByteArray data = workerReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(data);
                QJsonObject allWorkers = doc.object();

                for (const QString& key : allWorkers.keys()) {
                    QJsonObject workerData = allWorkers[key].toObject();
                    if (workerData["email"].toString() == ui.workerLoginEmail->toPlainText()) {
                        this->currentWorkerId = workerData["w_id"].toInt();
                        break;
                    }
                }

                workerReply->deleteLater();
                ui.stackedWidget->setCurrentIndex(5);
                updateWorkerDashboard(); 
                });
            });

        firebase.signUserIn(email, password);
        });

    //Worker Dashboard Page
    QObject::connect(ui.workerBackButton, &QPushButton::clicked, [this]() {
        this->currentWorkerId = -1; // delete id during login
        ui.stackedWidget->setCurrentIndex(0);
        ui.workerLoginEmail->clear();
        ui.workerPasswordLogin->clear();
        });  //Sign Out -> Mode Selection

    //Worker dashboard connections
    //When the selected item in the task list changes, update the details view
    QObject::connect(ui.workerTaskListWidget, &QListWidget::currentItemChanged,
        this, &AutoMagik::updateWorkerDashboardSelection);
    //Connect "Car Info" button
    QObject::connect(ui.carInfoButton, &QPushButton::clicked, this, &AutoMagik::showCarInfo);


    //added button which adds comment from workers to comment
    QObject::connect(ui.addCommentButton, &QPushButton::clicked, this, &AutoMagik::addCommentToTask);
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


/*

TODO IN UPDATEMANAGERTABLES:
FIX IT NOT REFRESHING WHEN YOU USE THE FUNCTION, AND ONLY DOING SO AFTER LOGGING IN AGAIN
( MAY HAVE SMTH TO DO WITH THE ID TOKEN NOT REFRESHING )

*/


//Helper function to update tables and buttons
void AutoMagik::updateManagerTables()
{

    //Clearing up workers
    workers.clear();
    QString workerUrl = "https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/workers.json?auth=" + firebase.getIdToken();
    //QString carUrl = "https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/cars.json?auth=" + firebase.getIdToken(); - cars will be implemented later
    //Same will go for tasks
    //Adding workers from db
    QNetworkRequest workerRequest((QUrl(workerUrl)));
    workerRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* workerReply = firebase.getNetworkAccessManager()->get(workerRequest); 

    QEventLoop loop;
    connect(workerReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray data = workerReply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject allWorkers = doc.object();

    for (const QString& key : allWorkers.keys()) {
        QJsonObject workerData = allWorkers[key].toObject();
        if (workerData["manager"].toString() == firebase.m_uid) {
            Worker w;
            w.setWorkerID(workerData["w_id"].toInt());
            w.setWorkerExperience(workerData["experience"].toInt());
            w.setWorkerName(workerData["name"].toString().toStdString());
            w.setWorkerAge(workerData["age"].toInt());
            w.setPosition(workerData["position"].toString().toStdString());
            w.setWorkerSalary(workerData["salary"].toInt());
            w.setFirebaseKey(key.toStdString()); // adding firebase key
            w.setEmail(workerData["email"].toString().toStdString()); // adding email
            workers.push_back(w);
        }
    }

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
    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto& task = tasks[i];
        if (currentWorkerId != -1 && task.getTaskWorkerID() != currentWorkerId) {
            continue;
        }
        QString carDesc = QString::fromStdString(task.getCarObject().getMake() + " " + task.getCarObject().getModel());
        QString instructionPreview = QString::fromStdString(task.getTaskInstructions());
        if (instructionPreview.length() > 30) {
            instructionPreview = instructionPreview.left(30) + QLatin1String("...");
        }

        QString itemText = QString("ID: %1 - %2 (%3)")
            .arg(task.getTaskID())
            .arg(carDesc)
            .arg(instructionPreview);
        QListWidgetItem* item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<int>(i)));
        ui.workerTaskListWidget->addItem(item);
    }
    ui.addCommentButton->setEnabled(true);
    updateWorkerDashboardSelection();
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
            QRegularExpression emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
            QRegularExpressionMatch match = emailRegex.match(emailInput->text().trimmed());

            if (nameInput->text().trimmed().isEmpty() || positionInput->text().trimmed().isEmpty() || passwordInput->text().trimmed().isEmpty())
            {
                QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Worker Name, Position and Password cannot be empty."));
            }
            else if (!match.hasMatch())
            {
                QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Email form is incorrect."));
            }
            else
            {
                dialog.accept();
            }
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        Worker newWorker;
        newWorker.setWorkerID(static_cast<int>(workers.size()) + 1001);
        newWorker.setWorkerName(nameInput->text().trimmed().toStdString());
        newWorker.setPosition(positionInput->text().trimmed().toStdString());
        newWorker.setWorkerExperience(experienceInput->value());
        newWorker.setWorkerSalary(salaryInput->value());
        newWorker.setWorkerAge(ageInput->value());

        QVariantMap data;
        data["w_id"] = QVariant(newWorker.getWorkerID());
        data["name"] = QVariant(QString::fromStdString(newWorker.getWorkerName()));
        data["position"] = QVariant(QString::fromStdString(newWorker.getPosition()));
        data["experience"] = QVariant(newWorker.getWorkerExperience());
        data["salary"] = QVariant(newWorker.getWorkerSalary());
        data["age"] = QVariant(newWorker.getWorkerAge());
        data["manager"] = QVariant(firebase.m_uid);

        // Connect to the workerAccountCreated signal before adding the worker
        connect(&firebase, &Firebase::workerAccountCreated, this,
            [this, newWorker, email = emailInput->text().trimmed()](const QString& fbKey) {
                Worker workerWithKey = newWorker;
                workerWithKey.setFirebaseKey(fbKey.toStdString());
                workerWithKey.setEmail(email.toStdString());
                workers.push_back(workerWithKey);
                updateManagerTables();
            });

        firebase.addWorkerAccount(emailInput->text().trimmed(), data, passwordInput->text().trimmed());
    }
}

//Add New Task
void AutoMagik::addTask()
{
    if (cars.empty()) {
        QMessageBox::information(this, "No Cars Available", "Please add a car before adding a task.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Add New Task");
    dialog.setMinimumWidth(450);
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    // Car selection
    QComboBox* carSelection = new QComboBox(&dialog);
    for (size_t i = 0; i < cars.size(); ++i) {
        const auto& car = cars[i];
        carSelection->addItem(
            QString::fromStdString(car.getMake() + " " + car.getModel() + " (" + std::to_string(car.getProductionYear()) + ")"),
            QVariant::fromValue(static_cast<int>(i))
        );
    }
    // Worker selection
    QComboBox* workerComboBox = new QComboBox(&dialog);
    if (!workers.empty()) {
        for (const auto& worker : workers) {
            workerComboBox->addItem(QString::fromStdString(worker.getWorkerName() + ", " + worker.getPosition()));
        }
    }
    else {
        workerComboBox->addItem("No Workers Available");
        workerComboBox->setDisabled(true);
    }

    // Priority selection
    QComboBox* priorityComboBox = new QComboBox(&dialog);
    priorityComboBox->addItem("Low", QVariant::fromValue(LOW));
    priorityComboBox->addItem("Medium", QVariant::fromValue(MEDIUM));
    priorityComboBox->addItem("High", QVariant::fromValue(HIGH));

    // Inputs
    QTextEdit* instructionsInput = new QTextEdit(&dialog);
    instructionsInput->setPlaceholderText("Enter task instructions (required)");
    instructionsInput->setMinimumHeight(80);

    QTextEdit* partsInput = new QTextEdit(&dialog);
    partsInput->setPlaceholderText("Enter parts needed (optional)");
    partsInput->setMinimumHeight(60);

    QTextEdit* initialCommentsInput = new QTextEdit(&dialog);
    initialCommentsInput->setPlaceholderText("Enter initial comments (optional)");
    initialCommentsInput->setMinimumHeight(60);

    // Form layout
    QFormLayout* form = new QFormLayout();
    form->addRow(new QLabel("Priority:"), priorityComboBox);
    form->addRow(new QLabel("Car:"), carSelection);
    form->addRow(new QLabel("Worker:"), workerComboBox);
    mainLayout->addLayout(form);
    mainLayout->addWidget(new QLabel("Instructions:"));
    mainLayout->addWidget(instructionsInput);
    mainLayout->addWidget(new QLabel("Parts Needed:"));
    mainLayout->addWidget(partsInput);
    mainLayout->addWidget(new QLabel("Initial Comments:"));
    mainLayout->addWidget(initialCommentsInput);
    mainLayout->addStretch();

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        if (instructionsInput->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Input Error", "Task Instructions cannot be empty.");
        }
        else {
            dialog.accept();
        }
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        // Gather data into a Task
        Task newTask;
        newTask.setTaskID(static_cast<int>(tasks.size()) + 5001);
        newTask.setTaskInstructions(instructionsInput->toPlainText().trimmed().toStdString());
        newTask.setPartsNeeded(partsInput->toPlainText().trimmed().toStdString());
        newTask.setComments(initialCommentsInput->toPlainText().trimmed().toStdString());
        newTask.setTaskPriority(static_cast<priority>(priorityComboBox->currentData().toInt()));

        int carIndex = carSelection->currentData().toInt();
        newTask.setTaskCar(cars[carIndex]);

        int workerId = 0;
        QString workerName;
        if (!workers.empty() && workerComboBox->currentIndex() >= 0) {
            workerId = workers[workerComboBox->currentIndex()].getWorkerID();
            workerName = QString::fromStdString(workers[workerComboBox->currentIndex()].getWorkerName());
        }
        newTask.setTaskWorkerID(workerId);
        newTask.setTaskStatus(NEW);

        // Add to local list
        int index = static_cast<int>(tasks.size());
        tasks.push_back(newTask);
        updateManagerTables();
        updateWorkerDashboard();

        // Connect to Firebase signal before sending
        connect(&firebase, &Firebase::taskAdded, this, [this, index](const QString& fbKey) {
            tasks[index].setFirebaseKey(fbKey.toStdString());
            updateManagerTables();
            });

        // Send to Firebase
        const Car& car = cars[carIndex];
        firebase.addTaskToDatabase(
            QString::fromStdString(car.getMake()),
            QString::fromStdString(car.getModel()),
            instructionsInput->toPlainText().trimmed(),
            partsInput->toPlainText().trimmed(),
            initialCommentsInput->toPlainText().trimmed(),
            workerId,
            "New",
            priorityComboBox->currentText(),
            firebase.getIdToken(),
            workerName
        );
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
            int index = static_cast<int>(cars.size()) - 1; 
            connect(&firebase, &Firebase::carAdded, this, [this, index](const QString& fbKey) {
                if (index >= 0 && index < static_cast<int>(cars.size())) {
                    cars[index].setFirebaseKey(fbKey.toStdString());
                    updateManagerTables(); 
                }
                });

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
	makeLabel->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
    QLabel* modelLabel = new QLabel(QString::fromStdString(associatedCar.getModel()), ui.carInfoGroupBox);
	modelLabel->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
    QLabel* yearLabel = new QLabel(QString::number(associatedCar.getProductionYear()), ui.carInfoGroupBox);
	yearLabel->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
    QLabel* engineLabel = new QLabel(QString::fromStdString(associatedCar.getEngineType()), ui.carInfoGroupBox);
	engineLabel->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
    QLabel* mileageLabel = new QLabel(QString::number(associatedCar.getCarMileage()) + QLatin1String(" km"), ui.carInfoGroupBox);
	mileageLabel->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
    QLabel* ownerLabel = new QLabel(QString::number(associatedCar.getClientPhoneNumber()), ui.carInfoGroupBox);
	ownerLabel->setStyleSheet("color:black; font-weight: bold;"); //Make it bold

    engineLabel->setWordWrap(true);


    //Add rows to form layout
	QLabel* makeLabelTitle = new QLabel(QLatin1String("Make:"), ui.carInfoGroupBox);
	makeLabelTitle->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
    carLayout->addRow(makeLabelTitle, makeLabel);
	QLabel* modelLabelTitle = new QLabel(QLatin1String("Model:"), ui.carInfoGroupBox);
	modelLabelTitle->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
	carLayout->addRow(modelLabelTitle, modelLabel);
	QLabel* yearLabelTitle = new QLabel(QLatin1String("Year:"), ui.carInfoGroupBox);
	yearLabelTitle->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
	carLayout->addRow(yearLabelTitle, yearLabel);
	QLabel* engineLabelTitle = new QLabel(QLatin1String("Engine:"), ui.carInfoGroupBox);
	engineLabelTitle->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
	carLayout->addRow(engineLabelTitle, engineLabel);
	QLabel* mileageLabelTitle = new QLabel(QLatin1String("Mileage:"), ui.carInfoGroupBox);
	mileageLabelTitle->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
	carLayout->addRow(mileageLabelTitle, mileageLabel);
	QLabel* ownerLabelTitle = new QLabel(QLatin1String("Owner Contact:"), ui.carInfoGroupBox);
	ownerLabelTitle->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
	carLayout->addRow(ownerLabelTitle, ownerLabel);

    carLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    ui.carInfoGroupBox->setLayout(carLayout); //Set the new layout


    //Populate Task Info
    ui.taskInfoGroupBox->setTitle(QString("Task Info (ID: %1)").arg(selectedTask.getTaskID()));
    //Clear previous widgets and layout
    qDeleteAll(ui.taskInfoGroupBox->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
    if (ui.taskInfoGroupBox->layout()) delete ui.taskInfoGroupBox->layout();
    QFormLayout* taskLayout = new QFormLayout();

    QLabel* instructionsDisplay = new QLabel(QString::fromStdString(selectedTask.getTaskInstructions()), ui.taskInfoGroupBox);
	instructionsDisplay->setStyleSheet("color:black; font-weight: bold;"); //Make it bold


    instructionsDisplay->setWordWrap(true); //Enable word wrap
    instructionsDisplay->setAlignment(Qt::AlignTop); //Align text to top

    QLabel* partsDisplay = new QLabel(ui.taskInfoGroupBox);
	partsDisplay->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
    partsDisplay->setWordWrap(true); //Enable word wrap
    partsDisplay->setAlignment(Qt::AlignTop); //Align text to top
    QString partsText = QString::fromStdString(selectedTask.getPartsNeeded());
    if (partsText.isEmpty()) {
        partsDisplay->setText(QLatin1String("<i>None specified</i>"));
    }
    else {
        partsDisplay->setText(partsText);
    }

	QLabel* instructionsLabel = new QLabel(QLatin1String("Instructions:"), ui.taskInfoGroupBox);
	instructionsLabel->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
	taskLayout->addRow(instructionsLabel, instructionsDisplay);
	QLabel* partsLabel = new QLabel(QLatin1String("Parts Needed:"), ui.taskInfoGroupBox);
	partsLabel->setStyleSheet("color:black; font-weight: bold;"); //Make it bold
	taskLayout->addRow(partsLabel, partsDisplay);

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

	QObject::connect(ui.markCompleteButton, &QPushButton::clicked, this, [this]() {
        if (currentWorkerTaskIndex >= 0 && currentWorkerTaskIndex < static_cast<int>(tasks.size())) {
            tasks[currentWorkerTaskIndex].setTaskStatus(COMPLETED); //Set task status to COMPLETED
            updateWorkerDashboard(); //Refresh the dashboard to reflect changes
			updateManagerTables(); //Refresh manager view as well
        }
		});
}

void AutoMagik::showCarInfo()
{
    if (currentWorkerTaskIndex < 0 || currentWorkerTaskIndex >= static_cast<int>(tasks.size()))
    {
        QMessageBox::warning(this, QLatin1String("No Task Selected"), QLatin1String("Please select a task from the list first."));
        return;
    }
    const Task& selectedTask = tasks[currentWorkerTaskIndex];
    const Car& associatedCar = selectedTask.getCarObject();
    //Store details for use when replies come back
    lastMakeForDialog = QString::fromStdString(associatedCar.getMake());
    lastModelForDialog = QString::fromStdString(associatedCar.getModel());
    lastYearForDialog = associatedCar.getProductionYear();
    //Clean model for API calls (basic split)
    QString modelForApi = lastModelForDialog.split(' ').first();
    ui.carInfoButton->setEnabled(false);
    ui.carInfoButton->setText(QLatin1String("Loading Web View..."));
	this->displayCarInfoDialog(); //Show dialog immediately
}

//DISPLAYING A WEBVIEW DIALOG HERE WITH THE CAR PARTS
void AutoMagik::displayCarInfoDialog()
{
	//Creating a web profile ith no persistent cookies
    QWebEngineProfile* profile = new QWebEngineProfile();
	profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    
	//Creating a new dialog
    QDialog dialog(this);
	dialog.setWindowTitle(QLatin1String("Find Car Parts"));
	dialog.setMinimumWidth(800);
    dialog.setMinimumHeight(600);

    //Creating the web view
	QWebEngineView* webView = new QWebEngineView(&dialog);
	
    //Creating a web page
    QWebEnginePage* page = new QWebEnginePage(profile, webView);
    webView->setPage(page);
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
	mainLayout->addWidget(webView); //Adding the web view to the layout

    dialog.setLayout(mainLayout); //Setting the layout to the dialog
    //Creating the URL for the web page
    QString url = QString("https://www.autodoc.pl");
    
    //Loading the URL in the web view
    webView->setUrl(QUrl(url));
	webView->show(); //Showing the web view

    dialog.exec(); //Executing the dialog
	ui.carInfoButton->setEnabled(true); //Re-enable the button after dialog is closed
	ui.carInfoButton->setText(QLatin1String("Browse Car Parts")); //Reset button text
}

//Function for adding tasks
void AutoMagik::editSelectedTask() {
    int rowIndex = ui.tasksTableWidget->currentRow();
    if (rowIndex < 0 || rowIndex >= static_cast<int>(tasks.size())) {
        QMessageBox::warning(this, "No Task Selected", "Please select a task from the list first.");
        return;
    }

    Task& selectedTask = tasks[rowIndex];
    QString taskId = QString::fromStdString(selectedTask.getFirebaseKey());

    // Create dialog
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Task");
    dialog.setMinimumWidth(450);
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    // Car selection combo box
    QComboBox* carSelection = new QComboBox(&dialog);
    int currentCarIndex = -1;
    for (size_t i = 0; i < cars.size(); ++i) {
        const auto& car = cars[i];
        QString carText = QString::fromStdString(car.getMake() + " " + car.getModel() + " (" + std::to_string(car.getProductionYear()) + ")");
        carSelection->addItem(carText, static_cast<int>(i));

        if (car == selectedTask.getCarObject()) {
            currentCarIndex = static_cast<int>(i);
        }
    }
    if (currentCarIndex >= 0) carSelection->setCurrentIndex(currentCarIndex);

    // Priority combo box
    QComboBox* priorityComboBox = new QComboBox(&dialog);
    priorityComboBox->addItem("Low", LOW);
    priorityComboBox->addItem("Medium", MEDIUM);
    priorityComboBox->addItem("High", HIGH);
    priorityComboBox->setCurrentText(QString::fromStdString(selectedTask.getTaskPriority()));

    // Text fields
    QTextEdit* instructionsInput = new QTextEdit(&dialog);
    instructionsInput->setText(QString::fromStdString(selectedTask.getTaskInstructions()));
    instructionsInput->setMinimumHeight(80);

    QTextEdit* partsInput = new QTextEdit(&dialog);
    partsInput->setPlaceholderText("Enter parts needed (optional)");
    partsInput->setText(QString::fromStdString(selectedTask.getPartsNeeded()));
    partsInput->setMinimumHeight(60);

    QTextEdit* initialCommentsInput = new QTextEdit(&dialog);
    initialCommentsInput->setPlaceholderText("Enter initial comments (optional)");
    initialCommentsInput->setText(QString::fromStdString(selectedTask.getComments()));
    initialCommentsInput->setMinimumHeight(60);

    // Layout
    QFormLayout* form = new QFormLayout();
    form->addRow("Select Priority:", priorityComboBox);
    form->addRow("Select Car:", carSelection);
    mainLayout->addLayout(form);

    mainLayout->addWidget(new QLabel("Instructions:", &dialog));
    mainLayout->addWidget(instructionsInput);
    mainLayout->addWidget(new QLabel("Parts Needed:", &dialog));
    mainLayout->addWidget(partsInput);
    mainLayout->addWidget(new QLabel("Initial Comments:", &dialog));
    mainLayout->addWidget(initialCommentsInput);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);

    // Dialog logic
    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        if (instructionsInput->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Input Error", "Task Instructions cannot be empty.");
        }
        else {
            dialog.accept();
        }
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Finalize changes
    if (dialog.exec() == QDialog::Accepted) {
        // Update local task
        selectedTask.setTaskInstructions(instructionsInput->toPlainText().trimmed().toStdString());
        selectedTask.setPartsNeeded(partsInput->toPlainText().trimmed().toStdString());
        selectedTask.setComments(initialCommentsInput->toPlainText().trimmed().toStdString());
        selectedTask.setTaskPriority(static_cast<priority>(priorityComboBox->currentData().toInt()));

        int selectedCarDataIndex = carSelection->currentData().toInt();
        if (selectedCarDataIndex >= 0 && selectedCarDataIndex < static_cast<int>(cars.size())) {
            selectedTask.setTaskCar(cars[selectedCarDataIndex]);
        }
        else {
            QMessageBox::critical(this, "Error", "Invalid car selection index.");
            return;
        }

        // Update Firebase
        firebase.updateTaskInDatabase(
            taskId,
            QString::fromStdString(selectedTask.getCarObject().getMake()),
            QString::fromStdString(selectedTask.getCarObject().getModel()),
            instructionsInput->toPlainText().trimmed(),
            partsInput->toPlainText().trimmed(),
            initialCommentsInput->toPlainText().trimmed(),
            selectedTask.getTaskWorkerID(),
            QString::fromStdString(selectedTask.getTaskStatus()),
            priorityComboBox->currentText(),
            firebase.getIdToken()
        );

        updateManagerTables();
        updateWorkerDashboard();
    }
}

//Function for assigning/reassigning tasks to workers
void AutoMagik::assignReassignTask()
{
    int selectedTaskIndex = ui.tasksTableWidget->currentRow();
    if (selectedTaskIndex < 0 || selectedTaskIndex >= static_cast<int>(tasks.size()))
    {
        QMessageBox::warning(this, "No Task Selected", "Please select a task from the list first.");
        return;
    }
    Task& selectedTask = tasks[selectedTaskIndex];
    QString taskId = QString::fromStdString(selectedTask.getFirebaseKey());
    int currentWorkerID = selectedTask.getTaskWorkerID();

    if (taskId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selected task has no Firebase ID");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Assign/Reassign Task");
    dialog.setMinimumWidth(450);
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    // Worker selection combo box
    QComboBox* workerComboBox = new QComboBox(&dialog);
    workerComboBox->addItem("Unassigned", 0); // Option to unassign task

    // Add all workers to the combo box
    for (const auto& worker : workers)
    {
        workerComboBox->addItem(
            QString::fromStdString(worker.getWorkerName() + " (" + worker.getPosition() + ")"),
            worker.getWorkerID()
        );
    }

    // Set current selection
    if (currentWorkerID != 0)
    {
        int index = workerComboBox->findData(currentWorkerID);
        if (index != -1) workerComboBox->setCurrentIndex(index);
    }

    QFormLayout* form = new QFormLayout();
    form->addRow("Select Worker:", workerComboBox);
    mainLayout->addLayout(form);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    mainLayout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        int selectedWorkerID = workerComboBox->currentData().toInt();
        // If selection didn't change, do nothing
        if (selectedWorkerID == currentWorkerID) {
            return;
        }
       // Get worker name for database update
        QString workerName;
        if (selectedWorkerID != 0) {
            for (const auto& worker : workers) {
                if (worker.getWorkerID() == selectedWorkerID) {
                    workerName = QString::fromStdString(worker.getWorkerName());
                    break;
                }
            }
        }
        // Update local task
        selectedTask.setTaskWorkerID(selectedWorkerID);

        // Update local workers
        if (currentWorkerID != 0) {
            for (auto& worker : workers) {
                if (worker.getWorkerID() == currentWorkerID) {
                    worker.clearTask();
                    break;
                }
            }
        }

        if (selectedWorkerID != 0) {
            for (auto& worker : workers) {
                if (worker.getWorkerID() == selectedWorkerID) {
                    worker.assignTask(selectedTask);
                    break;
                }
            }
        }

        // Connect to Firebase signal before updating
        connect(&firebase, &Firebase::taskUpdated, this, [this]() {
            updateManagerTables();
            updateWorkerDashboard();
            });

        // Update task in Firebase
        firebase.updateTaskInDatabase(
            taskId,
            QString::fromStdString(selectedTask.getCarObject().getMake()),
            QString::fromStdString(selectedTask.getCarObject().getModel()),
            QString::fromStdString(selectedTask.getTaskInstructions()),
            QString::fromStdString(selectedTask.getPartsNeeded()),
            QString::fromStdString(selectedTask.getComments()),
            selectedWorkerID,
            QString::fromStdString(selectedTask.getTaskStatus()),
            QString::fromStdString(selectedTask.getTaskPriority()),
            firebase.getIdToken(),
            workerName
        );
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
    Task& selectedTask = tasks[selectedTaskIndex];
    QString taskId = QString::fromStdString(selectedTask.getFirebaseKey());

    if (taskId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selected task has no Firebase ID");
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
        //delete task from firebase
        firebase.deleteTaskFromDatabase(taskId, firebase.getIdToken());
        connect(&firebase, &Firebase::taskDeleted, this, [this, selectedTaskIndex](const QString& deletedTaskId) {
            // cheching if key and id is valid
            if (selectedTaskIndex < tasks.size()) {
                if (QString::fromStdString(tasks[selectedTaskIndex].getFirebaseKey()) == deletedTaskId) {
                    // delete task from vector
                    tasks.erase(tasks.begin() + selectedTaskIndex);
                    // delete from row
                    ui.tasksTableWidget->removeRow(selectedTaskIndex);
                    // update interfejs
                    updateManagerTables();
                    updateWorkerDashboard();
                }
            }
            });
    }
}


//Function for editing cars
void AutoMagik::editSelectedCar()
{
    int selectedCarIndex = ui.carsTableWidget->currentRow();
    if (selectedCarIndex < 0 || selectedCarIndex >= static_cast<int>(cars.size())) {
        QMessageBox::warning(this, QLatin1String("No Car Selected"), QLatin1String("Please select a car from the list first."));
        return;
    }

    Car& selectedCar = cars[selectedCarIndex];
    QString carId = QString::fromStdString(selectedCar.getFirebaseKey());

    if (carId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selected car has no Firebase ID");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QLatin1String("Edit Car"));
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    // Input fields
    QLineEdit* makeInput = new QLineEdit(&dialog);
    makeInput->setText(QString::fromStdString(selectedCar.getMake()));

    QLineEdit* modelInput = new QLineEdit(&dialog);
    modelInput->setText(QString::fromStdString(selectedCar.getModel()));

    QLineEdit* engineInput = new QLineEdit(&dialog);
    engineInput->setText(QString::fromStdString(selectedCar.getEngineType()));

    QComboBox* yearInput = new QComboBox(&dialog);
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear; i >= 1900; i--) {
        yearInput->addItem(QString::number(i));
    }
    yearInput->setCurrentText(QString::number(selectedCar.getProductionYear()));

    QSpinBox* mileageInput = new QSpinBox(&dialog);
    mileageInput->setSuffix(QLatin1String(" km"));
    mileageInput->setRange(0, 3000000);
    mileageInput->setValue(selectedCar.getCarMileage());

    QLineEdit* phoneNumberInput = new QLineEdit(&dialog);
    phoneNumberInput->setText(QString::number(selectedCar.getClientPhoneNumber()));

    // Phone number validation
    QRegularExpression phoneRegex(QStringLiteral("^\\+?[\\d\\s]+$"));
    phoneNumberInput->setValidator(new QRegularExpressionValidator(phoneRegex, phoneNumberInput));

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(QLatin1String("Make:"), makeInput);
    formLayout->addRow(QLatin1String("Model:"), modelInput);
    formLayout->addRow(QLatin1String("Year:"), yearInput);
    formLayout->addRow(QLatin1String("Engine:"), engineInput);
    formLayout->addRow(QLatin1String("Mileage:"), mileageInput);
    formLayout->addRow(QLatin1String("Owner Phone:"), phoneNumberInput);

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

        if (phoneText.isEmpty()) {
            QMessageBox::warning(&dialog, QLatin1String("Input Error"), QLatin1String("Owner Phone number cannot be empty."));
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

        dialog.accept();
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() == QDialog::Accepted) {
        // Update local car object
        selectedCar.setCarMake(makeInput->text().trimmed().toStdString());
        selectedCar.setCarModel(modelInput->text().trimmed().toStdString());
        selectedCar.setEngineType(engineInput->text().trimmed().toStdString());
        selectedCar.setProductionYear(yearInput->currentText().toInt());
        selectedCar.setCarMileage(mileageInput->value());

        QString phoneCleaned = phoneNumberInput->text().trimmed();
        phoneCleaned.remove(' ').remove('+');
        selectedCar.setClientPhoneNumber(phoneCleaned.toInt());
        // Update Firebase
        firebase.updateCarInDatabase(
            carId,
            makeInput->text().trimmed(),
            modelInput->text().trimmed(),
            engineInput->text().trimmed(),
            yearInput->currentText().toInt(),
            mileageInput->value(),
            phoneCleaned.toInt(),
            firebase.getIdToken()
        );

        updateManagerTables();
    }
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
        // Get the Firebase key for the car (you'll need to store it when adding the car)
        QString carId = QString::fromStdString(cars[selectedCarIndex].getFirebaseKey());

        if (carId.isEmpty()) {
            QMessageBox::warning(this, "Error", "Selected car has no Firebase ID");
            return;
        }

        // Connect to the carDeleted signal before sending the delete request
        connect(&firebase, &Firebase::carDeleted, this, [this, selectedCarIndex](const QString& deletedCarId) {
            // Verify the deleted car matches our selection
            if (selectedCarIndex < cars.size() &&
                QString::fromStdString(cars[selectedCarIndex].getFirebaseKey()) == deletedCarId) {
                // Remove from local vector
                cars.erase(cars.begin() + selectedCarIndex);
                // Remove from table
                ui.carsTableWidget->removeRow(selectedCarIndex);
                // Update UI
                updateManagerTables();
            }
            });

        // Delete from Firebase
        firebase.deleteCarFromDatabase(carId, firebase.getIdToken());
    }
}
//Function for editing workers
void AutoMagik::editSelectedWorker()
{
    int selectedWorkerIndex = ui.workersTableWidget->currentRow();
    if (selectedWorkerIndex < 0 || selectedWorkerIndex >= static_cast<int>(workers.size()))
    {
        QMessageBox::warning(this, "No Worker Selected", "Please select a worker from the list first.");
        return;
    }

    Worker& selectedWorker = workers[selectedWorkerIndex];
    QString workerId = QString::fromStdString(selectedWorker.getFirebaseKey());

    if (workerId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selected worker has no Firebase ID");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Edit Worker");
    dialog.setMinimumWidth(400);

    QFormLayout* formLayout = new QFormLayout(&dialog);

    QLineEdit* nameInput = new QLineEdit(&dialog);
    nameInput->setText(QString::fromStdString(selectedWorker.getWorkerName()));

    QLineEdit* positionInput = new QLineEdit(&dialog);
    positionInput->setText(QString::fromStdString(selectedWorker.getPosition()));

    QSpinBox* experienceInput = new QSpinBox(&dialog);
    experienceInput->setSuffix(" years");
    experienceInput->setRange(0, 60);
    experienceInput->setValue(selectedWorker.getWorkerExperience());

    QSpinBox* salaryInput = new QSpinBox(&dialog);
    salaryInput->setPrefix("$ ");
    salaryInput->setRange(0, 500000);
    salaryInput->setSingleStep(1000);
    salaryInput->setValue(selectedWorker.getWorkerSalary());

    QSpinBox* ageInput = new QSpinBox(&dialog);
    ageInput->setRange(18, 100);
    ageInput->setValue(selectedWorker.getWorkerAge());

    formLayout->addRow(new QLabel("Worker Name:"), nameInput);
    formLayout->addRow(new QLabel("Position:"), positionInput);
    formLayout->addRow(new QLabel("Experience:"), experienceInput);
    formLayout->addRow(new QLabel("Salary:"), salaryInput);
    formLayout->addRow(new QLabel("Age:"), ageInput);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout->addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
        if (nameInput->text().trimmed().isEmpty() || positionInput->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Input Error", "Worker Name and Position cannot be empty.");
        }
        else {
            dialog.accept();
        }
        });
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        // Update local worker object
        selectedWorker.setWorkerName(nameInput->text().trimmed().toStdString());
        selectedWorker.setPosition(positionInput->text().trimmed().toStdString());
        selectedWorker.setWorkerExperience(experienceInput->value());
        selectedWorker.setWorkerSalary(salaryInput->value());
        selectedWorker.setWorkerAge(ageInput->value());

        // Prepare data for Firebase
        QVariantMap workerData;
        workerData["w_id"] = selectedWorker.getWorkerID();
        workerData["name"] = nameInput->text().trimmed();
        workerData["position"] = positionInput->text().trimmed();
        workerData["experience"] = experienceInput->value();
        workerData["salary"] = salaryInput->value();
        workerData["age"] = ageInput->value();
        workerData["email"] = QString::fromStdString(selectedWorker.getEmail()); 
        workerData["manager"] = firebase.m_uid;
        workerData["role"] = "worker"; 

        // Connect to the workerUpdated signal before updating
        connect(&firebase, &Firebase::workerUpdated, this, [this]() {
            updateManagerTables();
            });

        // Update worker in Firebase
        firebase.updateWorkerInDatabase(
            workerId,
            workerData,
            firebase.getIdToken()
        );
        updateManagerTables();
    }
}
//Function for deleting workers
void AutoMagik::deleteSelectedWorker()
{
    int selectedWorkerIndex = ui.workersTableWidget->currentRow();
    if (selectedWorkerIndex < 0 || selectedWorkerIndex >= static_cast<int>(workers.size()))
    {
        QMessageBox::warning(this, "No Worker Selected", "Please select a worker from the list first.");
        return;
    }

    Worker& selectedWorker = workers[selectedWorkerIndex];
    QString workerId = QString::fromStdString(selectedWorker.getFirebaseKey());
    QString workerEmail = QString::fromStdString(selectedWorker.getEmail());

    if (workerId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selected worker has no Firebase ID");
        return;
    }

    // Check if worker has assigned tasks
    bool hasTasks = false;
    for (const auto& task : tasks) {
        if (task.getTaskWorkerID() == selectedWorker.getWorkerID()) {
            hasTasks = true;
            break;
        }
    }

    if (hasTasks) {
        QMessageBox::warning(this, "Cannot Delete", "Worker has assigned tasks. Reassign or delete tasks first.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Delete Worker");
    dialog.setMinimumWidth(400);
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    QLabel* label = new QLabel("Are you sure you want to delete this worker?", &dialog);
    layout->addWidget(label);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout->addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        // Connect signals before deletion
        connect(&firebase, &Firebase::workerDeleted, this, [this, selectedWorkerIndex]() {
            // Remove from local vector
            workers.erase(workers.begin() + selectedWorkerIndex);
            // Update UI
            updateManagerTables();
        });

        connect(&firebase, &Firebase::workerAuthDeleted, this, []() {
            qDebug() << "Worker authentication deleted successfully";
        });

        // First delete from authentication
        firebase.deleteWorkerFromAuthentication(workerEmail);
        
        // Then delete from database
        firebase.deleteWorkerFromDatabase(workerId, firebase.getIdToken());
    }
}

// Add this method to AutoMagik.cpp
void AutoMagik::addCommentToTask()
{
    if (currentWorkerTaskIndex < 0 || currentWorkerTaskIndex >= static_cast<int>(tasks.size())) {
        QMessageBox::warning(this, "No Task Selected", "Please select a task first.");
        return;
    }

    QString newComment = ui.newCommentLineEdit->text().trimmed();
    if (newComment.isEmpty()) {
        QMessageBox::warning(this, "Empty Comment", "Please enter a comment before adding.");
        return;
    }

    Task& selectedTask = tasks[currentWorkerTaskIndex];

    // Append new comment to existing comments
    QString currentComments = QString::fromStdString(selectedTask.getComments());
    if (!currentComments.isEmpty()) {
        currentComments += "\n\n"; // Add separation between comments
    }
    currentComments += newComment;

    // Update local task
    selectedTask.setComments(currentComments.toStdString());

    // Update UI
    ui.commentsTextEdit->setPlainText(currentComments);
    ui.newCommentLineEdit->clear();

    // Update database
    QString taskId = QString::fromStdString(selectedTask.getFirebaseKey());
    if (taskId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Task has no Firebase ID");
        return;
    }

    // Connect to Firebase signal before updating
    connect(&firebase, &Firebase::taskUpdated, this, [this]() {
        QMessageBox::information(this, "Success", "Comment added successfully!");
        });

    // Update task in Firebase
    firebase.updateTaskInDatabase(
        taskId,
        QString::fromStdString(selectedTask.getCarObject().getMake()),
        QString::fromStdString(selectedTask.getCarObject().getModel()),
        QString::fromStdString(selectedTask.getTaskInstructions()),
        QString::fromStdString(selectedTask.getPartsNeeded()),
        currentComments,
        selectedTask.getTaskWorkerID(),
        QString::fromStdString(selectedTask.getTaskStatus()),
        QString::fromStdString(selectedTask.getTaskPriority()),
        firebase.getIdToken()
    );
}