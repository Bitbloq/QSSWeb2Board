#include "arduinoexceptions.h"

QString VerifyException::getErrorsLists(){
    QString msg="";
    for(int i=0;i<errorsList.size();i++){
        msg+=errorsList.at(i);
    }
    return msg;
}
