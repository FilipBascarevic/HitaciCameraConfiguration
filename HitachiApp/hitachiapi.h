#ifndef HITACHIAPI_H
#define HITACHIAPI_H

#include "serial.h"
#include <QDebug>

// This enum represents direction in methods
enum DIRECTION {READ, WRITE};

class hitachiAPI: public Serial {

private:
    char transmit_buff[18] = {};
    char receive_buff[10] = {};

    bool extractData(char *data);
    void insertData(char *data, DIRECTION dir);
    void calculateCheckSum(char *data, unsigned int len);
    bool checkCheckSum(char *data, unsigned int len);

public:

    // Camera control ( KP-D protocol )
    void LightControlMode (DIRECTION dir,QString &mode);

    // LENS control ( Housing protocol )
    void ZoomPositionSet (quint16 &value);
    void ZoomPositionGet (quint16 &value);


};

#endif // HITACHIAPI_H
