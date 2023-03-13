#pragma once

#include <QMediaPlayer>

class QBuffer;

class AudioNotePlayback : public QMediaPlayer
{
  Q_OBJECT

public:
  explicit AudioNotePlayback(QByteArray content = {}, QObject* parent = nullptr);

  const QByteArray& content() const;

public slots:
  void play();
  void stop();

private:
  QByteArray m_content;
  QBuffer* playbackBuffer;
};

