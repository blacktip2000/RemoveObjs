#include <QCoreApplication>
#include <QDebug>
#include "QConsoleListener"
#include "AppEngine.h"
const std::string gLoadingIcon[] = {"",".","..","...","....","....."};
//std::string gLoadingIcon[] = {"⠋","⠙","⠸","⠴","⠦","⠇"};
short gLoadingIndex = 0;


void printHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{

//    const char * msg = str.toLatin1().data();
    std::string msg = str.toStdString();
    switch (type) {
        case QtInfoMsg:
            fprintf(stderr, "\33[2K\rLoading%s", gLoadingIcon[gLoadingIndex].c_str());
            gLoadingIndex = (gLoadingIndex + 1) > 5 ? 0 : (gLoadingIndex + 1);
            break;
        case QtDebugMsg:
            fprintf(stderr, "\33[2K\r\033[1;33m%s\033[0m\n", msg.c_str());
            break;
        case QtWarningMsg:
            fprintf(stderr, "\33[2K\r\033[1;34m%s\033[0m\n", msg.c_str());
            break;
        case QtCriticalMsg:
            fprintf(stderr, "\33[2K\r\033[31m%s\033[0m\n", msg.c_str());
            break;
        case QtFatalMsg:
            fprintf(stderr, "\33[2K\r\033[31m%s\033[0m\n", msg.c_str());
            abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(printHandler);
    QCoreApplication a(argc, argv);
    QConsoleListener console;
    AppEngine appEngine;

    qDebug() << "********* DEL ALL OBJECT FILES *********";
    qDebug() << "* Support : Qt(mingw)/Flutter/React    *";
    qDebug() << "* Support : only 1 depth Dir           *";
    qDebug() << "********* DEL ALL OBJECT FILES *********";
    qDebug();
    qWarning() << "Input Target Dir(default: .): ";


    QObject::connect(&console, &QConsoleListener::newLine,
                     [&a, &appEngine](const QString &strNewLine) {

                        if(appEngine.isRunning())
                            return;

                        appEngine.runDelete(strNewLine);
                     });

    return a.exec();
}
