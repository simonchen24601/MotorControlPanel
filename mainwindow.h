// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLineEdit>
#include "controller.h"

QT_BEGIN_NAMESPACE

class QLabel;
class QTimer;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);
    void handleBytesWritten(qint64 bytes);
    void handleWriteTimeout();

    // Controller Event Callback
    void handleControllerConnected(int controller_number);
    void handleControllerDisconnected(int controller_number);
    void handleControllerLeftJoystickPushed(int controller_number, int x_offset, int y_offset);

private:
    void initActionsConnections();

private:
    void showStatusMessage(const QString &message);
    void showWriteError(const QString &message);

    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    Console *m_console = nullptr;
    SettingsDialog *m_settings = nullptr;
    qint64 m_bytesToWrite = 0;
    QTimer *m_timer = nullptr;
    QLineEdit *m_lineEditRPM = nullptr;
    QLineEdit *m_lineEditSpeed = nullptr;
    QSerialPort *m_serial = nullptr;
    XBoxController *m_controller = nullptr;
};

#endif // MAINWINDOW_H
