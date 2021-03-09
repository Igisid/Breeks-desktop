#ifndef INFOBUTTON_H
#define INFOBUTTON_H

#include <QGroupBox>

class InfoGB : public QGroupBox {
  Q_OBJECT

public:
  InfoGB(QWidget *parent = nullptr);

  void enterEvent(QEvent *event);
  void leaveEvent(QEvent *event);

signals:
  void enter();
  void leave();
};

#endif // INFOBUTTON_H
