#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Scanner.h"
#include "AudioNote.h"
#include "AudioNotesApp.h"
#include "AudioNotesRepo.h"
#include "AudioNotesModel.h"
#include "AudioNotesReposModel.h"
#include "AudioNotePlayback.h"
#include "AudioNoteCreator.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<Scanner>("AudioNotes", 1, 0, "Scanner");
    qmlRegisterType<AudioNotesApp>("AudioNotes", 1, 0, "AudioNotesApp");
    qmlRegisterType<AudioNotesRepo>("AudioNotes", 1, 0, "AudioNotesRepo");
    qmlRegisterType<AudioNotesReposModel>("AudioNotes", 1, 0, "AudioNotesReposModel");
    qmlRegisterType<AudioNote>("AudioNotes", 1, 0, "AudioNote");
    qmlRegisterType<AudioNotesModel>("AudioNotes", 1, 0, "AudioNotesModel");
    qmlRegisterType<AudioNotePlayback>("AudioNotes", 1, 0, "AudioNotePlayback");
    qmlRegisterType<AudioNoteCreator>("AudioNotes", 1, 0, "AudioNoteCreator");


    AudioNotesApp* notesApp = new AudioNotesApp();

    engine.rootContext()->setContextProperty("notesApp", notesApp);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    notesApp->init();

    return app.exec();
}
