#include "daywidget.h"

DayWidget::DayWidget(QWidget *parent) : QWidget(parent) {
  setAcceptDrops(true);
}

void DayWidget::dragEnterEvent(QDragEnterEvent *event) {
  event->acceptProposedAction();
  emit elementEnterArea(dayNumber_);
  // this->setStyleSheet("border: 2px solid #111111; border-radius: 9px;");
}

void DayWidget::dragLeaveEvent(QDragLeaveEvent *event) {
  emit elementLeaveArea(dayNumber_);
  // this->setStyleSheet("border-radius: 9px;");
}

void DayWidget::dropEvent(QDropEvent *event) {
  //  emit dropNoChanges();

  elementData_t elemData;
  QByteArray data = event->mimeData()->data(QStringLiteral("elemData"));
  QDataStream out(&data, QIODevice::ReadOnly);

  QString sId;
  out >> sId >> elemData.text >> elemData.timeStart >> elemData.timeEnd >> elemData.tagColorNum >> dragPos_;
  elemData.idOnServer = sId.toLong();
  elemData.tagColor = tag::ARR_COLORS[elemData.tagColorNum];

  QByteArray indexes = event->mimeData()->data(QStringLiteral("indexes"));
  QDataStream outIndexes(&indexes, QIODevice::ReadOnly);
  outIndexes >> dayIndex_ >> elemIndex_;

  QByteArray charVector = event->mimeData()->data(QStringLiteral("charVector"));
  QDataStream outVector(&charVector, QIODevice::ReadOnly);
  int size = 0;
  outVector >> size;
  for (int i = 0; i < size; i++) {
    charStyle_t ch;
    outVector >> ch.bold >> ch.italic >> ch.underline >> ch.strike >> ch.item >> ch.star >> ch.sColor;
    elemData.charStyleVector.push_back(ch);
  }

  if (dayNumber_ != dayIndex_) {
    emit dropElement(dayNumber_, dayIndex_, elemIndex_, elemData);
    emit sendDayAndElementIndex(dayIndex_, elemIndex_, false);
  }
  emit elementLeaveArea(dayNumber_);

  event->acceptProposedAction();
}

void DayWidget::dragMoveEvent(QDragMoveEvent *event) {
  event->acceptProposedAction();
  emit moveElement();
}

int DayWidget::getDayNumber() const {
  return dayNumber_;
}

void DayWidget::setDayNumber(const int numb) {
  dayNumber_ = numb;
}
