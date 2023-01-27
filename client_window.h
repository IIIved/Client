#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QWidget>
#include <QAbstractSocket>
#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QLayoutItem>
#include <QSpinBox>

class ResurcerClient;
class QStandardItemModel;
namespace Ui { class ClientWindow; }
class ClientWindow : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ClientWindow)
public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();
private:
    Ui::ClientWindow *ui;
    ResurcerClient *m_client;
    QStandardItemModel *m_model;
    QString m_lastUserName;
private slots:
    void attemptConnection();
    void resourceRequest();
    void connectedToServer();
    void attemptLogin(const QString &userName);
    void loggedIn();
    void loginFailed(const QString &reason);
    void messageReceived(const QString &sender, const QString &text);
    void disconnectedFromServer();
    void error(QAbstractSocket::SocketError socketError);
};

#endif // CLIENTWINDOW_H
