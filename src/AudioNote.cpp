#include "AudioNote.h"

#include "AudioNotePlayback.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include <QBuffer>
#include <QAudioDecoder>


AudioNote::AudioNote(QObject *parent)
    : QObject{parent},
      m_playback(nullptr)
{

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
    auto buffer = new QBuffer(&content, this);
    QAudioDecoder* audioDecoder = new QAudioDecoder(this);
    audioDecoder->setSourceDevice(buffer);
    audioDecoder->start();

    connect(audioDecoder, &QAudioDecoder::finished, [=](){
        std::vector<AudioSample> audioSamples;

        auto buffer = audioDecoder->read();
        int sampleSize = buffer.format().sampleSize();
        for(int i=0; i<buffer.sampleCount(); ++i) {
            if(buffer.format().sampleType() == QAudioFormat::SignedInt){
                if(sampleSize == 2) {
                    auto data = buffer.data<short>();
                    audioSamples.push_back(AudioSample{ (double)abs(*data)/(double)std::numeric_limits<short>::max(), (int)buffer.format().durationForBytes(i*sampleSize) });
                } else if(sampleSize == 4) {
                    auto data = buffer.data<int>();

                    audioSamples.push_back(AudioSample{ (double)abs(*data)/(double)std::numeric_limits<int>::max(), (int)buffer.format().durationForBytes(i*sampleSize) });
                } else if(sampleSize == 8) {
                    auto data = buffer.data<long>();

                    audioSamples.push_back(AudioSample{ (double)abs(*data)/(double)std::numeric_limits<long>::max(), (int)buffer.format().durationForBytes(i*sampleSize) });
                }
            } else if(buffer.format().sampleType() == QAudioFormat::UnSignedInt){
                if(sampleSize == 2) {
                    auto data = buffer.data<unsigned short>();

                    audioSamples.push_back(AudioSample{ (double)*data/(double)std::numeric_limits<unsigned short>::max(), (int)buffer.format().durationForBytes(i*sampleSize) });
                } else if(sampleSize == 4) {
                    auto data = buffer.data<unsigned int>();

                    audioSamples.push_back(AudioSample{ (double)*data/(double)std::numeric_limits<unsigned short>::max(), (int)buffer.format().durationForBytes(i*sampleSize) });
                } else if(sampleSize == 8) {
                    auto data = buffer.data<unsigned long>();

                    audioSamples.push_back(AudioSample{ (double)*data/(double)std::numeric_limits<unsigned long>::max(), (int)buffer.format().durationForBytes(i*sampleSize) });
                }
            } else if(buffer.format().sampleType() == QAudioFormat::Float){
                if(sampleSize == 4) {
                    auto data = buffer.data<float>();

                    audioSamples.push_back(AudioSample{ (double)*data/(double)std::numeric_limits<double>::max(), (int)buffer.format().durationForBytes(i*sampleSize) });
                } else if(sampleSize == 8) {
                    auto data = buffer.data<double>();

                    audioSamples.push_back(AudioSample{ (double)*data/(double)std::numeric_limits<double>::max(), (int)buffer.format().durationForBytes(i*sampleSize) });
                }
            }
        }
        m_samples.clear();
        for(size_t i=0; i<audioSamples.size(); i++){
            m_samples.append(QVariant::fromValue(audioSamples[i]));
        }
    });
}

const QString &AudioNote::path() const
{
    return m_path;
}

void AudioNote::setPath(const QString &newPath)
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

const QColor &AudioNote::color() const
{
    return m_color;
}

void AudioNote::setColor(const QColor &newColor)
{
    if (m_color == newColor)
        return;
    m_color = newColor;
    emit colorChanged();
}

const QString &AudioNote::name() const
{
    return m_name;
}

void AudioNote::setName(const QString &newName)
{
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}

const QString &AudioNote::password() const
{
    return m_password;
}

void AudioNote::setPassword(const QString &newPassword)
{
    if (m_password == newPassword)
        return;
    m_password = newPassword;
    emit passwordChanged();
}

const QVariantList &AudioNote::samples() const
{
    return m_samples;
}

AudioNotePlayback *AudioNote::playback() const
{
    return m_playback;
}

AudioNote* AudioNote::build(const QString &path)
{
    auto note = new AudioNote();
    note->setPath(path);
    note->init();
    return note;
}

void AudioNote::saveToFile(const QString &filePath)
{
    QJsonObject obj;
    obj.insert("name", m_name);
    obj.insert("color", m_color.name());
    obj.insert("emcrypted", m_encrypted);
    if(m_encrypted){
        obj.insert("password", m_password);
    }
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
