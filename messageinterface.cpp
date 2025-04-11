#include "messageinterface.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <cstdint>

FeedbackInfo parseFeedbackInfo(const char* payload, size_t length)
{
    FeedbackInfo ret;
    sscanf_s(payload, "%5d,%4d\r\n", &ret.rpm, &ret.speed, length);
    return ret;
}

std::string packConrolMessage(int x, int y) {
    std::string ss;
    // Jamie's hardcoded magic numbers
    uint8_t p1;
    uint8_t p2;
    if(x == 0 && y == 0) {  // stop
        p1 = 140;
        p2 = 140;
    }
    else if(y > 0 || y < 0) {
        if(y > 0) { // forward
            p1 = 253;
            p2 = 253;
        }
        else { // backward
            p1 = 252;
            p2 = 252;
        }
    }
    else {
        if(x > 0) { // right
            p1 = 251;
            p2 = 251;
        }
        else if (x < 0) {   // left
            p1 = 250;
            p2 = 250;
        }
        else {       // mid
            p1 = 249;
            p2 = 249;
        }
    }


    ss.push_back(p1);
    ss.push_back(p2);
    return ss;
}
