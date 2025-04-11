#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include <QTimer>

#ifdef _WIN32
#include <windows.h>
// https://learn.microsoft.com/en-us/windows/win32/xinput/xinput-game-controller-apis-portal
#include <Xinput.h>
#else
#error "platform not supported"
#endif

constexpr int CONTROLLER_PULL_RATE_MS = 500;
constexpr int CONTROLLER_CONNECTED_MAX = 4;
constexpr int CONTROLLER_INPUT_DEADZONE = 0.24f * FLOAT(0x7FFF);  // Default to 24% of the +/- 32767 range
/*
 * The XInput API supports up to four controllers connected at any time. The XInput functions all
 * require a dwUserIndex parameter that is passed in to identify the controller being set or queried.
 */

class XBoxController : public QObject
{
    Q_OBJECT

public:
    explicit XBoxController(QObject *parent = nullptr);
    ~XBoxController();

    struct ControllerState {
        XINPUT_STATE state;
        bool isConnected;
        bool isConnectedLastTime;
    };

signals:
    void onControllerConnected(int controller_number);
    void onControllerDisconnected(int controller_number);
    // void on_controller_button_clicked(int controller_number, int button_idx);
    void onControllerLeftJoystickPushed(int controller_number, int x_offset, int y_offset);
    void onControllerRightJoystickPushed(int controller_number, int x_offset, int y_offset);

private slots:
    void pollJoystick();

private:
    QTimer pull_timer_;
    ControllerState controllers_[CONTROLLER_CONNECTED_MAX];

    int controller_num_;
};

#endif // CONTROLLER_H
