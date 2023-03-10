#include "AudioNote.h"

#include "AudioNotePlayback.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include <QBuffer>
#include <QAudioDecoder>


AudioNote::AudioNote(QObject* parent)
  : QObject{parent},
    m_playback(nullptr) {}

template < typename T>
typename std::enable_if < std::is_signed<T>::value&& std::is_integral<T>::value,
         AudioSample >::type getSample (const QAudioBuffer& buffer, int shift)
{
  auto* data = buffer.data<T>();
  return AudioSample
  {
    static_cast<double>(abs(*data) / static_cast<double>(std::numeric_limits<T>::max())),
    static_cast<int>(buffer.format().durationForBytes(shift * sizeof(T)))
  };
}

template < typename T>
typename std::enable_if < (std::is_unsigned<T>::value&& std::is_integral<T>::value) || std::is_floating_point<T>::value,
         AudioSample >::type getSample (const QAudioBuffer& buffer, int shift)
{
  auto* data = buffer.data<T>();
  return AudioSample
  {
    static_cast<double>(*data / static_cast<double>(std::numeric_limits<T>::max())),
    static_cast<int>(buffer.format().durationForBytes(shift * sizeof(T)))
  };
}



void AudioNote::init()
{
  QFile f(m_path);
  f.open(QFile::ReadWrite);
  QByteArray header = f.readLine();
  auto object = QJsonDocument::fromJson(header).object();
  setName(object.value("name").toString());
  setColor(object.value("color").toString());
  setEncrypted(object.value("emcrypted").toBool());
  setPassword(object.value("password").toString());
  QByteArray content = f.readAll();
  m_playback = new AudioNotePlayback(content, this);
  //в qt своя модель памяти поэтому всегда нужно задавать parent
  auto* audioDecoder = new QAudioDecoder(this);
  audioDecoder->setSourceDevice(new QBuffer(&content, this));
  audioDecoder->start();
  connect(audioDecoder, &QAudioDecoder::finished, this, [ = ]()
  {
    std::vector<AudioSample> audioSamples;
    auto buffer = audioDecoder->read();
    const int sampleSize = buffer.format().sampleSize();

    for (int i = 0; i < buffer.sampleCount(); ++i)
    {
      const auto type = buffer.format().sampleType();

      switch (type)
      {
        case QAudioFormat::SignedInt:
        {
          switch (sampleSize)
          {
            case sizeof(short):
            {
              audioSamples.push_back(getSample<short>(buffer, i));
              break;
            }

            case sizeof(int):
            {
              audioSamples.push_back(getSample<int>(buffer, i));
              break;
            }

            case sizeof(long long):
            {
              audioSamples.push_back(getSample<long long>(buffer, i));
              break;
            }

            default:
              break;
          }
        }

        case QAudioFormat::UnSignedInt:
        {
          switch (sampleSize)
          {
            case sizeof(short):
            {
              audioSamples.push_back(getSample<unsigned short>(buffer, i));
              break;
            }

            case sizeof(int):
            {
              audioSamples.push_back(getSample<unsigned int>(buffer, i));
              break;
            }

            case sizeof(long long):
            {
              audioSamples.push_back(getSample<unsigned long long>(buffer, i));
              break;
            }

            default:
              break;
          }
        }

        case QAudioFormat::Float:
        {
          switch (sampleSize)
          {
            case sizeof(float):
            {
              audioSamples.push_back(getSample<float>(buffer, i));
              break;
            }

            case sizeof(double):
            {
              audioSamples.push_back(getSample<double>(buffer, i));
              break;
            }

            default:
              break;
          }
        }

        default:
          break;
      }
    }

    m_samples.clear();

    for (auto&& sample : audioSamples)
      m_samples.append(QVariant::fromValue(sample));
  });
}

const QString& AudioNote::path() const
{
  return m_path;
}

void AudioNote::setPath(const QString& newPath)
{
  if (m_path == newPath)
    return;

  m_path = newPath;
  emit pathChanged();
}

bool AudioNote::encrypted() const
{
  return m_encrypted;
}

void AudioNote::setEncrypted(bool newEncrypted)
{
  if (m_encrypted == newEncrypted)
    return;

  m_encrypted = newEncrypted;
  emit encryptedChanged();
}

const QColor& AudioNote::color() const
{
  return m_color;
}

void AudioNote::setColor(const QColor& newColor)
{
  if (m_color == newColor)
    return;

  m_color = newColor;
  emit colorChanged();
}

const QString& AudioNote::name() const
{
  return m_name;
}

void AudioNote::setName(const QString& newName)
{
  if (m_name == newName)
    return;

  m_name = newName;
  emit nameChanged();
}

const QString& AudioNote::password() const
{
  return m_password;
}

void AudioNote::setPassword(const QString& newPassword)
{
  if (m_password == newPassword)
    return;

  m_password = newPassword;
  emit passwordChanged();
}

const QVariantList& AudioNote::samples() const
{
  return m_samples;
}

AudioNotePlayback* AudioNote::playback() const
{
  return m_playback;
}

AudioNote* AudioNote::build(const QString& path)
{
  auto* note = new AudioNote();
  note->setPath(path);
  note->init();
  return note;
}

void AudioNote::saveToFile(const QString& filePath)
{
  QJsonObject obj;
  obj.insert("name", m_name);
  obj.insert("color", m_color.name());
  obj.insert("emcrypted", m_encrypted);

  if (m_encrypted)
    obj.insert("password", m_password);

  QFile out(m_path);
  out.open(QFile::WriteOnly);
  out.write(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact));
  out.write("\n");
  QFile in(filePath);
  in.open(QFile::ReadOnly);
  out.write(in.readAll());
}

void AudioNote::remove()
{
  m_playback->stop();
  emit removeNode();
}
