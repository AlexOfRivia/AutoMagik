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
    explicit Firebase(QObject *parent = nullptr);  // Constructor with optional parent
    ~Firebase();  // Destructor
    void addClient(const QString &clientName,const QString &clientSurname,const QString &email,const QString &workerName);
    void setAPIKey(const QString &apiKey);  // Method to set the Firebase Web API key
    void signUserIn(const QString &emailAddress, const QString &password);  // Sign in user with email and password
    QString m_uid;

signals:
    void userSignedIn();  // Signal emitted when the user signs in successfully

public slots:
    void networkReplyReadyRead();  // Slot called when a network reply has data ready
    void performAuthenticatedDatabaseCall();  // Slot to perform an authenticated GET request to Firebase Realtime DB

private:
    void performPOST(const QString &url, const QJsonDocument &payload);  // Internal method to perform a POST request
    void parseResponse(const QByteArray &response);  // Internal method to parse JSON response

    QString m_apiKey;  // Stores Firebase Web API key
    QString m_idToken;  // Stores user ID token after sign-in
    QNetworkAccessManager *m_networkAccessManager;  // Network manager used to send requests
    QNetworkReply *m_networkReply;  // Stores the current network reply
};

#endif
