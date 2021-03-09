#include "gentextedit.h"

void GenTextEdit::readFromDB(const int currentFile) {
  QJsonObject textInfo = filesystem::readTextEdidFromDB(currentFile);
  QJsonArray jChars = textInfo.value(QStringLiteral("charStyleVector")).toArray();
  QString text = textInfo.value(QStringLiteral("text")).toString();

  charStyleVector_.clear();
  charCounter_ = jChars.size();

  QTextStream out(&text);
  QChar tmpChar;

  for (int i = 0; i < charCounter_; ++i) {
    QTextCharFormat charFormat;
    charStyle_t ch;
    QJsonObject jChar = jChars[i].toObject();
    detailsSetCharStyle(ch);
    setStylesToChar(ch, charFormat, jChar);

    int cursorPos = this->textCursor().position();
    this->fillCharStyleVector(cursorPos, 1, ch);
    out >> tmpChar;
    this->textCursor().insertText(static_cast<QString>(tmpChar), charFormat); // add with style effects

    QTextCursor c = this->textCursor();
    c.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
    detailsColorText(c, ch.sColor);
  }
  checkSpelling();
  undoRedoBuffer_ = std::make_unique<UndoRedoText>();
}

void GenTextEdit::writeToDB(const int currentFile) {
  textInfo_t info;
  //  QJsonArray jChars;

  foreach (charStyle_t ch, charStyleVector_) {
    QJsonObject jChar;
    jChar.insert(QStringLiteral("bold"), ch.bold);
    jChar.insert(QStringLiteral("italic"), ch.italic);
    jChar.insert(QStringLiteral("underline"), ch.underline);
    jChar.insert(QStringLiteral("strike"), ch.strike);
    jChar.insert(QStringLiteral("item"), ch.item);
    jChar.insert(QStringLiteral("star"), ch.star);
    jChar.insert(QStringLiteral("sColor"), ch.sColor);
    jChar.insert(QStringLiteral("spellChecker"), ch.spellChecker);
    info.jArr.push_back(jChar);
  }

  info.text = this->toPlainText();
  filesystem::writeTextEditToDB(info, currentFile);
}
