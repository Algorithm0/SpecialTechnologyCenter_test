#pragma once

#include <QObject>
#include <QFutureWatcher>

class QStringList;

class Scanner : public QObject
{
  Q_OBJECT

public:
  explicit Scanner(QObject* parent = nullptr);

public slots:
  void scanFolder(const QString& path);

signals:
  void scanComplete(const QString& path, const QStringList& audioNotes);

private:
  static QStringList threadScan(const QString& path);
};

