#include "Scanner.h"
#include <mutex>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

Scanner::Scanner(QObject *parent)
    : QObject{parent}
{

}

void Scanner::scanFolder(QString path)
{
    //чтобы было потокобезопасно нужно сделать мютекс
    std::mutex mutex;

    numThreadsScanningFolder[path] = QThread::idealThreadCount();
    //чтобы все было очень быстро и не тормозил интерфейс - нужно сканировать в отдельном потоке, а лучше в нескольких!
    for(int i = 0; i < QThread::idealThreadCount(); i++) {
        mutex.lock();
        //недавно узнал про офигенно удобную вещь в std - позволяет создавать shared_pointer из this. Теперь постоянно применяю
        ScannerThread* thread = new ScannerThread(shared_from_this());
        connect(thread, &ScannerThread::ready, this, &Scanner::onScanComplete);
        thread->scanFolder(path);
        mutex.unlock();
    }
}

void Scanner::onScanComplete()
{
    auto &&scannerThread = dynamic_cast<ScannerThread*>(sender());
    if(!scannerThread){
        return;
    }
    auto &&folder = scannerThread->foler_;
    for(auto &&item : scannerThread->result){
        if(!folderNotes[folder].contains(item)) {
            folderNotes[folder].append(item);
        }
    }
    numThreadsScanningFolder[folder] = numThreadsScanningFolder[folder] - 1;

    //сообщить о прогрессе загрузки
    emit scanProgress(numThreadsScanningFolder[folder], QThread::idealThreadCount());

    //и если все потоки отработали - сигнал о результате
    if(numThreadsScanningFolder[folder] == 0) {
        emit scanComplete(folder, folderNotes[folder]);
    }
}

ScannerThread::ScannerThread(std::shared_ptr<Scanner> scanner)  {
    scanner_ = scanner;
}

void ScannerThread::scanFolder(const QString &folder){
    foler_ = folder;
    run();
}

void ScannerThread::scanDir(QString dir) {
    QDir folder_Dir(dir);
    auto &&folder_Entries = folder_Dir.entryInfoList();
    for(auto &&entry: folder_Entries) {
        if(!(entry.fileName() == ".") && entry.fileName() != "..") {
            if(entry.isDir()) {
                scanDir(entry.filePath());
            }
            if(entry.isFile() && (entry.completeSuffix() == "audionote")) {
                result.append(entry.filePath());
            }
        }
    }
}

void ScannerThread::run() {
    scanDir(foler_);
    emit ready();
}
