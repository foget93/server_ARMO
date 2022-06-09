#include "server.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>


//=================================================================

void Server::sendToClient(QTcpSocket *pSocket, const QString &str)
{
    //ф-ия ответ клиенту:str
    QByteArray arrBlock;
    QDataStream out (&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);


    out << quint32(0) << str;
    out.device()->seek(0);
    out << quint32(arrBlock.size() - sizeof(quint32));
    pSocket->write(arrBlock);
}

//=================================================================

Server::Server(QWidget *parent )
    : QWidget(parent), nextBlockSize{0}
{
    //настройка gui
    this->setWindowTitle("Server");
    this->resize(600,600);

    plabelForImage = new QLabel();
    plabelForImage->setFrameStyle(QFrame::Box | QFrame::Raised);
    plabelForImage->setLineWidth(2);

    plabelInfo = new QLabel("image.info");
    plabelInfo->setFrameStyle(QFrame::Box | QFrame::Raised);
    plabelInfo->setLineWidth(2);

    lblPort = new QLabel("порт: ");
    plineEditForPort = new QLineEdit;
    plineEditForPort->setText("2323");

    pbtnListen = new QPushButton("Слушать"); // коннект сделать
    //p_txt->setReadOnly(true);

    //layout setup
    QBoxLayout* pbxLayout = new QBoxLayout(QBoxLayout::TopToBottom);

    QHBoxLayout* phbxLayout = new QHBoxLayout;
    phbxLayout->addWidget(plabelInfo,3);
    phbxLayout->addStretch(1);
    phbxLayout->addWidget(lblPort);
    phbxLayout->addWidget(plineEditForPort);
    phbxLayout->addWidget(pbtnListen);

    pbxLayout->addWidget(plabelForImage);
    pbxLayout->addLayout(phbxLayout);

    this->setLayout(pbxLayout);

//=================================================================

    ptcpServer = new QTcpServer(this);

    connect(ptcpServer,     &QTcpServer::newConnection,
            this,           &Server::slotNewConnection
           );//обработка новых подключений

    connect(pbtnListen,     &QPushButton::clicked,
            this,           &Server::on_btnListen
           );//кнопка включения сервера
}

//=================================================================

void Server::startServer()
{
    //ф-ия запуска сервера
    if (!ptcpServer->listen(QHostAddress::Any, plineEditForPort->text().toInt())) {
        QMessageBox::critical(0,
                              "Server error",
                              "Unable to start the server:"
                              + ptcpServer->errorString()
                              );
        ptcpServer->close();
        return;
    }
    this->lblPort->setText("Слушаю порт: ");
    this->nextBlockSize = 0;
}

//=================================================================

void Server::slotNewConnection()
{
    //выделяем сокет
    this->pClientSocket = ptcpServer->nextPendingConnection();

    connect(pClientSocket, &QTcpSocket::disconnected,
            pClientSocket, &QTcpSocket::deleteLater
           );


    connect(pClientSocket, &QTcpSocket::readyRead,
            this,          &Server::slotReadClient
           );

    Sockets.push_back(pClientSocket);
    sendToClient(pClientSocket, QTime::currentTime().toString() + " Сервер: соединение установлено!");
}

//=================================================================

void Server::slotReadClient()
{
    /*
     * Цикл for нужен потому, что не все высланные клиентом данные могут прийти одновременно.
     * Сервер должен "уметь" получать как весь блок целиком, так и часть блока, а так же все блоки сразу.
     * Каждый переданный блок начинается с nextBlockSize.
     * Размер блока (nextBlockSize) считывается при условии pClientSocket->bytesAvailable() не меньше 4 байт
     * и nextBlockSize == 0. Далее проверяем этот размер с pClientSocket->bytesAvailable() и считываем+выводим данные
     * В зависимости от размера image отвечаем клиенту sendToClient.
    */
    this->pClientSocket = dynamic_cast<QTcpSocket*>(sender());/*(QTcpSocket*)sender()*/;
    QDataStream in (pClientSocket);
    in.setVersion(QDataStream::Qt_6_2);

    for (;;) {
        if (!nextBlockSize) {
            if (pClientSocket->bytesAvailable() < static_cast<qint64>(sizeof(quint32)) ) // что бы компилятор не ругался :)
                break;

            in >> nextBlockSize;
        }
        if (pClientSocket->bytesAvailable() < nextBlockSize)
            break;

        QImage image;
        QTime time;
        in >> imageName >> image >> time;

        if (!image.isNull()) {
            plabelForImage->setPixmap(QPixmap::fromImage(image).scaled(plabelForImage->size(), Qt::IgnoreAspectRatio)); //QPixmap::fromImage(image) Qt::KeepAspectRatio
            plabelForImage->setScaledContents(true);
            plabelInfo->setText(time.toString() + " " + imageName);

            nextBlockSize = 0;
            sendToClient(pClientSocket,
                         QString("%1 Сервер: Файл %2 получен.")
                             .arg(QTime::currentTime().toString())
                             .arg(imageName)
                         );
        }
        else {
            sendToClient(pClientSocket,
                         QString("%1 Сервер: Файл %2пуст.")
                             .arg(QTime::currentTime().toString())
                             .arg(imageName)
                         );
            nextBlockSize = 0;
        }
    }
}

//=================================================================

void Server::on_btnListen()
{
    //стартуем сервер startServer()
    if (this->pbtnListen->text() == tr("Слушать")) {
        this->pbtnListen->setText(tr("Отключить"));
        this->startServer();

    }
    else {
        this->pbtnListen->setText(tr("Слушать"));
        this->lblPort->setText("порт: ");

//        if (this->pClientSocket != nullptr) {
//                this->pClientSocket->disconnectFromHost();
////                if(this->pClientSocket->state() != QAbstractSocket::UnconnectedState)
////                    this->pClientSocket->waitForDisconnected(3000);
////                delete this->pClientSocket;
//        }

        ptcpServer->close();

    }
}

//=================================================================


