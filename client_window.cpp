#include "client_window.h"
#include "ui_client_window.h"
#include "client.h"

#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>

ClientWindow::ClientWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWindow)
    , m_client(new ResurcerClient(this))
    , m_model(new QStandardItemModel(this))
{
    ui->setupUi(this);

    m_model->insertColumn(0);

    ui->clientView->setModel(m_model);

    connect(m_client, &ResurcerClient::connected, this, &ClientWindow::connectedToServer);
    connect(m_client, &ResurcerClient::loggedIn, this, &ClientWindow::loggedIn);
    connect(m_client, &ResurcerClient::loginError, this, &ClientWindow::loginFailed);
    connect(m_client, &ResurcerClient::messageReceived, this, &ClientWindow::messageReceived);
    connect(m_client, &ResurcerClient::disconnected, this, &ClientWindow::disconnectedFromServer);
    connect(m_client, &ResurcerClient::error, this, &ClientWindow::error);

    connect(ui->connectButton, &QPushButton::clicked, this, &ClientWindow::attemptConnection);
    connect(ui->resourceRequestButton, &QPushButton::clicked, this, &ClientWindow::resourceRequest);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::resourceRequest()
{
    ui->resourceRequestButton->setEnabled(true);

    auto dialog = QDialog(this);
        QFormLayout form(&dialog);

        QSpinBox *timeInput = new QSpinBox( &dialog);
        timeInput->setSuffix(" sec");
        timeInput->setRange(10, 7200);

        QString timeLabel = QString("time");
        form.addRow(timeLabel, timeInput);

        QGridLayout* resourcesBox = new QGridLayout(&dialog);

        QCheckBox* checkboxes[4];
        checkboxes[0] = new QCheckBox(&dialog);
        checkboxes[1] = new QCheckBox(&dialog);
        checkboxes[2] = new QCheckBox(&dialog);
        checkboxes[3] = new QCheckBox(&dialog);
        resourcesBox->addWidget(checkboxes[0], 0, 0);
        resourcesBox->addWidget(checkboxes[1], 0, 1);
        resourcesBox->addWidget(checkboxes[2], 0, 2);
        resourcesBox->addWidget(checkboxes[3], 0, 3);

        form.addRow(new QLabel("resources:"));
        form.addRow(resourcesBox);

        form.addRow(new QLabel("1"), checkboxes[0]);
        form.addRow(new QLabel("2"), checkboxes[1]);
        form.addRow(new QLabel("3"), checkboxes[2]);
        form.addRow(new QLabel("4"), checkboxes[3]);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);

        form.addRow(&buttonBox);

        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        dialog.exec();

        for(int i = 0; i < 4; i++){
            if(checkboxes[i]->isChecked()){
                m_client->resourceRequest(i+1,timeInput->cleanText().toInt());
            }
        }
}

void ClientWindow::attemptConnection()
{
    const QString hostAddress = QInputDialog::getText(
        this
        , tr("Chose Server")
        , tr("Server Address")
        , QLineEdit::Normal
        , QStringLiteral("127.0.0.1:1967")
    );
    if (hostAddress.isEmpty())
        return;
    ui->connectButton->setEnabled(false);

    QStringList pieces = QString(hostAddress).split( ":" );

    m_client->connectToServer(QHostAddress(pieces[0]), pieces[1].toInt());
}

void ClientWindow::connectedToServer()
{
    const QString newUsername = QInputDialog::getText(this, tr("Chose Username"), tr("Username"));
    if (newUsername.isEmpty()){
        return m_client->disconnectFromHost();
    }
    attemptLogin(newUsername);
}

void ClientWindow::attemptLogin(const QString &userName)
{
    m_client->login(userName);
}

void ClientWindow::loggedIn()
{
    ui->resourceRequestButton->setEnabled(true);
    m_lastUserName.clear();
}

void ClientWindow::loginFailed(const QString &reason)
{
    QMessageBox::critical(this, tr("Error"), reason);
    connectedToServer();
}

void ClientWindow::messageReceived(const QString &sender, const QString &text)
{
    int newRow = m_model->rowCount();

    if (m_lastUserName != sender) {
        m_lastUserName = sender;

        QFont boldFont;

        boldFont.setBold(true);

        m_model->insertRows(newRow, 2);
        m_model->setData(m_model->index(newRow, 0), sender + QLatin1Char(':'));
        m_model->setData(m_model->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
        m_model->setData(m_model->index(newRow, 0), boldFont, Qt::FontRole);
        ++newRow;
    } else {
        m_model->insertRow(newRow);
    }
    m_model->setData(m_model->index(newRow, 0), text);
    m_model->setData(m_model->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    ui->clientView->scrollToBottom();
}

void ClientWindow::disconnectedFromServer()
{
    QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));

    ui->resourceRequestButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    m_lastUserName.clear();
}

void ClientWindow::error(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        return; // handled by disconnectedFromServer
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The host refused the connection"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The proxy refused the connection"));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the proxy"));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the server"));
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, tr("Error"), tr("You don't have permissions to execute this operation"));
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, tr("Error"), tr("Too many connections opened"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(this, tr("Error"), tr("Operation timed out"));
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy timed out"));
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, tr("Error"), tr("Unable to reach the network"));
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, tr("Error"), tr("An unknown error occured"));
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, tr("Error"), tr("Operation not supported"));
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, tr("Error"), tr("Your proxy requires authentication"));
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy comunication failed"));
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));
        return;
    default:
        Q_UNREACHABLE();
    }

    ui->connectButton->setEnabled(true);
    ui->resourceRequestButton->setEnabled(false);
    m_lastUserName.clear();
}
