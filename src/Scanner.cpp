#include "Scanner.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QDirIterator>

Scanner::Scanner(QObject* parent) : QObject(parent) {}

QStringList Scanner::threadScan(const QString& path)
{
  QStringList result;
  QDirIterator dir(path, {"*.audionote"}, QDir::Files, QDirIterator::Subdirectories);

  if (!dir.hasNext())
    return result;

  result.append(dir.next());

  while (dir.hasNext())
  {
    const QString findFile(dir.next());
    auto it = result.begin();

    if (*it > findFile)
    {
      result.append(findFile);
      continue;
    }

    ++it;

    for (; it != result.end(); ++it)
    {
      if (*it > findFile)
      {
        result.insert(--it, findFile);
        break;
      }
    }

    if (it == result.end())
      result.prepend(findFile);
  }

  return result;
}

void Scanner::scanFolder(const QString& path)
{
  auto* watcher = new QFutureWatcher<QStringList>(this);
  watcher->setFuture(QtConcurrent::run(Scanner::threadScan, path));
  connect(watcher, &QFutureWatcher<QStringList>::finished, this, [path, watcher, this]()
  {
    emit scanComplete(path, watcher->result());
    delete watcher;
  });
}
