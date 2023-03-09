#include "AudioNoteCreator.h"
#include "AudioNote.h"
#include "AudioNotesRepo.h"
#include <QDateTime>

#include <QDebug>
#include <QFile>

#include <QTimer>
#include <QAudioRecorder>
#include <QAudioProbe>
#include <QUrl>

#include <cmath>

void AudioNoteCreator::updateInputDevices()
{
    auto inputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    if(m_inputDevices != inputDevices) {
        m_inputDevices = std::move(inputDevices);
        emit inputDevicesChanged();
    }
}

AudioNoteCreator::AudioNoteCreator(QObject *parent)
    : QObject{parent},
      m_audioNote(new AudioNote(this)),
      m_inputDevices(QAudioDeviceInfo::availableDevices(QAudio::AudioInput)),
      m_recorder(nullptr),
      m_recordingAccepted(false),
      m_recordingAmplitude(0.0)
{
    auto updateDevicesTimer = new QTimer(this);
    connect(updateDevicesTimer, &QTimer::timeout, this, [this](){
        updateInputDevices();
    });
    updateDevicesTimer->start(5000);

    connect(m_audioNote, &AudioNote::nameChanged, this, &AudioNoteCreator::readyChanged);
    connect(m_audioNote, &AudioNote::pathChanged, this, &AudioNoteCreator::readyChanged);
    connect(m_audioNote, &AudioNote::encryptedChanged, this, [this](){
        if(!m_audioNote->encrypted()) {
            m_audioNote->setPassword("");
        }
        emit readyChanged();
    });
    connect(m_audioNote, &AudioNote::passwordChanged, this, &AudioNoteCreator::readyChanged);
}

AudioNote *AudioNoteCreator::audioNote() const
{
    return m_audioNote;
}


bool AudioNoteCreator::ready() const
{
    return !m_audioNote->name().isEmpty() && !m_recordedPath.isEmpty() && !(m_audioNote->encrypted() && m_audioNote->password().isEmpty());
}

QStringList AudioNoteCreator::inputDevices() const
{
    QStringList result;
    result.reserve(m_inputDevices.size());
    for(auto & device: m_inputDevices){
        result.append(device.deviceName());
    }
    return result;
}

template<std::size_t Size>
struct sized_integer;

template<>
struct sized_integer<8> {
    using signed_type = std::int8_t;
    using unsigned_type = std::uint8_t;
};

template<>
struct sized_integer<16> {
    using signed_type = std::int16_t;
    using unsigned_type = std::uint16_t;
};

template<>
struct sized_integer<32> {
    using signed_type = std::int32_t;
    using unsigned_type = std::uint32_t;
};
template<>
struct sized_integer<64> {
    using signed_type = std::int64_t;
    using unsigned_type = std::uint64_t;
};

template<std::size_t Size>
using signed_interger_t = typename sized_integer<Size>::signed_type;

template<std::size_t Size>
using unsigned_interger_t = typename sized_integer<Size>::unsigned_type;

template<QAudioFormat::SampleType SampleType, std::size_t Size>
struct audio_buffer_data;

template<std::size_t Size>
struct audio_buffer_data<QAudioFormat::SampleType::SignedInt, Size>{
    using type = signed_interger_t<Size>;
};

template<std::size_t Size>
struct audio_buffer_data<QAudioFormat::SampleType::UnSignedInt, Size>{
    using type = unsigned_interger_t<Size>;
};

template<std::size_t Size>
struct audio_buffer_data<QAudioFormat::SampleType::Float, Size>{
    using type = std::conditional_t<Size == 32, float, double>;
};

template<QAudioFormat::SampleType SampleType, std::size_t Size>
using audio_buffer_data_t = typename audio_buffer_data<SampleType, Size>::type;


template<QAudioFormat::SampleType SampleType>
qreal calculateAmplitude_impl(const QAudioBuffer &) {
    return 0.0;
}

