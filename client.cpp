#include "client.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>

ResurcerClient::ResurcerClient(QObject *parent)
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
{
    connect(m_clientSocket, &QTcpSocket::connected, this, &ResurcerClient::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &ResurcerClient::disconnected);
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ResurcerClient::onReadyRead);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ResurcerClient::error);
#else
    connect(m_clientSocket, &QAbstractSocket::errorOccurred, this, &ResurcerClient::error);
#endif
    connect(m_clientSocket, &QTcpSocket::disconnected, this, [this]()->void{m_loggedIn = false;});
}

void ResurcerClient::login(const QString &userName)
{
    if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);
        QJsonObject message;
        message[QStringLiteral("type")] = QStringLiteral("login");
        message[QStringLiteral("username")] = userName;
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void ResurcerClient::resourceRequest(qint32 mask, qint64 lifeTimeSeconds) {

    Q_ASSERT(mask);

    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("request");
    message[QStringLiteral("request")] = mask;
    message[QStringLiteral("time")] = QString::number(lifeTimeSeconds);

    sendJsonObject(message);
}

void ResurcerClient::sendJsonObject(const QJsonObject& message) {

    emit messageReceived("my: ",  "sendJsonObject " + QJsonDocument(message).toJson(QJsonDocument::Compact));

    QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);
    clientStream << QJsonDocument(message).toJson();
}

void ResurcerClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void ResurcerClient::jsonReceived(const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value("type");
    if (typeVal.isNull() || !typeVal.isString())
        return;
    if (typeVal.toString() == "login") {
        if (m_loggedIn)
            return;
        const QJsonValue resultVal = docObj.value("status");
        qint32 loginSuccess = resultVal.toString().toInt(0);
        if (loginSuccess == 1) {
            emit loggedIn();
            return;
        }

        const QJsonValue reasonVal = docObj.value("reason");
        emit loginError(reasonVal.toString());

    } else {
        emit messageReceived("server: ", QString::fromUtf8(QJsonDocument(docObj).toJson()));
    }
}

void ResurcerClient::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address, port);
}

void ResurcerClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);

    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    jsonReceived(jsonDoc.object());
            }
        } else {
            break;
        }
    }
}
