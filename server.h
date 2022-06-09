#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTextEdit>
#include <QTcpSocket>
#include <QLabel>
#include <QImage>
#include <QLineEdit>
#include <QPushButton>
//=======================================================================

class Server : public QWidget
{
    Q_OBJECT

private:
    QTcpServer*             ptcpServer;
    QTcpSocket*             pClientSocket;
    QVector<QTcpSocket*>    Sockets;

    QLabel*                 plabelForImage;
    QLabel*                 plabelInfo;

    QLabel*                 lblPort;
    QLineEdit*              plineEditForPort;
    QPushButton*            pbtnListen;

    QString                 imageName;
    quint32                 nextBlockSize;


private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public:
    explicit Server(QWidget *parent = nullptr);
    void startServer();

public slots:
     //void incomingConnection(qintptr socketDescriptor) override;
    void slotNewConnection();
    void slotReadClient();
    void on_btnListen();
};
#endif // SERVER_H
