#include "addbuttonhover.h"

#include <QGraphicsDropShadowEffect>

AddButtonHover::AddButtonHover(QWidget *parent) : QPushButton(parent) {
  this->setStyleSheet(styleSheetDefault);
}

void AddButtonHover::enterEvent(QEvent *event) {
  QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
  effect->setBlurRadius(10);
  effect->setXOffset(0);
  effect->setYOffset(1);
  effect->setColor("#909090");
  this->setGraphicsEffect(effect);

#if QT_VERSION >= 0x060000
  QPushButton::enterEvent((QEnterEvent *)event);
#else
  QPushButton::enterEvent(event);
#endif
}

void AddButtonHover::leaveEvent(QEvent *event) {
  QPushButton::leaveEvent(event);
  this->setGraphicsEffect(nullptr);
}
