#include "imagehover.h"

ImageHover::ImageHover(QWidget *parent) : QPushButton(parent) {}

void ImageHover::enterEvent(QEvent *event) {
  emit imageEnter(true);

#if QT_VERSION >= 0x060000
  QPushButton::enterEvent((QEnterEvent *)event);
#else
  QPushButton::enterEvent(event);
#endif
}

void ImageHover::leaveEvent(QEvent *event) {
  emit imageLeave(false);
  QPushButton::leaveEvent(event);
}
