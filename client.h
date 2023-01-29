#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class QHostAddress;
class QJsonDocument;
class ResurcerClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ResurcerClient)
public:
    explicit ResurcerClient(QObject *parent = nullptr);
public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void login(const QString &userName);
    void disconnectFromHost();

    void sendJsonObject(const QJsonObject& message);
    void resourceRequest(qint32 mask, qint64 lifeTimeSeconds);


private slots:
    void onReadyRead();
signals:
    void connected();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const QString &sender, const QString &text);
    void error(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_clientSocket;
    bool m_loggedIn;
    void jsonReceived(const QJsonObject &doc);
};

#endif // CLIENT_H
