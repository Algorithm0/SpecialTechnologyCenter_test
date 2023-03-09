#pragma once

#include <QObject>
#include <QStringList>
#include <memory>
#include <QHash>
#include <QThread>


class Scanner;

class ScannerThread: public QThread {
    Q_OBJECT
public:
    std::shared_ptr<Scanner> scanner_;
    QString foler_;
    QStringList result;

    //нужно чтобы scanner оставался валидным если поток закончит работу. Безопасность памяти - наше все
    ScannerThread(std::shared_ptr<Scanner> scanner);
    void scanFolder(const QString & folder);
    // QThread interface
protected:
    void scanDir(QString dir);
    virtual void run() override;
signals:
    void ready();
};


class Scanner : public QObject, public std::enable_shared_from_this<Scanner>
{
    Q_OBJECT

    QHash<QString, int> numThreadsScanningFolder;
    QHash<QString, QStringList> folderNotes;
public:
    explicit Scanner(QObject *parent = nullptr);
public slots:
    void scanFolder(QString path);

    void onScanComplete();
signals:
    void scanProgress(int scanned, int total);
    void scanComplete(QString path, QStringList audioNotes);
};

