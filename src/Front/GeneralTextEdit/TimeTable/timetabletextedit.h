#ifndef TIMETABLETEXTEDIT_H
#define TIMETABLETEXTEDIT_H
#include "../gentextedit.h"

class TimetableTextEdit : public GenTextEdit {
  Q_OBJECT
public:
  TimetableTextEdit(QWidget *parent = nullptr);
  void fillCharsAndSetText(QString text, const QVector<charStyle_t> &arr);
  void keyPressEvent(QKeyEvent *event);
};

#endif // TIMETABLETEXTEDIT_H
