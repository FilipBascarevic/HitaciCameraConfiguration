/** Serial.cpp
 *
 * A very simple serial port control class. Implementation of all methods from this class.
 *
 * @author Filip Bascarevic
 *
 * @version 0.1 -- 30 April 2019
 */

#include"serial.h"

Serial::~Serial()
{
    // close serial port
    close();

    BaudRate = QSerialPort::Baud9600;
    DataBits = QSerialPort::Data8;
    StopBits = QSerialPort::OneStop;
    Parity   = QSerialPort::NoParity;

}


bool Serial::open(QString serialPortName, QSerialPort::BaudRate serialPortBaudRate, QSerialPort::DataBits serialPortDataBits, QSerialPort::StopBits serialPortStopBits,
    QSerialPort::Parity serialPortParity)
{

    // Set a name of serial port
    serialPort.setPortName(serialPortName);
    // Set a BaudRate of serial port
    serialPort.setBaudRate(serialPortBaudRate);
    // Set a DataBits of serial port
    serialPort.setDataBits(serialPortDataBits);
    // Set a StopBits of serial port
    serialPort.setStopBits(serialPortStopBits);
    // Set a Parity of serial port
    serialPort.setParity(serialPortParity);

    // Open serial port
    if (!serialPort.open(QIODevice::ReadWrite)) {
        standardOutput << QObject::tr("Failed to open port %1, error: %2")
                          .arg(serialPortName).arg(serialPort.error()) << endl;
        // Close port if it wasn't open correctly
        this -> ~Serial();
        return false;
    }
    else {
        BaudRate = serialPortBaudRate;
        DataBits = serialPortDataBits;
        StopBits = serialPortStopBits;
        Parity   = serialPortParity;

        return true;
    }


}

void Serial::close()
{

    // close serial port
    serialPort.close();

}

qint64 Serial::write(const char *buffer, qint64 buffLen)
{

    qint64 numWritten = serialPort.write(buffer, buffLen);

    if(!serialPort.waitForBytesWritten(-1)){
        standardOutput << QObject::tr("bytesWritten() signal hasn't been emitted.") << endl;
    }

    return numWritten;

}

qint64 Serial::read(char *buff, qint64 buffLen,int timeOut, bool nullTerminate)
{
    if (nullTerminate) {
        buffLen--;
    }

    qint64 numRead = 0;

    if (serialPort.waitForReadyRead(timeOut)){
        numRead = serialPort.read(buff, buffLen);
    }
    else {
        standardOutput << QObject::tr("Wait read response timeout. ReadyRead() signal hasn't been emitted.") << endl;
        numRead = 0;
    }



    if (nullTerminate) {
        buff[buffLen] = '\0';
    }

    return numRead;
}

#define FLUSH_BUFFSIZE 10

void Serial::flush()
{
    char buff[FLUSH_BUFFSIZE];
    qint64 numBytes;
    numBytes = read(buff, FLUSH_BUFFSIZE, false);

    while (numBytes != 0) {
        numBytes = read(buff, FLUSH_BUFFSIZE, false);
    }

}

//void availablePorts()
void availableSerialPorts()
    {
        QTextStream out(stdout);
        const auto serialPortInfos = QSerialPortInfo::availablePorts();

        out << "Total number of ports available: " << serialPortInfos.count() << endl;

        const QString blankString = "N/A";
        QString description;
        QString manufacturer;
        QString serialNumber;

        for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
            description = serialPortInfo.description();
            manufacturer = serialPortInfo.manufacturer();
            serialNumber = serialPortInfo.serialNumber();
            out << endl
                << "Port: " << serialPortInfo.portName() << endl
                << "Location: " << serialPortInfo.systemLocation() << endl
                << "Description: " << (!description.isEmpty() ? description : blankString) << endl
                << "Manufacturer: " << (!manufacturer.isEmpty() ? manufacturer : blankString) << endl
                << "Serial number: " << (!serialNumber.isEmpty() ? serialNumber : blankString) << endl
                << "Vendor Identifier: " << (serialPortInfo.hasVendorIdentifier()
                                             ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16)
                                             : blankString) << endl
                << "Product Identifier: " << (serialPortInfo.hasProductIdentifier()
                                              ? QByteArray::number(serialPortInfo.productIdentifier(), 16)
                                              : blankString) << endl
                << "Busy: " << (serialPortInfo.isBusy() ? "Yes" : "No") << endl;
        }
    }
