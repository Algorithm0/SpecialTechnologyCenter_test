#pragma once

#include <QAbstractListModel>

class AudioNote;

class AudioNotesModel : public QAbstractListModel
{
  Q_OBJECT
public:
  explicit AudioNotesModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex&) const final;
  QVariant data(const QModelIndex& index, int) const final;
  QHash<int, QByteArray> roleNames() const final;

  void addIfNotExists(AudioNote* note);

public slots:
  void removeNote();

public:
  QVector<AudioNote*> m_items;
};

