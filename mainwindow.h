#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include "wit_c_sdk.h"
#include "REG.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void readWTData();   // Đọc từ UART (/dev/ttyS0)
    void readSIMData();  // Đọc từ USB (/dev/ttyUSB2)
    void gpsRequest();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialWT;
    QSerialPort *serialSIM;
    QTimer *timerGPS;

    static void onWitDataUpdate(uint32_t uiReg, uint32_t uiRegNum);
};

#endif
