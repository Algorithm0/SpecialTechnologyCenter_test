#include "AudioNotesApp.h"

#include "AudioNotesRepo.h"
#include "AudioNotesReposModel.h"
#include <QFile>
#include <QTextStream>
#include <QUrl>

#include <QDebug>

AudioNotesApp::AudioNotesApp(QObject *parent)
    : QObject{parent}
{
    m_reposModel = new AudioNotesReposModel(this);
}

bool AudioNotesApp::emptyNotes() const
{
    return m_reposModel->rowCount(QModelIndex()) == 0;
}

void AudioNotesApp::init()
{
    urls_.clear();
    QFile file("init.txt");
    if (file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        while (!in.atEnd()){
            if(auto &&path = in.readLine(); path != ""){
                urls_.append(QUrl(path));
            }
        }
        file.close();
    }
    for(auto &&i : urls_){
        m_reposModel->addRepo(i.toLocalFile());
    }
    if(!emptyNotes()){
        emit updateWindow();
    }
}

void AudioNotesApp::save()
{
    QString filename = "init.txt";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        for(auto &&i : urls_){
            stream << i.toString() << endl;
        }
    }
    file.close();
}

void AudioNotesApp::addAudioRepo(const QUrl &path)
{
    m_reposModel->addRepo(path);
    urls_.append(path);
    save();
}

AudioNotesReposModel *AudioNotesApp::reposModel() const
{
    return m_reposModel;
}
