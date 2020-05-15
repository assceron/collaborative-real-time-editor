#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include "MainServer.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Text Editor Server");
    parser.addHelpOption();

    QCommandLineOption dbgOption(QStringList() << "d" << "debug",
                                 QCoreApplication::translate("main", "Debug output [default: off]."));
    parser.addOption(dbgOption);
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                  QCoreApplication::translate("main", "Port for server [default: 1234]."),
                                  QCoreApplication::translate("main", "port"), QLatin1Literal("1234"));
    parser.addOption(portOption);
    parser.process(app);

    bool debug = parser.isSet(dbgOption);
    int port = parser.value(portOption).toInt();

    MainServer *server = new MainServer(port, debug);

    return app.exec();
}