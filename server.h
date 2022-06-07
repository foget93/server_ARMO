#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTextEdit>
#include <QTcpSocket>
#include <QLabel>
#include <QImage>
//=======================================================================

class Server : public QWidget
{
    Q_OBJECT

private:
    QTcpServer* p_tcpServer;
    QLabel*     p_txt;
    QLabel*     p_labelForPicture;
    quint32     nextBlockSize;

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public:
    Server(int Port, QWidget *parent = nullptr);

public slots:
    virtual void slotNewConnection();
            void slotReadClient();
};
#endif // SERVER_H
