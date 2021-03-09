#include "infogb.h"

#include <QEnterEvent>

InfoGB::InfoGB(QWidget *parent) : QGroupBox(parent) {}

void InfoGB::enterEvent(QEvent *event) {
  emit enter();
  QWidget::enterEvent((QEnterEvent *)event);
}

void InfoGB::leaveEvent(QEvent *event) {
  emit leave();
  QWidget::leaveEvent(event);
}
