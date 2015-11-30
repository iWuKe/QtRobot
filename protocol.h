#ifndef PROTOCOL_H
#define PROTOCOL_H


#define MSG_REQ         1
#define MSG_REPLY       2
#define MSG_SYS_COMMAND 3

#define CMD_BIT_POWEROFF    1

struct _msg
{
   QString m_message;
   qint16 m_velocidad;
   qint16 m_motor1;
   qint16 m_motor2;
   quint8 m_bateriaLi;
   quint8 m_bateriaPb;
   qint16  m_giro;
};

#endif // PROTOCOL_H
