#include "AudioNotesModel.h"
#include "AudioNote.h"
#include <QFile>
#include <QDebug>

AudioNotesModel::AudioNotesModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

int AudioNotesModel::rowCount(const QModelIndex &) const
{
    return m_items.size();
}

QVariant AudioNotesModel::data(const QModelIndex &index, int ) const
{
    return QVariant::fromValue(m_items[index.row()]);
}

QHash<int, QByteArray> AudioNotesModel::roleNames() const
{
    static QHash<int, QByteArray> sNames = { { 1, "note" }};
    return sNames;
}

void AudioNotesModel::addIfNotExists(AudioNote *note)
{
    if(note){
        auto &&item = std::find_if(m_items.begin(), m_items.end(), [path = note->path()](auto &&item){
            return item && item->path() == path;
        });
        if(item != m_items.end()){
            return;
        }
        connect(note, &AudioNote::removeNode, this, &AudioNotesModel::removeNote);
        beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
        m_items.append(note);
        endInsertRows();
    }
}

void AudioNotesModel::removeNote()
{
    if(auto &&object = qobject_cast<AudioNote*>(sender())){
        int index = -1;
        for(auto &&item : m_items){
            ++index;
            if(object == item){
                break;
            }
        }
        if(index != -1){
            beginRemoveRows(QModelIndex(), index, index);
            m_items.remove(index);
            endRemoveRows();
            object->deleteLater();
           // QFile file (object->path());
           // file.remove();
        }
    }
}
