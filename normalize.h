#ifndef NORMALIZE_H
#define NORMALIZE_H

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDomNode>
#include <QDomNodeList>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QProgressBar>
#include <QLabel>

class Normalize : public QObject
{
    Q_OBJECT
public:
    explicit Normalize(int id, QObject *parent = 0);
    QString fileName;
private:
    int id;
    int _errno;
signals:
    void finished();
    void sendLabel(QString);
    void sendProgress(int);
    void sendMessage(QString);
public slots:
    void start();
    void onFinished();
};

#endif // NORMALIZE_H
