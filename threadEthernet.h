#ifndef __ROBOTRHEAD_H__
#define __ROBOTRHEAD_H__


#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtNetwork/QTcpSocket>
#include <QQueue>
#include <QTcpServer>
#include "protocol.h"

 class threadEthernet: public QThread
 {
     Q_OBJECT

 public:
     threadEthernet(QObject *parent = 0);
     void start(const QHostAddress &hostName, quint16 port);
     ~threadEthernet();

     int SendMessage (struct _msg *message);
     void run();
     void pushMsg (struct _msg *message);
	
 signals:
     void newMessage(struct _msg *message);
     void error(int socketError, const QString &message);

 private:
     QHostAddress hostName;
     quint16 port;
     QMutex mutex;
     QWaitCondition cond;
     bool quit;
     QTcpServer *m_pServer;
     QQueue<_msg *> qCola;
 };

#endif // __ROBOTRHEAD_H__
