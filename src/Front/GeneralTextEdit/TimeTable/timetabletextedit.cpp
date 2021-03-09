#include "timetabletextedit.h"

#include <QScrollBar>

TimetableTextEdit::TimetableTextEdit(QWidget *parent) : GenTextEdit(parent) {
  this->clearCharStyleVector();
  this->clear();
  this->setCharCounter(0);

  this->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {"
                                           "background-color: #FFFFF0;"
                                           "width: 7px;"
                                           "margin: 0px 0px 0px 0px;"
                                           "border: 0.1px solid #FFFFFF;}"

                                           "QScrollBar::handle:vartical {"
                                           "border-radius: 3px;"
                                           "background: #e3e3df;"
                                           "min-height: 30px;}"

                                           "QScrollBar::handle:vertical:hover {"
                                           "border-radius: 3px;"
                                           "background: #c7c7bf;"
                                           "min-height: 30px;}"

                                           "QScrollBar::add-line:vertical {"
                                           "border: none;"
                                           "background: none;}"

                                           "QScrollBar::sub-line:vertical {"
                                           "border: none;"
                                           "background: none;}");
}

void TimetableTextEdit::fillCharsAndSetText(QString text, const QVector<charStyle_t> &arr) {
  this->clearCharStyleVector();
  this->setCharCounter(arr.size());

  QJsonArray jArr;

  for (const charStyle_t &ch : arr) {
    QJsonObject jChar;
    jChar.insert(QStringLiteral("bold"), ch.bold);
    jChar.insert(QStringLiteral("italic"), ch.italic);
    jChar.insert(QStringLiteral("underline"), ch.underline);
    jChar.insert(QStringLiteral("strike"), ch.strike);
    jChar.insert(QStringLiteral("item"), ch.item);
    jChar.insert(QStringLiteral("star"), ch.star);
    jChar.insert(QStringLiteral("sColor"), ch.sColor);
    jChar.insert(QStringLiteral("spellChecker"), ch.spellChecker);
    jArr.push_back(jChar);
  }

  QTextStream out(&text);
  QChar tmpChar;
  charStyle_t ch;
  QTextCharFormat charFormat;

  for (int i = 0; i < this->getCharCounter(); ++i) {
    QJsonObject jChar = jArr[i].toObject();
    GenTextEdit::setStylesToChar(ch, charFormat, jChar);
    int cursorPos = this->textCursor().position();
    this->fillCharStyleVector(cursorPos, 1, ch);
    out >> tmpChar;
    this->textCursor().insertText(static_cast<QString>(tmpChar), charFormat);
  }
}

void TimetableTextEdit::keyPressEvent(QKeyEvent *event) {
  GenTextEdit::keyPressEvent(event);
}
