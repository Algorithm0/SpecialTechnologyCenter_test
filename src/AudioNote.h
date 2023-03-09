#pragma once

#include <QObject>
#include <QColor>
#include <QVariantList>

struct AudioSample {
private:
    Q_GADGET
public:
    Q_PROPERTY(double loudness MEMBER loudness)
    Q_PROPERTY(double time MEMBER time)

    double loudness;
    int time;
};

class AudioNotePlayback;

class AudioNote : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path WRITE setPath READ path NOTIFY pathChanged)

    Q_PROPERTY(QString name WRITE setName READ name NOTIFY nameChanged)
    Q_PROPERTY(QColor color WRITE setColor READ color NOTIFY colorChanged)
    Q_PROPERTY(bool encrypted WRITE setEncrypted READ encrypted NOTIFY encryptedChanged)
    Q_PROPERTY(QString password WRITE setPassword READ password NOTIFY passwordChanged)

    Q_PROPERTY(QVariantList samples READ samples NOTIFY samplesChanged)

    Q_PROPERTY(AudioNotePlayback* playback READ playback NOTIFY playbackChanged)

public:
    explicit AudioNote(QObject *parent = nullptr);
    void init();

    const QString &path() const;
    void setPath(const QString &newPath);

    bool encrypted() const;

    void setEncrypted(bool newEncrypted);

    const QColor &color() const;
    void setColor(const QColor &newColor);

    const QString &name() const;
    void setName(const QString &newName);

    const QString &password() const;
    void setPassword(const QString &newPassword);

    const QVariantList &samples() const;

    static AudioNote* build(const QString & path);


    void saveToFile(const QString & filePath);

public slots:
    void remove();

signals:

    void pathChanged();
    void encryptedChanged();
    void colorChanged();
    void nameChanged();
    void passwordChanged();
    void samplesChanged();
    void playbackChanged();
    void removeNode();

private:
    QString m_path;
    bool m_encrypted{};

    QColor m_color;
    QString m_name;
    QString m_password;
    QVariantList m_samples;
    AudioNotePlayback *m_playback;

    AudioNotePlayback *playback() const;
};

