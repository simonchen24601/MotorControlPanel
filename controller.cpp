#include "controller.h"

XBoxController::XBoxController(QObject *parent)
    : QObject(parent)
{
    ZeroMemory(&controllers_, sizeof(ControllerState) * CONTROLLER_CONNECTED_MAX);

    connect(&pull_timer_, &QTimer::timeout, this, &XBoxController::pollJoystick);
    pull_timer_.start(CONTROLLER_PULL_RATE_MS);
}

XBoxController::~XBoxController()
{
    pull_timer_.stop();
}

void XBoxController::pollJoystick()
{
    DWORD dwResult;
    for( DWORD i = 0; i < CONTROLLER_CONNECTED_MAX; i++ ) {
        dwResult = XInputGetState( i, &controllers_[i].state );

        if( dwResult == ERROR_SUCCESS ) {
            controllers_[i].isConnected = true;
            if(!controllers_[i].isConnectedLastTime) {
                emit onControllerConnected(i);
                controllers_[i].isConnectedLastTime = true;
            }
        }
        else {
            controllers_[i].isConnected = false;
            if(controllers_[i].isConnectedLastTime) {
                emit onControllerDisconnected(i);
                controllers_[i].isConnectedLastTime = false;
            }
        }

        // Zero value if thumbsticks are within the dead zone
        if( controllers_[i].state.Gamepad.sThumbLX < CONTROLLER_INPUT_DEADZONE &&
             controllers_[i].state.Gamepad.sThumbLX > -CONTROLLER_INPUT_DEADZONE )
        {
            controllers_[i].state.Gamepad.sThumbLX = 0;
        }
        if ( controllers_[i].state.Gamepad.sThumbLY < CONTROLLER_INPUT_DEADZONE &&
             controllers_[i].state.Gamepad.sThumbLY > -CONTROLLER_INPUT_DEADZONE )
        {
            controllers_[i].state.Gamepad.sThumbLY = 0;
        }
        if( controllers_[i].state.Gamepad.sThumbRX < CONTROLLER_INPUT_DEADZONE &&
             controllers_[i].state.Gamepad.sThumbRX > -CONTROLLER_INPUT_DEADZONE )
        {
            controllers_[i].state.Gamepad.sThumbRX = 0;
        }
        if( controllers_[i].state.Gamepad.sThumbRY < CONTROLLER_INPUT_DEADZONE &&
             controllers_[i].state.Gamepad.sThumbRY > -CONTROLLER_INPUT_DEADZONE )
        {
            controllers_[i].state.Gamepad.sThumbRY = 0;
        }
    }

    for( DWORD i = 0; i < CONTROLLER_CONNECTED_MAX; i++ ) {
        if(controllers_[i].isConnected == false) {
            continue;
        }

        auto lx = controllers_[i].state.Gamepad.sThumbLX;
        auto ly = controllers_[i].state.Gamepad.sThumbLY;
        emit onControllerLeftJoystickPushed(i, lx, ly);

        auto rx = controllers_[i].state.Gamepad.sThumbRX;
        auto ry = controllers_[i].state.Gamepad.sThumbRY;
        emit onControllerRightJoystickPushed(i, rx, ry);
    }
}
