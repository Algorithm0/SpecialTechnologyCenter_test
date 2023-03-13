#include "AudioNotePlayback.h"

#include <QBuffer>
#include <utility>

AudioNotePlayback::AudioNotePlayback(QByteArray content, QObject* parent)
  : QMediaPlayer(parent, QMediaPlayer::StreamPlayback),
    m_content(std::move(content)),
    playbackBuffer(new QBuffer(&m_content, this))
{}

const QByteArray& AudioNotePlayback::content() const
{
  return m_content;
}

void AudioNotePlayback::play()
{
  switch (state())
  {
    case QMediaPlayer::PlayingState:
      return;

    case QMediaPlayer::PausedState:
    {
      QMediaPlayer::play();
      break;
    }

    case QMediaPlayer::StoppedState:
    {
      playbackBuffer->open(QIODevice::ReadOnly);
      setMedia(QMediaContent(), playbackBuffer);
      QMediaPlayer::play();
      break;
    }
  }
}

void AudioNotePlayback::stop()
{
  if (state() == QMediaPlayer::StoppedState)
    return;

  QMediaPlayer::stop();
  playbackBuffer->close();
}
