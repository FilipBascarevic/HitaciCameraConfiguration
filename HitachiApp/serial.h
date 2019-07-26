/** Serial.h
 *
 * A very simple serial port control class. This calss contains opening/closing serial port.
 * Reading / Writing form/to serial port. Flush for flushing serial port.
 *
 * @author Filip Bascarevic
 *
 * @version 0.1 -- 30 April 2019
 */

#ifndef SERIAL_H
#define SERIAL_H

    #include <QStringList>
    #include<QTextStream>
    #include<QtSerialPort/qserialport.h>
    #include<QtSerialPort/qserialportinfo.h>

    class Serial {

    public:
        QSerialPort::BaudRate BaudRate;
        QSerialPort::StopBits StopBits;
        QSerialPort::Parity Parity;
        QSerialPort::DataBits DataBits;

    private :
        QSerialPort serialPort;
        QTextStream standardOutput;
    public :

        Serial() : standardOutput(stdout)
        {
            BaudRate = QSerialPort::Baud9600;
            DataBits = QSerialPort::Data8;
            StopBits = QSerialPort::OneStop;
            Parity   = QSerialPort::NoParity;
        }

        virtual ~Serial();

        /** Open the serial port for reading and writing.
        *
        * @param name of serial port
        * @param BaudRate
        * @param DataBits
        * @param StopBits
        * @param Parity
        *
        * @return boolean is serial port opened correctly
        */
        bool open(QString serialPortName, QSerialPort::BaudRate serialPortBaudRate = QSerialPort::Baud9600, QSerialPort::DataBits serialPortDataBits = QSerialPort::Data8,
            QSerialPort::StopBits serialPortStopBits = QSerialPort::OneStop, QSerialPort::Parity serialPortParity = QSerialPort::NoParity);


        /** Writes a string of bytes to the serial port.
        *
        * @param buffer pointer to the buffer containing the bytes
        * @param buffLen the number of bytes in the buffer
        *
        * @return int the number of bytes written
        */
        qint64 write(const char *buffer, qint64 buffLen);

        /** Reads a string of bytes from the serial port.
         *
        * @param buffer pointer to the buffer to be written to
        * @param buffLen the size of the buffer
        * @param timeOut timeout in msec. Default not timeout
        * @param nullTerminate if set to true it will null terminate the string
        *
        * @return int the number of bytes read
        */

        qint64 read(char *buffer, qint64 buffLen,int timeOut = -1, bool nullTerminate = false);

        /** Flushes everything from the serial port's read buffer
        */
        void flush();

        /** Close serial port
        */
        void close();

        friend void availableSerialPorts();

    };

    void availableSerialPorts();

#endif // SERIAL_H
