#include "Firebase.h"
#include <QDebug>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QJsonObject>


// Constructor for the Firebase class
Firebase::Firebase(QObject* parent)
    : QObject{ parent }
    , m_apiKey(QString())
{
    m_networkAccessManager = new QNetworkAccessManager(this);

    // Connect signals to automatically perform a database call when a user signs in
    connect(this, &Firebase::workerSignedIn, this, &Firebase::performAuthenticatedDatabaseCall);
    connect(this, &Firebase::managerSignedIn, this, &Firebase::performAuthenticatedDatabaseCall);
}

// Destructor for the Firebase class
Firebase::~Firebase()
{
    // Clean up the network manager
    m_networkAccessManager->deleteLater();
}

// Store the Firebase API key
void Firebase::setAPIKey(const QString& apiKey)
{
    m_apiKey = apiKey;
}

// Sign in a user using email and password
void Firebase::signUserIn(const QString& emailAddress, const QString& password)
{
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)"); //checking if email address is in right format 
    QRegularExpressionMatch match = emailRegex.match(emailAddress);

    if (!match.hasMatch()) {
        qDebug() << "Invalid email format. Write a proper email";
        return;
    }

    QString signInEndpoint = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=" + m_apiKey;

    QVariantMap variantPayload;
    variantPayload["email"] = emailAddress;
    variantPayload["password"] = password;
    variantPayload["returnSecureToken"] = true;

    QJsonDocument jsonPayload = QJsonDocument::fromVariant(variantPayload);
    performPOST(signInEndpoint, jsonPayload);
}

//Function for creating manager
void Firebase::addManagerAccount(const QString& email, const QString& password) {
    QString signUpEndpoint = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=" + m_apiKey;

    QVariantMap payload;
    payload["email"] = email;
    payload["password"] = password;
    payload["returnSecureToken"] = true;

    QNetworkRequest request((QUrl(signUpEndpoint)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkAccessManager->post(request, QJsonDocument::fromVariant(payload).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply, email, password]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        QJsonDocument json = QJsonDocument::fromJson(response);

        if (json.object().contains("error")) {
            QString errorMsg = json.object().value("error").toObject().value("message").toString();
            emit registrationFailed(errorMsg);
            return;
        }

        QString uid = json.object().value("localId").toString();
        QString idToken = json.object().value("idToken").toString();

        QString dbUrl = "https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/managers/" + uid + ".json?auth=" + idToken;

        QVariantMap managerData;
        managerData["email"] = email;
        managerData["password"] = password; //updated to include password in our db
        managerData["role"] = "manager";

        QNetworkRequest dbRequest((QUrl(dbUrl)));
        dbRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply* dbReply = m_networkAccessManager->put(dbRequest, QJsonDocument::fromVariant(managerData).toJson());

        connect(dbReply, &QNetworkReply::finished, [this, dbReply]() {
            QByteArray dbResponse = dbReply->readAll();
            dbReply->deleteLater();

            if (QJsonDocument::fromJson(dbResponse).object().contains("error")) {
                emit registrationFailed("Failed to save manager data");
            }
            else {
                emit managerAccountCreated(); 
            }
            });
        });
}


void Firebase::addWorkerAccount(const QString& email, const QVariantMap& workerData, const QString& password) {
    QString signUpEndpoint = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=" + m_apiKey;

    QVariantMap payload;
    payload["email"] = email;
    payload["password"] = password;
    payload["returnSecureToken"] = true;

    QNetworkRequest request((QUrl(signUpEndpoint)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkAccessManager->post(request, QJsonDocument::fromVariant(payload).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply, email, password, workerData]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        QJsonDocument json = QJsonDocument::fromJson(response);

        if (json.object().contains("error")) {
            QString errorMsg = json.object().value("error").toObject().value("message").toString();
            emit registrationFailed(errorMsg);
            return;
        }

        QString uid = json.object().value("localId").toString();
        QString idToken = json.object().value("idToken").toString();

        QString dbUrl = "https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/workers/" + uid + ".json?auth=" + idToken;

        QVariantMap localWorkerData = workerData;
        localWorkerData["email"] = email;
        localWorkerData["password"] = password;
        localWorkerData["role"] = "worker";

        QNetworkRequest dbRequest((QUrl(dbUrl)));
        dbRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply* dbReply = m_networkAccessManager->put(dbRequest, QJsonDocument::fromVariant(localWorkerData).toJson());

        connect(dbReply, &QNetworkReply::finished, [this, dbReply]() {
            QByteArray dbResponse = dbReply->readAll();
            dbReply->deleteLater();

            if (QJsonDocument::fromJson(dbResponse).object().contains("error")) {
                emit registrationFailed("Failed to save worker data");
            }
            else {
                emit workerAccountCreated(); //might be useful for smth
            }
            });
        });
}



// Helper function to send a POST request with JSON data
void Firebase::performPOST(const QString& url, const QJsonDocument& payload)
{
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    // Send POST request
    m_networkReply = m_networkAccessManager->post(request, payload.toJson());

    // Connect to handle the response when it's ready
    connect(m_networkReply, &QNetworkReply::readyRead, this, &Firebase::networkReplyReadyRead);
}

