#ifndef MOTORTHREADS_H
#define MOTORTHREADS_H
#include <QThread>

/**********************************************Threads: updownMotor*********************************************/
/**********************************************Threads: updownMotor*********************************************/

class MoveToOriginTd : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};

class ReplaceNeedleManulTd : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};


class PierceAndLiftThrustTd : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};

class PierceOnlyTd : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};

class LiftThrustOnlyTd : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};



/**********************************************Thread: twirlMotor*********************************************/
/**********************************************Thread: twirlMotor*********************************************/

class TwirlTd : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};


/**********************************************Threads: Combo*********************************************/
/**********************************************Threads: Combo*********************************************/
class Combo1Td : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};

class Combo2Td : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};

class Combo3Td : public QThread
{
private:
    volatile bool stopped;
protected:
    void run();
public:
    void stop();
    void renew();
};


#endif // MOTORTHREADS_H
