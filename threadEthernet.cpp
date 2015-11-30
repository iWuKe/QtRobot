#include <QtDebug>
#include "threadEthernet.h"

threadEthernet::threadEthernet(QObject *parent)
        : QThread(parent), quit(false)
{
}

threadEthernet::~threadEthernet()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void threadEthernet::pushMsg (struct _msg *message)
{
    qCola.enqueue (message);
}

void threadEthernet::start(const QHostAddress &hostAddress, quint16 port)
{
    QMutexLocker locker(&mutex);
    this->hostName = hostAddress;
    this->port = port;
    if (!isRunning())
        QThread::start();
    else
        cond.wakeOne();
}

void threadEthernet::run()
{
    mutex.lock();
    QHostAddress serverName = hostName;
    quint16 serverPort = port;
    mutex.unlock();
    m_pServer=new QTcpServer;

    const int Timeout = 1 * 1000;

    if (!m_pServer->listen (serverName, serverPort))
    {
        emit error(m_pServer->serverError(), m_pServer->errorString());
        return;
    }

    while ( m_pServer->isListening() && !quit)
    {
        if (m_pServer->waitForNewConnection())
        {
            QTcpSocket *pClient = m_pServer->nextPendingConnection();
        }
    }

   m_pServer->close();
}
