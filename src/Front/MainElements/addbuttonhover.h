#ifndef ADDBUTTONHOVER_H
#define ADDBUTTONHOVER_H

#include <QPushButton>

class AddButtonHover : public QPushButton
{
  Q_OBJECT

public:
  AddButtonHover(QWidget *parent = nullptr);
  void enterEvent(QEvent *);
  void leaveEvent(QEvent *);

private:
  const QString styleSheetDefault = QStringLiteral("border-image:url(:/Images/Front/Images/addButtonDefault.png)");
  const QString styleSheetHover = QStringLiteral("border-image:url(:/Images/Front/Images/addButtonHover.png)");
};

#endif // ADDBUTTONHOVER_H
