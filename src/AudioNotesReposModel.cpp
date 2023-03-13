#include "AudioNotesReposModel.h"
#include "AudioNotesRepo.h"
#include <QUrl>
#include <QDebug>
#include <utility>

AudioNotesReposModel::AudioNotesReposModel(QObject* parent)
  : QAbstractListModel{parent}
{
}

int AudioNotesReposModel::rowCount(const QModelIndex&) const
{
  return static_cast<int>(m_items.size());
}

QVariant AudioNotesReposModel::data(const QModelIndex& index, int ) const
{
  return QVariant::fromValue(m_items[index.row()].get());
}

QHash<int, QByteArray> AudioNotesReposModel::roleNames() const
{
  static QHash<int, QByteArray> sNames = { { 1, "repo" }};
  return sNames;
}

void AudioNotesReposModel::addRepo(const QUrl& path)
{
  auto&& newRepo = new AudioNotesRepo(path.toLocalFile(), this);
  newRepo->init();
  addRepo(newRepo);
}

void AudioNotesReposModel::addRepo(QString path)
{
  auto*&& newRepo = new AudioNotesRepo(std::move(path), this);
  newRepo->init();
  addRepo(newRepo);
}

void AudioNotesReposModel::addRepo(AudioNotesRepo* repo)
{
  if (repo == nullptr)
    return;

  const bool alreadyExsist = std::any_of(m_items.cbegin(), m_items.cend(), [ repo ](auto&& elem)
  {
    return elem->path().compare(repo->path()) == 0;
  });

  if (alreadyExsist)
    return;

  beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
  m_items.emplace_back(repo);
  endInsertRows();
}
