#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_actionConnect_device_triggered();

    void on_actionDisconnect_device_triggered();

    void on_actionExit_triggered();

    void on_execute_pushButton_clicked();

    void on_quickStop_pushButton_toggled(bool checked);

    void UnlockInfo1();

    void LockInfo1();

    void UnlockInfo2();

    void LockInfo2();

    void on_tightenButton_clicked();

    void on_loosenButton_clicked();

    void on_executeTwirl_pushButton_clicked();

    void on_stopTwirl_pushButton_toggled(bool checked);

    void on_executeAll_pushButton_clicked();

    void on_actionAutomatic_triggered();

    void on_actionManul_triggered();


    void on_actLiftThrust_pushButton_clicked();

    void on_freezLiftThrust_pushButton_clicked();

    void on_actTwirl_pushButton_clicked();

    void on_freezTwirl_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    int photoelecTimer;
    int updownResetTimer;
    int comboExecInfoAcquiTimer;
    int execTwirlInfoAcquiTimer;
    int execUpdownInfoAcquiTimer;
    int actLiftThrustTimer;
    int actTwirlTimer;
protected:
    void timerEvent(QTimerEvent *);

};

#endif // MAINWINDOW_H
