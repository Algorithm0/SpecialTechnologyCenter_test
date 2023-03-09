#pragma once

#include <QAbstractListModel>
#include <memory>

class AudioNotesRepo;
class QUrl;

class AudioNotesReposModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AudioNotesReposModel(QObject *parent = nullptr);
    ~AudioNotesReposModel() = default;

    int rowCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addRepo(const QUrl &path);
    void addRepo(QString path);

private:
    std::vector<std::unique_ptr<AudioNotesRepo>> m_items;

    void addRepo(AudioNotesRepo* repo);
};

