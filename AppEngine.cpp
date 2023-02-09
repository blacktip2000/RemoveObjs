#include <QDebug>
#include <QtConcurrent>
#include "AppEngine.h"

AppEngine::AppEngine(QObject *parent)
    : QObject{parent}
{
    _timer.setInterval(500);
    _timer.setSingleShot(false);
    connect(&_timer, &QTimer::timeout, this, [=](){
        qInfo();
    });
    connect(this, &AppEngine::startThread, this, [=](){
        _timer.start();
    });
    connect(this, &AppEngine::stopThread, this, [=](){
        _timer.stop();
        qDebug() << "* DONE";
    });
//    QByteArray abc = "1123\\n\\r1122";
//    QTextStream st(&abc);
//    qDebug() << st.readLine();
//    qDebug() << st.readLine();
//    qDebug() << st.readLine();
//    qDebug() << st.readLine();
//    qDebug() << QString::fromStdString(abc.toStdString().c_str()).toLocal8Bit().data();

}

void AppEngine::runDelete(QDir path)
{
    try {
        if(path.exists() == false)
            throw QString("not found : %1").arg(path.absolutePath());

        QtConcurrent::run([=]{
            emit startThread();
            doClean(path);
            emit stopThread();
        });

    } catch (QString dbg) {
        qCritical() << dbg;
    }
}

bool AppEngine::isRunning()
{
    return _timer.isActive();
}

void AppEngine::doClean(QDir path)
{
    const QString targetQT = "Makefile";
    const QString targetFlutter = "pubspec.yaml";
    const QString targetReact = "package.json";

    QFileInfoList entrys = path.entryInfoList(QDir::Files | QDir::NoDot | QDir::NoDotDot);
    if(entrys.contains(QFileInfo(path, targetQT))) {
        qDebug() << "* Found(QT):" << path.absolutePath();
        doCleanUp(path, "mingw32-make.exe", {"clean", "-j12"});
    }
    if(entrys.contains(QFileInfo(path, targetFlutter))) {
        qDebug() << "* Found(Flutter):" << path.absolutePath();
        doCleanUp(path, "flutter", {"clean"});
    }
    if(entrys.contains(QFileInfo(path, targetReact))) {
        qDebug() << "* Found(React):" << path.absolutePath();
        doCleanUp(path, "watchman", {"watch-del-all"}, false);
#ifdef Q_OS_WIN

        doCleanUp(path, "rm", {"-rf","yarn.lock", "package-lock.json", "node_modules"}, false);
        doCleanUp(path, "rm", {"-rf","android/app/build"}, false);
        doCleanUp(path, "rm", {"ios/Pods", "ios/Podfile.lock"}, false);
        doCleanUp(path, "rm", {"-rf","~/Library/Developer/Xcode/DerivedData"}, false);
#else
        doCleanUp(path, "rm", {"-rf","yarn.lock", "package-lock.json", "node_modules"}, false);
        doCleanUp(path, "rm", {"-rf","android/app/build"}, false);
        doCleanUp(path, "rm", {"ios/Pods", "ios/Podfile.lock"}, false);
        doCleanUp(path, "rm", {"-rf","~/Library/Developer/Xcode/DerivedData"}, false);
#endif
    }

    entrys = path.entryInfoList(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    foreach (QFileInfo entry, entrys) {
        doClean(entry.absoluteFilePath());
    }
}

void AppEngine::doCleanUp(QDir path, QString program, QStringList args, bool isErrorPrint)
{
    QProcess _cleanProcess;
    _cleanProcess.setWorkingDirectory(path.absolutePath());
    _cleanProcess.setProgram(program);
    _cleanProcess.setArguments(args);
    _cleanProcess.setReadChannel(QProcess::StandardOutput);
    connect(&_cleanProcess, &QProcess::readyReadStandardOutput, [&_cleanProcess](){
        QByteArray buf = _cleanProcess.readAllStandardOutput();
        QTextStream stream(buf);
        while(stream.atEnd() == false)
            qDebug() << stream.readLine().toLocal8Bit().data();
    });
    if(isErrorPrint) {
        _cleanProcess.setReadChannel(QProcess::StandardError);
        connect(&_cleanProcess, &QProcess::readyReadStandardError, [&_cleanProcess](){
            QByteArray buf = _cleanProcess.readAllStandardError();
            QTextStream stream(buf);
            while(stream.atEnd() == false)
                qCritical() << stream.readLine().toLocal8Bit().data();
        });
    }

    _cleanProcess.start();
    _cleanProcess.waitForFinished(-1);
}
