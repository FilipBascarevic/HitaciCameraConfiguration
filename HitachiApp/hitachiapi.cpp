#include "hitachiapi.h"

bool hitachiAPI::extractData(char *data) {

    bool valid_message = true;
    //char data[6] = {};

    // Check STX
    if (receive_buff[0] != 0x02) {
        valid_message = false;
    }
    // Check ETX
    if (receive_buff[7] != 0x03) {
        valid_message = false;
    }
    // Check Check Sum
    valid_message = checkCheckSum(receive_buff, 10);


    // Next 6 bytes are received data
    data = &receive_buff[1];

    return valid_message;


}

void hitachiAPI::calculateCheckSum(char *data, unsigned int len) {

    unsigned int sum = 0x0;

    for (unsigned int i = 0; i < len-2; i++) {
        if ((i == 0) || (i==len-3))
            sum += data[i];
        else
            sum += data[i] + 0x30;
    }

    // Do XOR with sum
    sum ^= 0xFF;

    // Set last two bits in data
    data[len-2] = (char) ((sum & 0xF0) >> 4);
    data[len-1] = (char) (sum & 0x0F);

    return;

}

bool hitachiAPI::checkCheckSum(char *data, unsigned int len) {

    unsigned int sum = 0x0;
    bool valid_checkSum = true;

    for (unsigned int i = 0; i < len-2; i++) {
        if ((i == 0) || (i==len-3))
            sum += data[i];
        else
            sum += data[i] + 0x30;
    }

    // Do XOR with sum
    sum ^= 0xFF;

    // Check last two bits in data
    if (data[len-2] != (char) ((sum & 0xF0) >> 4))
        valid_checkSum = false;

    if (data[len-1] != (char) (sum & 0x0F))
        valid_checkSum = false;

    return valid_checkSum;

}

void hitachiAPI::insertData(char *data, DIRECTION dir){
    // Fill transmit buffer and send command
    // STX = 0x02
    transmit_buff[0] = 0x02;
    // Fill Text
    // Status
    transmit_buff[1] = 0x00;
    transmit_buff[2] = 0x01;
    // ID no.
    transmit_buff[3] = 0x00;
    transmit_buff[4] = 0xFF;
    // Area Address
    if (dir == WRITE) {
        transmit_buff[5] = 0x00;
        transmit_buff[6] = 0x01;
    }
    else {
        transmit_buff[5] = 0x08;
        transmit_buff[6] = 0x01;
    }

    // Relative no.
    transmit_buff[7] = 0x00;
    transmit_buff[8] = 0x1B;

    // Fill data
    transmit_buff[9] = data[0];
    transmit_buff[10] = data[1];
    transmit_buff[11] = data[2];
    transmit_buff[12] = data[3];
    transmit_buff[13] = data[4];
    transmit_buff[14] = data[5];


    // ETX
    transmit_buff[15] = 0x03;
    // Add for check sum calculation
    calculateCheckSum(transmit_buff, 18);
}

void hitachiAPI::LightControlMode(DIRECTION dir, QString &mode) {

    // send ENQ and wait ACK. If after 4 ENG 4 NAK is occurred
    // control abort process is occurred
    char enq = 0x05;
    char ack = 0x06;
    char resp = 0x00;
    char data[6] = {};
    qint64 read_write_bytes = 0;
    int counter = 0;
    while (counter != 4) {
        // Send ENQ
        read_write_bytes = write(&enq, 1);
        if (read_write_bytes == 0) {
            qFatal("ENQ isn't send successfully!!!");
        }

        // wait ACK or NAK. If NAK is occurred, repete this process
        read_write_bytes = read(&resp, 1, 1000);
        if (read_write_bytes == 0) {
            qFatal("ACK or NAK aren't received successfully!!!");
        }
        // Received ACK
        if (resp == 0x06) {
            break;
        }
        // Received NAK
        else if (resp == 0x15) {
            counter++;
        }

    }

    if (counter == 4) {
        qFatal("Aborting request!!!");
    }


    counter = 0;
    while(counter != 4) {

        // Data
        data[2] = 0x00;
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        if (mode == "AVERAGE") {
            data[0] = 0x00;
            data[1] = 0x00;
        }
        else if (mode == "BLC") {
            data[0] = 0x00;
            data[1] = 0x01;
        }
        else if (mode == "PEAK/AVE") {
            data[0] = 0x00;
            data[1] = 0x02;
        }
        else {
            data[0] = 0x00;
            data[1] = 0x00;
        }

        insertData(data, dir);

        // Send data
        read_write_bytes = write(transmit_buff, 18);
        if (read_write_bytes != 18) {
            qFatal("Transmitted message isn't sent successfully!!!");
        }

        // Wait ACK 3s
        read_write_bytes = read(&resp, 1, 3000);
        if (read_write_bytes == 0) {
            counter++;
        }
        else {
            // Received ACK
            if (resp == 0x06) {
                break;
            }
        }
    }


    if (counter == 4) {
        qFatal("Data frame error!!!");
    }

    // If DIRECTION is READ, wait answered data

    if (dir == READ) {
        counter = 0;

        while (counter != 4) {

            // Wait data 3s
            read_write_bytes = read(receive_buff, 10, 3000);

            if (read_write_bytes != 10) {
                qFatal("Received message isn't received successfully!!!");
            }

            // Check message. If message is valid, generate ack
            bool is_valid = extractData(data);

            if (is_valid) {
                // AVERAGE mode
                if (data[1] == 0x00) {
                    mode = "AVERAGE";
                }
                // BLC mode
                else if (data[1] == 0x01) {
                    mode = "BLC";
                }
                else if (data[1] == 0x02) {
                    mode = "PEAK/AVE";
                }
                else {
                   mode = "UNKNOWN";
                }

                // Generate ACK
                read_write_bytes = write(&ack, 1);
                if (read_write_bytes == 0) {
                    qFatal("ACK isn't send successfully!!!");
                }

            }
            else {
                counter++;
            }


        }
        if (counter == 4) {
            qFatal("Transmission frame error!!!");
        }

    }

}