template<QAudioFormat::SampleType SampleType, std::size_t Size, std::size_t ...SizesLeft>
qreal calculateAmplitude_impl(const QAudioBuffer & audioBuffer) {
    if(audioBuffer.format().sampleSize() == Size) {
        using AudioBufferData = audio_buffer_data_t<SampleType, Size>;
        qreal amplitude = 0;
        auto count = audioBuffer.sampleCount();

        auto data = audioBuffer.data<AudioBufferData>();
        for(auto i=0; i<count; ++i) {
            auto sampleAmplitude = [&data](){
                if constexpr(SampleType == QAudioFormat::SampleType::UnSignedInt) {
                    static constexpr auto max_value = std::numeric_limits<AudioBufferData>::max()/2;
                    return static_cast<qreal>(static_cast<AudioBufferData>(*data)%max_value)/static_cast<qreal>(max_value);
                } else {
                    static constexpr auto max_value = std::numeric_limits<AudioBufferData>::max();
                    return std::fabs(static_cast<qreal>(*data))/static_cast<qreal>(max_value);
                }
            }();
            amplitude += sampleAmplitude/count;
            data++;
        }
        return amplitude;
    }
    return calculateAmplitude_impl<SampleType, SizesLeft...>(audioBuffer);
}

qreal calculateAmplitude(const QAudioBuffer & audioBuffer) {
    switch(audioBuffer.format().sampleType()) {
    case QAudioFormat::SampleType::SignedInt:
        return calculateAmplitude_impl<QAudioFormat::SampleType::SignedInt, 8, 16, 32, 64>(audioBuffer);
    case QAudioFormat::SampleType::UnSignedInt:
        return calculateAmplitude_impl<QAudioFormat::SampleType::UnSignedInt, 8, 16, 32, 64>(audioBuffer);
    case QAudioFormat::SampleType::Float:
        return calculateAmplitude_impl<QAudioFormat::SampleType::Float, 32, 64>(audioBuffer);
    default:
        return 0.0;
    }
}

void AudioNoteCreator::startRecording(const QString &device) {
    if(m_recorder){
        return;
    }
    m_recorder = new QAudioRecorder(this);
    auto probe = new QAudioProbe(m_recorder);
    probe->setSource(m_recorder);
    connect(probe, &QAudioProbe::audioBufferProbed, this, [this](const QAudioBuffer & audioBuffer){
        m_recordingAmplitude = calculateAmplitude(audioBuffer);
        emit recordingAmplitudeChanged();
    });


    QAudioEncoderSettings audioSettings;

    auto audioDevice = [this, &device](){
        for(auto &deviceInfo: m_inputDevices) {
            if(deviceInfo.deviceName() == device) {
                return deviceInfo;
            }
        }
        return QAudioDeviceInfo::defaultInputDevice();
    }();

    m_recorder->setAudioInput(audioDevice.deviceName());
    audioSettings.setCodec("audio/x-opus");
    audioSettings.setChannelCount(1);
    audioSettings.setSampleRate(16000);

    m_recorder->setAudioSettings(audioSettings);
    m_recorder->setContainerFormat("audio/ogg");
    m_recorder->setOutputLocation(QUrl::fromLocalFile("_audionote_tmp_" + QString::number(QDateTime::currentMSecsSinceEpoch())));

    connect(m_recorder, &QAudioRecorder::stateChanged, this, [this](QMediaRecorder::State state){
        if(state != QMediaRecorder::StoppedState) {
            return;
        }
        if(auto &&path = m_recorder->actualLocation().toLocalFile(); m_recordingAccepted) {
            m_recordedPath = path;
            emit readyChanged();
        } else {
            QFile::remove(path);
        }
        m_recorder->deleteLater();
        m_recorder = nullptr;
        emit isRecordingChanged();
    });

    m_recorder->record();
    emit isRecordingChanged();
}

void AudioNoteCreator::stopRecording()
{
    if(!m_recorder) {
        return;
    }
    m_recordingAccepted = true;
    m_recorder->stop();

}

void AudioNoteCreator::cancelRecording()
{
    if(!m_recorder) {
        return;
    }
    m_recordingAccepted = false;
    m_recorder->stop();
}

void AudioNoteCreator::create(AudioNotesRepo *targetRepo)
{
    m_audioNote->setPath(targetRepo->path() + "/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".audionote");
    m_audioNote->saveToFile(m_recordedPath);
    m_audioNote->init();
    targetRepo->addNote(m_audioNote);
}

bool AudioNoteCreator::isRecording() const
{
    return m_recorder != nullptr;
}

qreal AudioNoteCreator::recordingAmplitude() const
{
    return m_recordingAmplitude;
}
