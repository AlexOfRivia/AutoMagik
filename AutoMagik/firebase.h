#ifndef FIREBASE_H  // Include guard to prevent multiple inclusion
#define FIREBASE_H

#include <QObject>  // Base class for all Qt objects
#include <QNetworkAccessManager>  // Handles network operations
#include <QNetworkReply>  // Represents the reply from a network request
#include <QJsonDocument>  // Represents a JSON document (used for parsing and creating JSON)

// Firebase class that handles authentication and database operations
class Firebase : public QObject
{
    Q_OBJECT  // Macro enabling Qt's meta-object features (signals/slots)
public:
    explicit Firebase(QObject* parent = nullptr);  // Constructor with optional parent
    ~Firebase();  // Destructor
    void setAPIKey(const QString& apiKey);  // Method to set the Firebase Web API key
    void signUserIn(const QString& emailAddress, const QString& password);  // Sign in user with email and password
    QString m_uid;
    void addManagerAccount(const QString& email, const QString& password); //Create account for manager
    void addCarToDatabase(const QString& make, const QString& model, const QString& engineType, int year, int mileage, int phoneNumber, const QString& idToken); //adding car to firebase
    void addNewWorkerToDatabase(const QString& workerName, const QString& position, const int experience, const int salary, const int age, const QString& idToken);//adding new worker to firebase
    QString getIdToken() const { return m_idToken; } // for getting token from firebase
signals:
    void workerSignedIn(const QString& workerName);  // Signal emitted when the workers sign in successfully
    void managerSignedIn(const QString& managerName); //Signal emittes when the managers sign in successfully
    void managerAccountCreated();  //Signal emitted when manager account was created successfully
    void loginFailed(const QString& errorMessage);  //signal emittes when the was an error during login
    void registrationFailed(const QString& errorMessage); //signal emittes when the was an error during registration
public slots:
    void networkReplyReadyRead();  // Slot called when a network reply has data ready
    void performAuthenticatedDatabaseCall();  // Slot to perform an authenticated GET request to Firebase Realtime DB

private:
    void performPOST(const QString& url, const QJsonDocument& payload);  // Internal method to perform a POST request
    void parseResponse(const QByteArray& response);  // Internal method to parse JSON response
    QString m_apiKey;  // Stores Firebase Web API key
    QString m_idToken;  // Stores user ID token after sign-in
    QNetworkAccessManager* m_networkAccessManager;  // Network manager used to send requests
    QNetworkReply* m_networkReply;  // Stores the current network reply
};

#endif
