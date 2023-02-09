#ifndef APPENGINE_H
#define APPENGINE_H

#include <QObject>
#include <QDir>
#include <QTimer>
#include <QProcess>

class AppEngine : public QObject
{
    Q_OBJECT
public:
    explicit AppEngine(QObject *parent = nullptr);

    void runDelete(QDir path);
    bool isRunning();

private:
    QTimer _timer;
    static void doClean(QDir path);
    static void doCleanUp(QDir path, QString program, QStringList args, bool isErrorPrint = true);

signals:
    void startThread();
    void stopThread();
};

#endif // APPENGINE_H
