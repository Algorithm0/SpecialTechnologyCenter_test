#include "AudioNotePlayback.h"

#include <QMediaPlayer>
#include <QBuffer>

AudioNotePlayback::AudioNotePlayback(QByteArray content, QObject *parent)
    : QObject{parent},
      m_content(content),
      player(nullptr),
      playbackBuffer(nullptr)
{

}

qint64 AudioNotePlayback::postition() const
{
    return m_postition;
}

void AudioNotePlayback::position(qint64 newPostition)
{
    if (m_postition == newPostition)
        return;
    m_postition = newPostition;
    emit positionChanged();
}

qint64 AudioNotePlayback::duration() const
{
    return m_duration;
}

const QByteArray &AudioNotePlayback::content() const
{
    return m_content;
}

void AudioNotePlayback::play()
{
    if(player){
        return;
    }
    playbackBuffer = new QBuffer(&m_content, this);
    playbackBuffer->open(QIODevice::ReadOnly);
    player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    player->setMedia(QMediaContent(), playbackBuffer);
    connect(player, &QMediaPlayer::stateChanged, this, [this](QMediaPlayer::State state){
        if(state == QMediaPlayer::StoppedState){
            player->deleteLater();
            playbackBuffer->deleteLater();
            player = nullptr;
            playbackBuffer = nullptr;
            emit activeChanged();
        }
    });
    connect(player, &QMediaPlayer::positionChanged, this, [this](){
        m_postition = player->position();
        emit positionChanged();

    });
    connect(player, &QMediaPlayer::durationChanged, this, [this](){
        m_duration = player->duration();
        emit durationChanged();
    });
    player->play();
    emit activeChanged();
}

void AudioNotePlayback::stop()
{
    if(player){
        player->stop();
        player->deleteLater();
        playbackBuffer->deleteLater();
        player = nullptr;
        playbackBuffer = nullptr;
    }
    emit activeChanged();
}

bool AudioNotePlayback::active() const
{
    return player != nullptr;
}
