#ifndef DAYWIDGET_H
#define DAYWIDGET_H

#include <QDrag>
#include <QDragEnterEvent>
#include <QWidget>

#include "Front/datastructures.h"

class DayWidget : public QWidget {
  Q_OBJECT

public:
  explicit DayWidget(QWidget *parent = nullptr);

  void dragEnterEvent(QDragEnterEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dropEvent(QDropEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);

  int getDayNumber() const;
  void setDayNumber(const int);

private:
  int dayNumber_;
  int dayIndex_;
  int elemIndex_;
  QPoint dragPos_;

signals:
  void dropElement(const int, const int, const int, const elementData_t);
  void sendDayAndElementIndex(const int, const int, bool);
  void sendElementsHeight(const int, const int);
  void moveElement();
  void dropNoChanges();

  void elementEnterArea(int);
  void elementLeaveArea(int);
};

#endif // DAYWIDGET_H
