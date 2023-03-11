#include "AudioNotesRepo.h"
#include "AudioNote.h"
#include "AudioNotesModel.h"

#include <QFileInfo>
#include <QDebug>
#include <utility>

#include "Scanner.h"

AudioNotesRepo::AudioNotesRepo(QString path, QObject* parent)
  : QObject{parent}, m_notesModel(new AudioNotesModel(this)), m_path(std::move(path))
{}

void AudioNotesRepo::init()
{
  update();
}

AudioNotesModel* AudioNotesRepo::notesModel() const
{
  return m_notesModel;
}

QString AudioNotesRepo::name() const
{
  return QFileInfo(m_path).fileName();
}

const QString& AudioNotesRepo::path() const
{
  return m_path;
}

void AudioNotesRepo::addNote(AudioNote* note)
{
  m_notesModel->addIfNotExists(note);
}

void AudioNotesRepo::update()
{
  static Scanner scanner;
  connect(&scanner, &Scanner::scanComplete, this, [this](const QString & path, const QStringList & audioNotes)
  {
    if (path.compare(m_path) != 0)
      return;

    //реверс для сохранения порядка
    for (auto it = audioNotes.crbegin(); it != audioNotes.crend(); ++it)
      m_notesModel->addIfNotExists(AudioNote::build(*it));
  });
  scanner.scanFolder(m_path);
}
