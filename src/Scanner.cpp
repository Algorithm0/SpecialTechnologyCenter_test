#include "Scanner.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QDir>
#include <QFileInfo>

Scanner::Scanner(QObject* parent) : QObject(parent) {}

QStringList Scanner::threadScan(const QString& path)
{
  QStringList result;
  QDir dir(path);
  auto&& fileInfoList = dir.entryInfoList({"*.audionote"}, QDir::Files | QDir::NoDotAndDotDot);

  for (auto&& note : fileInfoList)
    result.append(note.filePath());

  fileInfoList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

  for (auto&& dir : fileInfoList)
    result.append(threadScan(dir.filePath()));

  return result;
}

void Scanner::scanFolder(const QString& path)
{
  auto* watcher = new QFutureWatcher<QStringList>();
  watcher->setFuture(QtConcurrent::run(Scanner::threadScan, path));
  connect(watcher, &QFutureWatcher<QStringList>::finished, this, [path, watcher, this]()
  {
    emit scanComplete(path, watcher->result());
    delete watcher;
  });
}


