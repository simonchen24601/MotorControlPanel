// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "messageinterface.h"

#include <QLabel>
#include <QMessageBox>
#include <QTimer>

#include <chrono>
#include <string>
#include <sstream>

static constexpr std::chrono::seconds kWriteTimeout = std::chrono::seconds{5};

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    //! [0]
    m_status(new QLabel),
    m_console(new Console),
    m_settings(new SettingsDialog(this)),
    m_timer(new QTimer(this)),
    //! [1]
    m_serial(new QSerialPort(this)),
    m_controller(new XBoxController(this))
{
    //! [1]
    m_ui->setupUi(this);
    m_console->setEnabled(false);

    m_lineEditRPM = m_ui->lineEditRPM;
    m_lineEditSpeed = m_ui->lineEditSpeed;
    m_lineEditRPM->setText("-1");
    m_lineEditSpeed->setText("-1");

    // setCentralWidget(m_console);
    m_ui->verticalLayout_main->addWidget(m_console);

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handleWriteTimeout);
    m_timer->setSingleShot(true);

    //! [2]
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(m_serial, &QSerialPort::bytesWritten, this, &MainWindow::handleBytesWritten);
    //! [2]
    connect(m_console, &Console::getData, this, &MainWindow::writeData);
    //! [3]

    connect(m_controller, &XBoxController::onControllerConnected, this, &MainWindow::handleControllerConnected);
    connect(m_controller, &XBoxController::onControllerDisconnected, this, &MainWindow::handleControllerDisconnected);
    connect(m_controller, &XBoxController::onControllerLeftJoystickPushed, this, &MainWindow::handleControllerLeftJoystickPushed);
}
//! [3]

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                              .arg(p.name, p.stringBaudRate, p.stringDataBits,
                                   p.stringParity, p.stringStopBits, p.stringFlowControl));
        QByteArray data;
        data.push_back(static_cast<char>(0xff));
        writeData(data);

    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About The Control Panel"),
                       tr("TODO"));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    const qint64 written = m_serial->write(data);
    if (written == data.size()) {
        m_bytesToWrite += written;
        m_timer->start(kWriteTimeout);
    } else {
        const QString error = tr("Failed to write all data to port %1.\n"
                                 "Error: %2").arg(m_serial->portName(),
                                       m_serial->errorString());
        showWriteError(error);
    }
}
//! [6]

//! [7]
void MainWindow::readData()
{
    m_console->putData("[serial] ");
    const QByteArray data = m_serial->readAll();
    m_console->putData(data);

    FeedbackInfo feedback = parseFeedbackInfo(data.toStdString().c_str(), data.size());
    m_lineEditRPM->setText(std::to_string(feedback.rpm).c_str());
    m_lineEditSpeed->setText(std::to_string(feedback.speed).c_str());
}
//! [7]

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}
//! [8]

//! [9]
void MainWindow::handleBytesWritten(qint64 bytes)
{
    m_bytesToWrite -= bytes;
    if (m_bytesToWrite == 0)
        m_timer->stop();
}
//! [9]

void MainWindow::handleWriteTimeout()
{
    const QString error = tr("Write operation timed out for port %1.\n"
                             "Error: %2").arg(m_serial->portName(),
                                   m_serial->errorString());
    showWriteError(error);
}

void MainWindow::handleControllerConnected(int controller_number)
{
    std::stringstream ss;
    ss << "[controller] no." << controller_number << " connected\r\n";
    m_console->putData(ss.str().c_str());
}

void MainWindow::handleControllerDisconnected(int controller_number)
{
    std::stringstream ss;
    ss << "[controller] no." << controller_number << " disconnected\r\n";
    m_console->putData(ss.str().c_str());
}

void MainWindow::handleControllerLeftJoystickPushed(int controller_number, int x_offset, int y_offset)
{
    if(x_offset != 0 || y_offset != 0) {
        std::stringstream ss;
        ss << "[controller] no." << controller_number << " LX=" << x_offset << " LY=" << y_offset << "\r\n";
        m_console->putData(ss.str().c_str());
    }

    // TODO: send the data to the serial port
    if(m_serial->isOpen()) {
        QByteArray data(packConrolMessage(x_offset, y_offset));
        writeData(data);
        // m_serial->write();
    }
}

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::showWriteError(const QString &message)
{
    QMessageBox::warning(this, tr("Warning"), message);
}
