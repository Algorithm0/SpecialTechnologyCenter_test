#pragma once

#include <QAbstractListModel>

class AudioNote;

class AudioNotesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AudioNotesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addIfNotExists(AudioNote* note);

public slots:
    void removeNote();

public:
    QVector<AudioNote*> m_items;
};

