#pragma once

#include <QObject>

#include <QAudioDeviceInfo>

class QAudioRecorder;

class AudioNotesRepo;
class AudioNote;
class AudioNoteCreator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(AudioNote* audioNote READ audioNote CONSTANT)
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY isRecordingChanged)
    Q_PROPERTY(qreal recordingAmplitude READ recordingAmplitude NOTIFY recordingAmplitudeChanged)
    Q_PROPERTY(QStringList inputDevices READ inputDevices NOTIFY inputDevicesChanged)


    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
public:
    explicit AudioNoteCreator(QObject *parent = nullptr);

    bool ready()const;
    QStringList inputDevices()const;
    bool isRecording() const;

    qreal recordingAmplitude() const;

public slots:
    void startRecording(const QString & device);
    void stopRecording();
    void cancelRecording();
    void create(AudioNotesRepo* targetRepo);
signals:
    void readyChanged();
    void inputDevicesChanged();
    void isRecordingChanged();
    void recordingAmplitudeChanged();

private:
    AudioNote *m_audioNote;
    QList<QAudioDeviceInfo> m_inputDevices;
    QAudioRecorder* m_recorder;
    bool m_recordingAccepted;
    qreal m_recordingAmplitude;

    QString m_recordedPath;

    AudioNote *audioNote() const;
    void updateInputDevices();
};

