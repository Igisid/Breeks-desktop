#include "russiandictionary.h"

#include <QDebug>
#include <QDir>
#include <QFile>

RussianDictionary::RussianDictionary() {
  //.DIC
  //  QString path =
  //  "C:/Users/ypyla/PROJECTS/Breeks-desktop/src/Front/RusDic/RusDic.txt";
  //  QString path = QDir::current().path() + "/Front/RusDic/RusDic.txt";
  QString path = QDir::current().path() + "/RusDic.txt"; // Release
  qDebug() << path;

  QFile fDic(path);

  if (!fDic.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "FILE OPENING CRASHED!";
  }
  QString sDic = fDic.readAll();
  QTextStream sourceDic(&sDic);
  size_t nWords = 0;
  sourceDic >> nWords;
  QString word = QLatin1String("");

  for (size_t i = 0; i < nWords - 1; ++i) {
    sourceDic >> word;
    detailsAddWord(word);
  }
}

bool RussianDictionary::isCorrectWord(const QString &word) {
  const int indexFirstLetter = QChar(word.at(0).toLower()).unicode() - RUS_A_CODE;
  const int length = word.length();

  return std::any_of(std::cbegin(arrDic_[indexFirstLetter][length]), std::cend(arrDic_[indexFirstLetter][length]),
                     [&](const QString &dicWord) {
                       return word == dicWord;
                     });
}

void RussianDictionary::addNewWord(const QString &word) {
  if (word.length() < MAX_WORD_LEN) {
    detailsAddWord(word);
  }
}

void RussianDictionary::detailsAddWord(QString word) {
  word = word.toLower();
  while (!word.isEmpty() && !(word.at(0) >= QChar(L'а') && word.at(0) <= QChar(L'я'))) {
    word = word.right(word.length() - 1);
  }
  if (!word.isEmpty()) {
    const int index = (word.at(0) != QChar(YO_UNICODE)) ? QChar(word.at(0).toLower()).unicode() - RUS_A_CODE : YO_POS;
    const int length = word.length();
    arrDic_[index][length].push_back(word);
  }
}

void RussianDictionary::printAllWords() {
  for (int i = 0; i < N_LETTERS; ++i) {
    for (int j = 0; j < MAX_WORD_LEN; ++j) {
      for (const QString &word : std::as_const(arrDic_)[i][j]) {
        qDebug() << word;
      }
    }
  }
}
