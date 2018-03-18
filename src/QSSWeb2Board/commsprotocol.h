#ifndef COMMSPROTOCOL_H
#define COMMSPROTOCOL_H

#include <QPair>
#include <QString>

class CommsProtocol
{
public:
    CommsProtocol();

    //commands dicitionary
    static const QPair<QString,QString>  VERIFY;
    static const QPair<QString,QString>  UPLOAD;
    static const QPair<QString,QString>  SKETCH;
    static const QPair<QString,QString>  BOARDID;

};

#endif // COMMSPROTOCOL_H