// Called when the reply from the POST request is ready
void Firebase::networkReplyReadyRead()
{
    // Read all data from the response
    QByteArray response = m_networkReply->readAll();
    qDebug() << response;

    // Clean up
    m_networkReply->deleteLater();

    // Parse the JSON response
    parseResponse(response);
}

// Parse the JSON login response and determine user role (manager or worker)
void Firebase::parseResponse(const QByteArray& response) {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(response);

    // If there is mistake in email/password
    if (jsonDocument.object().contains("error")) {
        QString errorMsg = jsonDocument.object().value("error").toObject().value("message").toString();

        if (errorMsg.contains("INVALID_EMAIL") || errorMsg.contains("EMAIL_NOT_FOUND")) {
            emit loginFailed("Invalid login");
        }
        else if (errorMsg.contains("INVALID_LOGIN_CREDENTIALS")) {
            emit loginFailed("INVALID EMAIL OR PASSWORD");
        }
        else if (errorMsg.contains("INVALID_PASSWORD")) {
            emit loginFailed("Invalid password");
        }
        else {
            emit loginFailed("Login failed: " + errorMsg);
        }
        return;
    }

    // Succesfull login - continue to check password
    m_idToken = jsonDocument.object().value("idToken").toString();
    m_uid = jsonDocument.object().value("localId").toString();

    // Cheching if user is a manager
    QString managerUrl = QString("https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/managers/%1.json?auth=%2")
        .arg(m_uid)
        .arg(m_idToken);

    QNetworkReply* managerReply = m_networkAccessManager->get(QNetworkRequest(QUrl(managerUrl)));

    connect(managerReply, &QNetworkReply::finished, [this, managerReply]() {
        QByteArray managerResponse = managerReply->readAll();
        managerReply->deleteLater();

        QJsonDocument managerDoc = QJsonDocument::fromJson(managerResponse);

        if (managerDoc.isObject() && !managerDoc.object().isEmpty()) {
            //User is the manager
            QJsonObject data = managerDoc.object();
            QString role = data.value("role").toString();

            if (role == "manager") {
                QString name = data.value("name").toString();
                emit managerSignedIn(name);
            }
            else {
                emit loginFailed("Access denied:Unproperly role of user");
            }
        }
        else {
            //If user isnt a manager we check if he is a worker
            QString workerUrl = QString("https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/workers/%1.json?auth=%2")
                .arg(m_uid)
                .arg(m_idToken);
            QNetworkReply* workerReply = m_networkAccessManager->get(QNetworkRequest(QUrl(workerUrl)));
            connect(workerReply, &QNetworkReply::finished, [this, workerReply]() {
                QByteArray workerResponse = workerReply->readAll();
                workerReply->deleteLater();

                QJsonDocument workerDoc = QJsonDocument::fromJson(workerResponse);

                if (workerDoc.isObject() && !workerDoc.object().isEmpty()) {
                    // User is a worker
                    QJsonObject data = workerDoc.object();
                    QString role = data.value("role").toString();

                    if (role == "worker") {
                        QString name = data.value("name").toString();
                        emit workerSignedIn(name);
                    }
                    else {
                        emit loginFailed("Acces denied:Unproperly role of user");
                    }
                }
                else {
                    // User is not a manager/worker he is not in database
                    emit loginFailed("Access denied: User not found in database");
                }
                });
        }
        });
}
//Function for adding car to database
void Firebase::addCarToDatabase(const QString& make, const QString& model, const QString& engineType,
    int year, int mileage, int phoneNumber, const QString& idToken) {
    QString dbUrl = "https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/cars.json?auth=" + idToken;

    QVariantMap carData;
    carData["make"] = make;
    carData["model"] = model;
    carData["engineType"] = engineType;
    carData["year"] = year;
    carData["mileage"] = mileage;
    carData["phoneNumber"] = phoneNumber;

    QNetworkRequest request((QUrl(dbUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkAccessManager->post(request, QJsonDocument::fromVariant(carData).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        if (QJsonDocument::fromJson(response).object().contains("error")) {
            qDebug() << "Failed to save car data";
        }
        else {
            qDebug() << "Car added successfully";
        }
        });
}
//Function for adding workers to database
void Firebase::addNewWorkerToDatabase(const QString& workerName, const QString& position, const int experience, const int salary, const int age, const QString& idToken)
{
    QString dbUrl = "https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/automagik/workers.json?auth=" + idToken;
    QVariantMap workerData;
    workerData["name"] = workerName;
    workerData["position"] = position;
    workerData["experience"] = experience;
    workerData["salary"] = salary;
    workerData["age"] = age;
    QNetworkRequest request((QUrl(dbUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkAccessManager->post(request, QJsonDocument::fromVariant(workerData).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        if (QJsonDocument::fromJson(response).object().contains("error")) {
            qDebug() << "Failed to save worker data";
        }
        else {
            qDebug() << "Worker added successfully";
        }
        });
}


// Called after login to fetch additional user info from the database
void Firebase::performAuthenticatedDatabaseCall()
{
    // Example: fetch user's name from the Realtime Database
    QString endPoint = "https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/Workers/Employee/" + m_uid + "/name.json?auth=" + m_idToken;

    // Send GET request to database
    m_networkReply = m_networkAccessManager->get(QNetworkRequest(QUrl(endPoint)));
}



