/********************************************************************************
** Form generated from reading UI file 'client_window.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENT_WINDOW_H
#define UI_CLIENT_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientWindow
{
public:
    QGridLayout *gridLayout;
    QPushButton *resourceRequestButton;
    QListView *clientView;
    QPushButton *connectButton;

    void setupUi(QWidget *ClientWindow)
    {
        if (ClientWindow->objectName().isEmpty())
            ClientWindow->setObjectName("ClientWindow");
        ClientWindow->resize(400, 300);
        gridLayout = new QGridLayout(ClientWindow);
        gridLayout->setObjectName("gridLayout");
        resourceRequestButton = new QPushButton(ClientWindow);
        resourceRequestButton->setObjectName("resourceRequestButton");
        resourceRequestButton->setEnabled(false);

        gridLayout->addWidget(resourceRequestButton, 1, 0, 1, 2);

        clientView = new QListView(ClientWindow);
        clientView->setObjectName("clientView");
        clientView->setEnabled(true);
        clientView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        gridLayout->addWidget(clientView, 2, 0, 1, 2);

        connectButton = new QPushButton(ClientWindow);
        connectButton->setObjectName("connectButton");

        gridLayout->addWidget(connectButton, 0, 0, 1, 2);


        retranslateUi(ClientWindow);

        QMetaObject::connectSlotsByName(ClientWindow);
    } // setupUi

    void retranslateUi(QWidget *ClientWindow)
    {
        ClientWindow->setWindowTitle(QCoreApplication::translate("ClientWindow", "Qt  Client Client", nullptr));
        resourceRequestButton->setText(QCoreApplication::translate("ClientWindow", "ResourceRequest", nullptr));
        connectButton->setText(QCoreApplication::translate("ClientWindow", "Connect", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientWindow: public Ui_ClientWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENT_WINDOW_H
