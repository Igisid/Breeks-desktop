#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

#include "Back/secret-data.h"
#include "Front/datastructures.h"

namespace Network {
  class ServerConnection: public QObject
  {
    Q_OBJECT
  public:
    ServerConnection(QObject *parent = nullptr);
    ServerConnection(QNetworkAccessManager *, Network::UserData * , QObject *parent = nullptr);

    Network::UserData * getUserData();

    QString resolveAccessTokenFromRequest();
    QString resolveRefreshTokenFromRequest();

    // ====================================
    //  Auth requests below:
    // ====================================
    void sendAuthRequest(const QString&, const QString&);
    void sendPostRefreshRequest(const QString&, const QString&);

    // ====================================
    //  POST data to server methods below:
    // ====================================
    void sendPostRequest(const QUrl&, const QByteArray&);
    void sendPostRequestWithBearerToken(const QUrl&, const QByteArray&, const QString&); // !

    // ====================================
    //  PUT data to server methods below:
    // ====================================
    void sendPutRequestWithBearerToken(const QUrl&, const QByteArray&, const QString&); // !

    // ====================================
    //  DELETE data from server methods below:
    // ====================================
    void sendDeleteRequestWithBearerToken(const QUrl&, const QString&); // !

    // ====================================
    //  GET data from server methods below:
    // ====================================    
    void sendGetRequestWithBearerToken(const QUrl&, const QString&); // !

  private:
    QNetworkAccessManager * networkAccessManager_;
    Network::UserData * userData_;
    QList<lastRequest_t> listOfLastRequests_;
    bool mutex = false;

  public slots:
    void onfinish(QNetworkReply *);

  signals:
    void initSecretData(QString, QString, QString);
    void initTEidOnServer(long);
    void initBLidOnServer(long);
    void loginReply(bool);
    void sendBreeksLinesToGUI(const QList<breeksData_t>&);
    void sendTTElementsToGUI(const QList<elementData_t>&);
    void sendNoteToGUI(note_t&);
    void sendImageToGUI(const image_t&);

    // this signal is emitted whenever we want to (re-)load all data of the week
    void initWeekData(const QString&);

    void logout();

    void sendDataToRfrshFile(const QString&, const QString&);
  };


// ===================
//  API Properties
//====================
const QString serverUrl = QStringLiteral("https://breeks.cf");
const QString registrationUrl = QStringLiteral("/user/registration");
const QString authUrl = QStringLiteral("/auth/signin");
const QString refreshUrl = QStringLiteral("/auth/refresh");

// TimetableElements
const QString addTTElementUrl = QStringLiteral("/timetableElement/addTimetableElement");
const QString editTTElementUrl = QStringLiteral("/timetableElement/editTimetableElement");
const QString deleteTTElementUrl = QStringLiteral("/timetableElement/deleteTimetableElement");
const QString getTTElementsForDayUrl = QStringLiteral("/timetableElement/listOfTimetableElements/");

// BreeksLines
const QString addBreeksLineUrl = QStringLiteral("/breeks/addLine");
const QString editBreeksLineUrl = QStringLiteral("/breeks/editLine");
const QString deleteBreeksLineUrl = QStringLiteral("/breeks/deleteLine");
const QString getAllLinesInWeekUrl = QStringLiteral("/breeks/listOfLines/");

// Notes
const QString addNoteUrl = QStringLiteral("/note/addNote");
const QString editNoteUrl = QStringLiteral("/note/editNote");
const QString getNoteByDateAndPageUrl = QStringLiteral("/note/getNoteByDateAndPage/");

// Image
const QString addImageUrl = QStringLiteral("/image/addImage");
const QString getImageUrl = QStringLiteral("/image/getImage/");
} // namespace Network

#endif // SERVERCONNECTION_H
