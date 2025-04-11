#ifndef MESSAGEINTERFACE_H
#define MESSAGEINTERFACE_H

#include <string>

enum MessageType {
    MSG_VEHICLE_FORWARD = 'W',
    MSG_VEHICLE_BACKWARD = 'S',
    MSG_VEHICLE_LEFT = 'A',
    MSG_VEHICLE_RIGHT = 'D'
};

#pragma pack(push, 1)

struct FeedbackInfo {
    int rpm;
    int speed;
};

#pragma pack(pop)

FeedbackInfo parseFeedbackInfo(const char* payload, size_t length);

std::string packConrolMessage(int x, int y);

#endif // MESSAGEINTERFACE_H
