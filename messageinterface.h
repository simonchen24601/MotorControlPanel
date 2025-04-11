#ifndef MESSAGEINTERFACE_H
#define MESSAGEINTERFACE_H

#include <QObject>
#include <QSerialPort>
// #include <QBluetooth>
// QBluetooth: Win32 backend has been removed. There will not be a working Bluetooth backend when Qt is built with mingw
// QBluetooth:

enum MessageType {
    MSG_VEHICLE_FORWARD = 'W',
    MSG_VEHICLE_BACKWARD = 'S',
    MSG_VEHICLE_LEFT = 'A',
    MSG_VEHICLE_RIGHT = 'D'
};

class MessageInterfaceBase : QObject
{
    Q_OBJECT
public:
    MessageInterfaceBase();
    virtual ~MessageInterfaceBase() {};

    virtual int init() = 0;
    virtual int connect() = 0;
    virtual int send_msg() { return -1; };

signals:
    void on_recv_msg();
};

// class SerialMessageInterface : public MessageInterfaceBase {
// public:
//     int init() override;
//     int connect() override;
// };

#endif // MESSAGEINTERFACE_H
