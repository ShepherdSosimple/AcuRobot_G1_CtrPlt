#ifndef SENSOR_H
#define SENSOR_H

//基类:传感器
class Sensor
{
public:
    int num;  //传感器对应的采集卡端口
    float voltage;

    Sensor();
    explicit Sensor(int n);
    ~Sensor();
    void GetVoltage();

};

//派生类：光电开关
class Photoelec: public Sensor
{
private:
    const float max_v = 1.9;
    const float min_v = 1.0;
public:
    bool arrive;
    Photoelec();
    Photoelec(int n);
    ~Photoelec();
    bool Arrive();
};


#endif // SENSOR_H
