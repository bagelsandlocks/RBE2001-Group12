#pragma once

class BlueMotor
{
public:
    BlueMotor();
    void setEffort(int effort);
    void setEffortTuned(int effort);
    int moveTo(long position);
    long getPosition();
    void reset();
    void setup();

private:
    void setEffort(int effort, bool clockwise);
    static void isr1();
    static void isr2();
    static void isr3();
    static void isr4();
    


    const int tolerance = 3;
    const int PWMOutPin = 11;
    const int AIN2 = 4;
    const int AIN1 = 13;
    const int ENCA = 0;
    const int ENCB = 1;
    const int deadBandAjust = 100;
    float Kp = 1.5;
};