#pragma once

class ServoControl
{
public:
    ServoControl();
    void grabPanel();
    void setup();
    void jawOpen();
    void jawClose();
    void jawStop();
    void setEffort(int effort);
    int getPosition();

private:
};