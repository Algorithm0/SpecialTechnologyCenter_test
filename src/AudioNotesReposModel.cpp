#include "AudioNotesReposModel.h"
#include "AudioNotesRepo.h"
#include <QUrl>
#include <QDebug>

AudioNotesReposModel::AudioNotesReposModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

int AudioNotesReposModel::rowCount(const QModelIndex &) const
{
    return m_items.size();
}

QVariant AudioNotesReposModel::data(const QModelIndex &index, int ) const
{
    return QVariant::fromValue(m_items[index.row()].get());
}

QHash<int, QByteArray> AudioNotesReposModel::roleNames() const
{
    static QHash<int, QByteArray> sNames = { { 1, "repo" }};
    return sNames;
}

void AudioNotesReposModel::addRepo(const QUrl &path)
{
    auto &&newRepo = new AudioNotesRepo(path.toLocalFile(), this);
    newRepo->init();
    addRepo(newRepo);
}

void AudioNotesReposModel::addRepo(QString path)
{
    auto &&newRepo = new AudioNotesRepo(path, this);
    newRepo->init();
    addRepo(newRepo);
}


void AudioNotesReposModel::addRepo(AudioNotesRepo *repo)
{
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items.emplace_back(repo);
    endInsertRows();
}
