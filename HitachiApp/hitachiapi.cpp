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
    // Add here code


    // Next 6 bytes are received data
    data = &receive_buff[1];

    return valid_message;


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
