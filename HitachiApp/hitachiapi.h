#ifndef HITACHIAPI_H
#define HITACHIAPI_H

#include "serial.h"

// This enum represents direction in methods
enum DIRECTION {READ, WRITE};

class hitachiAPI: public Serial {

private:
    char transmit_buff[18] = {};
    char receive_buff[10] = {};

    bool extractData(char *data);
    void insertData(char *data, DIRECTION dir);

public:
    void LightControlMode (DIRECTION dir,QString &mode);


};

#endif // HITACHIAPI_H