void hitachiAPI::ZoomPositionSet(quint16 &value) {


    qint64 read_write_bytes = 0;

    char command[9] = {};
    char response[4] = {};

    // command message for transmitting
    command[0] = 0x02;
    command[1] = '0';
    command[2] = 'z';
    command[3] = 'a';
    // convert number to HEX ASCII
    quint16 valueShift = static_cast <quint16> (value << 6);
    quint16 mask = 0x000F;
    quint16 number = 0;
    // Convert 16bit number into HEX ASCII :D
    for (int i = 0; i < 4; i++) {
        number = valueShift & mask;
        number = number >> (i*4);
        switch (number) {
            case 0: command[7-i] = '0';
                break;
            case 1: command[7-i] = '1';
                break;
            case 2: command[7-i] = '2';
                break;
            case 3: command[7-i] = '3';
                break;
            case 4: command[7-i] = '4';
                break;
            case 5: command[7-i] = '5';
                break;
            case 6: command[7-i] = '6';
                break;
            case 7: command[7-i] = '7';
                break;
            case 8: command[7-i] = '8';
                break;
            case 9: command[7-i] = '9';
                break;
            case 10: command[7-i] = 'A';
                break;
            case 11: command[7-i] = 'B';
                break;
            case 12: command[7-i] = 'C';
                break;
            case 13: command[7-i] = 'D';
                break;
            case 14: command[7-i] = 'E';
                break;
            case 15: command[7-i] = 'F';
                break;
        }
        mask = static_cast <quint16> (mask << 4);
    }

    command[8] = 0x03;

    // send command
    read_write_bytes = write(command, 9);
    if (read_write_bytes != 9) {
        qFatal("Command isn't sent successfully!!!");
    }

    // replay
    read_write_bytes = read(response, 4, 3000);
    if (read_write_bytes != 4) {
        qFatal("Not enought caracters is received or timeout is occurred!!!");
    }

    // Check response
    // STX
    if (response[0] != 0x02) {
        qDebug("Bed response, STX");
    }
    // CAM ID
    if (response[1] != '0') {
        qDebug("Bed response, CAM ID");
    }
    // ACK
    if (response[2] != 0x06) {
        qDebug("Bed response, ACK");
    }
    // ETX
    if (response[3] != 0x03) {
        qDebug("Bed response, ETX");
    }

}

void hitachiAPI::ZoomPositionGet(quint16 &value) {
    qint64 read_write_bytes = 0;

    char command[6] = {};
    char response[8] = {};

    // command message for transmitting
    command[0] = 0x02;
    command[1] = '0';
    command[2] = 'z';
    command[3] = 'a';
    command[4] = '?';
    command[5] = 0x03;

    // send command
    read_write_bytes = write(command, 6);
    if (read_write_bytes != 6) {
        qFatal("Command isn't sent successfully!!!");
    }

    // replay
    read_write_bytes = read(response, 8, 3000);
    if (read_write_bytes != 8) {
        qFatal("Not enought caracters is received or timeout is occurred!!!");
    }

    // Check response
    // STX
    if (response[0] != 0x02) {
        qDebug("Bed response, STX");
    }
    // CAM ID
    if (response[1] != '0') {
        qDebug("Bed response, CAM ID");
    }
    // ACK
    if (response[2] != 0x06) {
        qDebug("Bed response, ACK");
    }
    // ETX
    if (response[7] != 0x03) {
        qDebug("Bed response, ETX");
    }

    // extract data from response
    value = 0;
    // convert HEX ASCII to number
    quint16 mask = 0;
    // Convert 16bit number into HEX ASCII :D
    for (int i = 0; i < 4; i++) {
        switch (response[3+i]) {
            case '0': mask = 0;
                break;
            case '1': mask = 1;
                break;
            case '2': mask = 2;
                break;
            case '3': mask = 3;
                break;
            case '4': mask = 4;
                break;
            case '5': mask = 5;
                break;
            case '6': mask = 6;
                break;
            case '7': mask = 7;
                break;
            case '8': mask = 8;
                break;
            case '9': mask = 9;
                break;
            case 'A': mask = 10;
                break;
            case 'B': mask = 11;
                break;
            case 'C': mask = 12;
                break;
            case 'D': mask = 13;
                break;
            case 'E': mask = 14;
                break;
            case 'F': mask = 15;
                break;
        }
        mask = static_cast <quint16> (mask << 4*(3-i));
        value = value | mask;
    }
}
