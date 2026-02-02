#include "mainwindow.h"
#include "ui_mainwindow.h"

// Biến lưu trữ dữ liệu từ SDK
static float fAcc[3], fAngle[3];
static bool hasNewSensorData = false;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    serialWT = new QSerialPort(this);
    serialSIM = new QSerialPort(this);

    WitInit(WIT_PROTOCOL_NORMAL, 0x50);
    WitRegisterCallBack(onWitDataUpdate);

    connect(serialWT, &QSerialPort::readyRead, this, &MainWindow::readWTData);
    connect(serialSIM, &QSerialPort::readyRead, this, &MainWindow::readSIMData);
}

void MainWindow::on_btnStart_clicked() {
    // 1. Mở UART cho WT901
    serialWT->setPortName("/dev/ttyS0");
    serialWT->setBaudRate(QSerialPort::Baud9600);
    if(serialWT->open(QIODevice::ReadWrite)) {
        serialWT->clear();
        ui->textConsole->append("--- Đã mở cổng WT901 (UART) ---");
    }

    // 2. Mở USB cho SIM7600
    serialSIM->setPortName("/dev/ttyUSB2");
    serialSIM->setBaudRate(QSerialPort::Baud115200);
    if(serialSIM->open(QIODevice::ReadWrite)) {
        ui->textConsole->append("--- Đã mở cổng SIM7600 (USB) ---");
        serialSIM->write("AT+CGPS=1\r\n");

        timerGPS = new QTimer(this);
        connect(timerGPS, &QTimer::timeout, this, &MainWindow::gpsRequest);
        timerGPS->start(2000);
    }
}

void MainWindow::readWTData() {
    QByteArray data = serialWT->readAll();
    for(int i=0; i<data.size(); i++) WitSerialDataIn((uint8_t)data[i]);

    if(hasNewSensorData) {
        ui->label_Acc->setText(QString("Acc: X:%1 Y:%2 Z:%3").arg(fAcc[0],0,'f',2).arg(fAcc[1],0,'f',2).arg(fAcc[2],0,'f',2));
        ui->label_Angle->setText(QString("Roll:%1 Pitch:%2").arg(fAngle[0],0,'f',1).arg(fAngle[1],0,'f',1));
        hasNewSensorData = false;
    }
}

void MainWindow::readSIMData() {
    QByteArray data = serialSIM->readAll();
    QString res = QString::fromLocal8Bit(data);

    // Chỉ hiển thị phản hồi sạch lên console
    if (res.contains("OK") || res.contains("+CGPSINFO")) {
        ui->textConsole->append("SIM: " + res.trimmed());
    }

    if(res.contains("+CGPSINFO:")) {
        QString coords = res.section(':', 1).trimmed();
        if(coords.startsWith(",,,,,")) ui->label_SimStatus->setText("Đang đợi sóng GPS...");
        else ui->label_SimStatus->setText(coords);
    }
}

void MainWindow::gpsRequest() {
    if(serialSIM->isOpen()) serialSIM->write("AT+CGPSINFO\r\n");
}

void MainWindow::on_btnStop_clicked() {
    if(serialWT->isOpen()) serialWT->close();
    if(serialSIM->isOpen()) {
        serialSIM->write("AT+CGPS=0\r\n");
        serialSIM->close();
    }
    if(timerGPS) timerGPS->stop();
    ui->textConsole->append("--- Hệ thống dừng ---");
}

void MainWindow::onWitDataUpdate(uint32_t uiReg, uint32_t uiRegNum) {
    for(uint32_t i = 0; i < uiRegNum; i++) {
        if(uiReg >= AX && uiReg <= AZ) fAcc[uiReg - AX] = sReg[uiReg] / 32768.0f * 16.0f;
        if(uiReg >= Roll && uiReg <= Yaw) fAngle[uiReg - Roll] = sReg[uiReg] / 32768.0f * 180.0f;
        uiReg++;
    }
    hasNewSensorData = true;
}

MainWindow::~MainWindow() { delete ui; }
