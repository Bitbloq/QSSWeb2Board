# QSSWeb2Board
QT Secure Web Socket for compiling and loading online generated Arduino code into Arduino .

# How to Build
## Dependencies
  - qt5-qmake 
  - qt5-default 
  - libqt5websockets5-dev 
  - libqt5serialport5-dev 
  - build-essential
  
  SSL Certificates for web2board.es are required. If you don't have them use self created localhost ssl certificates and adjust sslserver.cpp lines 36 and 37:
  
    QFile certFile(QStringLiteral(":/res/www.web2board.es.crt"));
    QFile keyFile(QStringLiteral(":/res/www.web2board.es.key"));
  
  qmake QSSWeb2Board.pro
  make
