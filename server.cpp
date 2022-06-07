#include "server.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>

/*private:
    QTcpServer* p_tcpServer;
    QTextEdit*  p_txt;
    quint16     nextBlockSize;*/
//=================================================================

void Server::sendToClient(QTcpSocket *pSocket, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out (&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);

    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

//=================================================================

Server::Server(int nPort, QWidget *parent )
    : QWidget(parent), nextBlockSize{0}
{
    p_tcpServer = new QTcpServer(this);

    if (!p_tcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0,
                              "Server error",
                              "Unable to start the server:"
                              + p_tcpServer->errorString()
                              );
        p_tcpServer->close();
        return;
    }

    connect(p_tcpServer,    &QTcpServer::newConnection,
            this,           &Server::slotNewConnection
            );

    //p_txt = new QTextEdit;
    p_txt = new QLabel;

    p_labelForPicture = new QLabel();//!!!!!!!!!!!!!
    p_labelForPicture->setFrameStyle(QFrame::Box | QFrame::Raised);
    p_labelForPicture->setLineWidth(2);
    //p_labelForPicture->setFixedSize(QSize(300,300));

    //p_txt->setReadOnly(true);

    //layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(p_txt);
    pvbxLayout->addWidget(p_labelForPicture);
    this->setLayout(pvbxLayout);
}

//=================================================================

/*virtual*/
void Server::slotNewConnection()
{
    QTcpSocket* pClientSocket = p_tcpServer->nextPendingConnection();

    connect(pClientSocket, &QTcpSocket::disconnected,
            pClientSocket, &QTcpSocket::deleteLater
            );

    connect(pClientSocket, &QTcpSocket::readyRead,
            this,          &Server::slotReadClient
            );

    //sendToClient(pClientSocket, "Server Response: Connected!");
}

//=================================================================

void Server::slotReadClient()
{
    QTcpSocket* pClientSocket = dynamic_cast<QTcpSocket*>(sender());/*(QTcpSocket*)sender()*/;
    QDataStream in (pClientSocket);
    in.setVersion(QDataStream::Qt_6_2);

    for (;;) {
        if (!nextBlockSize) {
            if (pClientSocket->bytesAvailable() < static_cast<int>(sizeof(quint32)) )
                break;

            in >> nextBlockSize;
        }
        if (pClientSocket->bytesAvailable() < nextBlockSize)
            break;

//        QTime time;
//        QString str;
        QImage image;
        in >> image;

//        QString strMessage =
//                time.toString() + " " + "Client has sent: " + str;
        //p_txt->append(strMessage);
//        p_txt->setText(strMessage);
//////////////////////////////////
        //QImage img("C:\\Users\\Дмитрий\\Desktop\\Собесы.PNG");
        p_labelForPicture->setPixmap(QPixmap::fromImage(image));

        nextBlockSize = 0;

//        sendToClient(pClientSocket,
//                     "Server Response: Received \"" + str + "\""
//                     );
    }
}

//=================================================================


