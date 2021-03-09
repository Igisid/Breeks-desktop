#ifndef CALENDARWEEK_H
#define CALENDARWEEK_H

#include <QDateTime>
#include <QObject>
#include <QPushButton>

class CalendarWeek : public QPushButton {
  Q_OBJECT

public:
  CalendarWeek(QWidget *parent = nullptr);

  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
  bool eventFilter(QObject *object, QEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;

  std::unique_ptr<QDateTime> date;

signals:
  void changeCalendarWeek(qint64);

private slots:
  void onEnterEvent();
};

#endif // CALENDARWEEK_H
