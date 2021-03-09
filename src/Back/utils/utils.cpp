#include "Back/utils/utils.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

QJsonObject objectFromString(const QString &in) {
  QJsonObject obj = QJsonObject();

  QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

  // check validity of the document
  if (!doc.isNull()) {
    if (doc.isObject()) {
      obj = doc.object();
    } else {
      qDebug() << "Document is not an object" << Qt::endl;
    }
  } else {
    qDebug() << "Invalid JSON ... " << in << Qt::endl;
  }

  return obj;
}

QJsonArray jsonArrayFromString(const QString &in) {
  QJsonArray arr = QJsonArray();

  QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

  // check validity of the document
  if (!doc.isNull()) {
    if (doc.isArray()) {
      arr = doc.array();
    } else {
      qDebug() << "Document is not an array" << Qt::endl;
    }
  } else {
    qDebug() << "Invalid JSON ... " << in << Qt::endl;
  }

  return arr;
}

void createCharStyleVector(QVector<charStyle_t> &charStyleVector, const QJsonArray &jsonArray) {
  for (const auto &jsonIter : jsonArray) {
    QJsonObject jsonForOneChar = jsonIter.toObject();

    qDebug() << jsonForOneChar;

    charStyle_t charStyle = {jsonForOneChar.value("bold").toBool(),      jsonForOneChar.value("italic").toBool(),
                             jsonForOneChar.value("underline").toBool(), jsonForOneChar.value("strike").toBool(),
                             jsonForOneChar.value("item").toBool(),      jsonForOneChar.value("spellChecker").toBool(),
                             jsonForOneChar.value("star").toBool(),      jsonForOneChar.value("sColor").toString()};

    charStyleVector.push_back(charStyle);
  }
}

void createArrNEmoji(int *arrNEmoji, const unsigned size, const QJsonArray &jsonArray) {
  unsigned i = 0;
  for (const auto &jsonIter : jsonArray) {
    QJsonObject jsonForOneEmoji = jsonIter.toObject();

    if (i < size)
      arrNEmoji[i] = jsonForOneEmoji.value(QStringLiteral("emojiNum")).toInt();
    else
      qDebug() << "Array contains more than " << size << " emojies";
    ++i;
  }
}
