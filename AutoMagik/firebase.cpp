#include "firebase.h"  // Include the corresponding header file
#include <QDebug>  // For logging and debugging
#include <QVariantMap>  // To store key-value data in a QVariantMap
#include <QNetworkRequest>  // To make network requests
#include <QJsonObject>  // To handle JSON objects

// Constructor for the Firebase class
Firebase::Firebase(QObject *parent)
    : QObject{parent}  // Call base QObject constructor
    , m_apiKey(QString())  // Initialize API key as empty string
{
    m_networkAccessManager = new QNetworkAccessManager(this);  // Initialize network access manager
    connect(this, &Firebase::userSignedIn, this, &Firebase::performAuthenticatedDatabaseCall);  // Connect signal to slot
}

// Destructor for Firebase
Firebase::~Firebase()
{
    m_networkAccessManager->deleteLater();  // Delete network manager later to avoid memory leaks
}

// Set the Firebase Web API key
void Firebase::setAPIKey(const QString &apiKey)
{
    m_apiKey = apiKey;  // Store the API key
}

// Function to sign in user with email and password
void Firebase::signUserIn(const QString &emailAddress, const QString &password)
{
    // Create the endpoint URL for signing in with password
    QString signInEndpoint = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=" + m_apiKey;//i create a sample database to log in

    QVariantMap variantPayload;  // Create a map for the JSON payload
    variantPayload["email"] = emailAddress;  // Add email to payload
    variantPayload["password"] = password;  // Add password to payload
    variantPayload["returnSecureToken"] = true;  // Ask Firebase to return an ID token

    QJsonDocument jsonPayload = QJsonDocument::fromVariant(variantPayload);  // Convert to JSON document
    performPOST(signInEndpoint, jsonPayload);  // Send POST request
}

// Function to send a POST request with JSON payload
void Firebase::performPOST(const QString &url, const QJsonDocument &payload)
{
    QNetworkRequest request{QUrl(url)};  // Create network request with URL
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));  // Set content type to JSON

    m_networkReply = m_networkAccessManager->post(request, payload.toJson());  // Send POST request and store reply
    connect(m_networkReply, &QNetworkReply::readyRead, this, &Firebase::networkReplyReadyRead);  // Connect to read handler
}

// Slot triggered when network reply has data ready to read
void Firebase::networkReplyReadyRead()
{
    QByteArray response = m_networkReply->readAll();  // Read all data from reply
    qDebug() << response;  // Print response for debugging
    m_networkReply->deleteLater();  // Schedule reply for deletion
    parseResponse(response);  // Parse the JSON response
}

// Function to parse response from Firebase
void Firebase::parseResponse(const QByteArray &response)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(response);  // Convert response to JSON

    // Check if the response contains an error
    if (jsonDocument.object().contains("error")) {
        qDebug() << "Error occurred:" << response;  // Print error message
    }
    // If the response contains a "kind" key, assume it's a success
    else if (jsonDocument.object().contains("kind")) {
        QString idToken = jsonDocument.object().value("idToken").toString();  // Extract ID token
        qDebug() << "User signed in successfully!";  // Log success
        qDebug()<<"User UID (localId)"<<m_uid;
        m_idToken = idToken;  // Store the ID token
        emit userSignedIn();  // Emit signal that user is signed in
    }
    // If response format is unexpected
    else {
        qDebug() << "Unexpected response:" << response;  // Log unrecognized response
    }
}


//we add client to our database
void Firebase::addClient(const QString &clientName, const QString &clientSurname, const QString &email,const QString &workerName)
{
    if (m_idToken.isEmpty()) {
        qWarning() << "You must be signed in to add a client.";
        return;
    }
    // Endpoint: clients.json – Firebase give a unique key for our clients
    QString endpoint = QString("https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/Workers/Employee/9MZvf5veBeafQ4ighRulBtNr8iI3.json?auth=%1").arg(m_idToken);
    QUrl url(endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Przygotowujemy dane klienta w formacie JSON
    QJsonObject clientObject;
    clientObject["firstName"] = clientName;
    clientObject["lastName"] = clientSurname;
    clientObject["email"] = email;
    clientObject["worker_name"] = workerName;
    clientObject["ownerUID"] = m_uid;  //we can filter our clients depending on workers
    QJsonDocument doc(clientObject);
    QByteArray payload = doc.toJson();

    // Send Post
    QNetworkReply *reply = m_networkAccessManager->post(request, payload);
    connect(reply, &QNetworkReply::finished, [reply]() {
        QByteArray response = reply->readAll();
        qDebug() << "Client added! Firebase response:" << response;
        reply->deleteLater();
    });
}
// Slot to perform a GET request to authenticated Firebase Realtime Database
void Firebase::performAuthenticatedDatabaseCall()
{
    // Construct the URL with the user's ID token for authentication
    QString endPoint = "https://automagik-96e43-default-rtdb.europe-west1.firebasedatabase.app/Workers/Employee/" + m_uid + "/name.json?auth=" +m_idToken; //in real-time database it get sample data
    m_networkReply = m_networkAccessManager->get(QNetworkRequest(QUrl(endPoint)));  // Send GET request
}
