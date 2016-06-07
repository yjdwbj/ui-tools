#ifndef HANDLEJSON_H
#define HANDLEJSON_H
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>
#include <QtCore/QFileDevice>
#include <QTextStream>

void HandleFrameObject(QJsonObject qjo,QString ParentName);


#endif // HANDLEJSON_H
