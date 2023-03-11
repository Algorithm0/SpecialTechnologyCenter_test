#include "AudioNotesModel.h"
#include "AudioNote.h"
#include <QFile>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

AudioNotesModel::AudioNotesModel(QObject* parent)
  : QAbstractListModel{parent}
{
}

int AudioNotesModel::rowCount(const QModelIndex&) const
{
  return m_items.size();
}

QVariant AudioNotesModel::data(const QModelIndex& index, int ) const
{
  return QVariant::fromValue(m_items[index.row()]);
}

QHash<int, QByteArray> AudioNotesModel::roleNames() const
{
  static QHash<int, QByteArray> sNames = { { 1, "note" }};
  return sNames;
}

void AudioNotesModel::addIfNotExists(AudioNote* note)
{
  if (note == nullptr)
    return;

  auto&& item = std::find_if(m_items.cbegin(), m_items.cend(), [path = note->path()](auto&& item)
  {
    return (item != nullptr) && (item->path() == path);
  });

  if (item != m_items.cend())
    return;

  connect(note, &AudioNote::removeNode, this, &AudioNotesModel::removeNote);
  beginInsertRows(QModelIndex(), 0, 0);
  m_items.prepend(note);
  endInsertRows();
}

void AudioNotesModel::removeNote()
{
  auto*&& object = qobject_cast<AudioNote*>(sender());

  if (object == nullptr)
    return;

  int index = -1;

  for (auto&& item : m_items)
  {
    ++index;

    if (object == item)
      break;
  }

  if (index != -1)
  {
    beginRemoveRows(QModelIndex(), index, index);
    m_items.remove(index);
    endRemoveRows();
    object->deleteLater();
    const QString path = object->path();
    QtConcurrent::run([path]()
    {
      QFile file (path);
      constexpr char attemptsCount = 5;

      for (char i = 0; !file.remove() && i < attemptsCount; ++i);
    });
  }
}
