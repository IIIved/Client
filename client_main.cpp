
#include <QApplication>
#include "client_window.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientWindow clientWin;
    clientWin.show();
    return a.exec();
}
