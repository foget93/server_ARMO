#include "server.h"

#include <QApplication>
#include <QScrollArea>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server server;
    server.show();
    return a.exec();
}
