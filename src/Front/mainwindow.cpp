#include "mainwindow.h"

#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QPointer>
#include <QScrollArea>
#include <QScrollBar>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

#include <memory>

#include "Front/GeneralTextEdit/gentextedit.h"
#include "Front/MainElements/EmojiHub/emojihub.h"
#include "Front/MainElements/elementtemplate.h"
#include "Front/datastructures.h"

#include "Back/secret-data.h"
#include "Back/server-connection.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      fileLastVisit_(fileLastVisitName_),
      fileMon_(fileMonName_),
      fileTue_(fileTueName_),
      fileWed_(fileWedName_),
      fileThu_(fileThuName_),
      fileFri_(fileFriName_),
      fileSat_(fileSatName_) {
  ui->setupUi(this);

  this->showMaximized();

  // TEST NETWORK
  server = std::make_unique<Network::ServerConnection>(std::make_shared<QNetworkAccessManager>(),
                                                       std::make_shared<Network::UserData>());
  userData = server->getUserData();

  // init week signals from server
  connect(server.get(), &Network::ServerConnection::initWeekData, this, &MainWindow::clearAndInitWeekData);

  connect(server.get(), &Network::ServerConnection::sendBreeksLinesToGUI, this, &MainWindow::initBreeksLines);

  connect(server.get(), &Network::ServerConnection::sendTTElementsToGUI, this, &MainWindow::initTTElements);

  connect(server.get(), &Network::ServerConnection::sendNoteToGUI, this, &MainWindow::initNote);

  connect(server.get(), &Network::ServerConnection::sendImageToGUI, this, &MainWindow::initImage);

  // logout signal
  connect(server.get(), &Network::ServerConnection::logout, this, &MainWindow::logout);

  this->setStyleSheet(QStringLiteral("background: #F9F9F9"));

  connect(ui->buttonImage, &ImageHover::imageEnter, this, &MainWindow::setImageBackgroundView);
  connect(ui->buttonImage, &ImageHover::imageLeave, this, &MainWindow::setImageBackgroundView);

  setWorkZone();

  setAllElementsEffects();

  ui->note->setContentsMargins(10, 10, 10, 10);
  connect(ui->note, qOverload<int>(&GenTextEdit::sendServerRequest), this, &MainWindow::sendPostRequestNote);

  // ADD BREEKS FORM
  /// @bug This is not a normalized connection
  connect(this, SIGNAL(sendBreekData(bool *, breeksData_t)), this, SLOT(recieveBreeksZoneData(bool *, breeksData_t)));
  connect(ui->emojiHub, &EmojiHub::changeEmoji, ui->emojiButton, &DemoBreek::changeEmoji);
  connect(ui->addBreekGB, &AddBreekGB::focusOut, ui->emojiHub, &EmojiHub::showThis);
  setStyleAddBreeksForm();
  ui->addBreekGB->hide();

  // ADD TIMETABLE ELEMENT FORM
  /// @bug This is not a normalized connection
  connect(this, SIGNAL(sendTimetableElementData(bool *, elementData_t)), this,
          SLOT(recieveTimeTableZoneData(bool *, elementData_t)));
  setStyleAddTimetableElementForm();
  ui->addTimetableElementGb->hide();

  // INFO BUTTONS
  setInfoButtonsStyle();
  QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
  effect->setBlurRadius(5);
  effect->setXOffset(0);
  effect->setYOffset(1);
  effect->setColor("#909090");
  ui->infoButton->setGraphicsEffect(effect);

  // REGISTRATION
  ui->message->hide();
  connect(server.get(), &Network::ServerConnection::loginReply, this, &MainWindow::loginReply);
  setShadow(ui->reg);
  setShadow(ui->login);

  ui->widget->hide();
  ui->hideCalendar->hide();

  // connect for writing to refresh file
  connect(server.get(), &Network::ServerConnection::sendDataToRfrshFile, this, &MainWindow::writeToRfrshFile);

  // check if we can auto authorize the user
  checkSavedSession();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::logout() {
  clearWeekData();
  server->getUserData()->setAccessToken(QLatin1String(""));
  server->getUserData()->setRefreshToken(QLatin1String(""));
  ui->authFrom->show();
  ui->demoLable->show();
  writeToRfrshFile(QLatin1String(""), QLatin1String(""));
}

void MainWindow::writeToRfrshFile(const QString &refresh, const QString &email) {
  QJsonObject json;
  json.insert(QStringLiteral("refresh"), refresh);
  json.insert(QStringLiteral("email"), email);
  QJsonDocument jsonDoc(json);
  QByteArray jsonData = jsonDoc.toJson();

  QFile refreshFile(QDir::current().path() + rfrshPath_);
  if (!refreshFile.open(QIODevice::ReadWrite)) {
    qDebug() << "Error. Refresh file is not open!";
    return;
  }

  refreshFile.resize(0);
  refreshFile.write(jsonData);
}

QJsonObject *MainWindow::openRefreshFile() {
  QFile refreshFile(QDir::current().path() + rfrshPath_);

  if (!refreshFile.open(QIODevice::ReadOnly)) {
    qDebug() << "Error. Refresh file is not open!";
    return nullptr;
  }

  QByteArray data = refreshFile.readAll();
  QJsonDocument jDoc = QJsonDocument::fromJson(data);

  return new QJsonObject(jDoc.object());
}

void MainWindow::checkSavedSession() {
  QJsonObject *jsonRfrsh = openRefreshFile();
  if (jsonRfrsh != nullptr) {
    QString refreshToken = jsonRfrsh->value(QStringLiteral("refresh")).toString();
    QString email = jsonRfrsh->value(QStringLiteral("email")).toString();
    if (refreshToken.isEmpty()) {
      // registration form, auto-filling email if having the one
      if (!email.isEmpty()) {
        ui->mail->setText(email);
      }
    } else {
      // send refresh-request
      if (!email.isEmpty()) {
        ui->mail->setText(email);
        server->sendPostRefreshRequest(email, refreshToken);
      } else {
        qDebug() << "Error. There's a refresh but no email.";
      }
    }
  }
}

void MainWindow::clearAndInitWeekData(const QString &token) {
  clearWeekData();
  initWeekData(token);
}

void MainWindow::initWeekData(const QString &token) {
  QString sDateFirstDayWeek;
  QDateTime dt;
  dt.setDate(arrDays_[0].date);
  sDateFirstDayWeek.setNum(dt.toMSecsSinceEpoch());

  // get breeks lines
  QUrl url(Network::serverUrl + Network::getAllLinesInWeekUrl + sDateFirstDayWeek);
  server->sendGetRequestWithBearerToken(url, token);

  // get TTElements
  for (const auto &day : arrDays_) {
    QString sDate;
    QDateTime dt;
    dt.setDate(day.date);
    sDate.setNum(dt.toMSecsSinceEpoch());

    url = Network::serverUrl + Network::getTTElementsForDayUrl + sDate;
    server->sendGetRequestWithBearerToken(url, token);
  }

  // get notes
  url = Network::serverUrl + Network::getNoteByDateAndPageUrl + sDateFirstDayWeek + "/" +
        QString::number(ui->note->getNumberCurrentFile());
  server->sendGetRequestWithBearerToken(url, token);

  // get image location
  url = Network::serverUrl + Network::getImageUrl + sDateFirstDayWeek;
  server->sendGetRequestWithBearerToken(url, token);
}

void MainWindow::initBreeksLines(const QList<breeksData_t> &listOfLines) {
  for (const auto &breeksLine : listOfLines) {
    bool arrConditions[6] = {false};
    QString sConditions = QStringLiteral("000000").number(breeksLine.conditions, 2);
    while (sConditions.length() != 6) {
      sConditions = "0" + sConditions;
    }
    for (int i = 0; i < DAYS_COUNT; ++i) {
      arrConditions[i] = sConditions.at(i).digitValue();
    }

    int arrStates[6];
    QString sStates = QStringLiteral("000000").number(breeksLine.states, 4);
    while (sStates.length() != 6) {
      sStates = "1" + sStates;
    }

    for (int i = 0; i < DAYS_COUNT; ++i) {
      arrStates[i] = sStates.at(i).digitValue();
    }

    recieveBreeksZoneData(arrConditions, breeksLine, arrStates);
  }
}

void MainWindow::initTTElements(const QList<elementData_t> &listOfTTElements) {
  for (const auto &element : listOfTTElements) {
    bool arr[6] = {false};
    QDateTime date = QDateTime();
    date.setMSecsSinceEpoch(element.date);
    int dayOfWeek = date.date().dayOfWeek() - 1;

    arr[dayOfWeek] = true;

    recieveTimeTableZoneData(arr, element, false);
  }
}

void MainWindow::initNote(note_t &note) {
  ui->note->fillCharsAndSetTextt(note.text, note.charStyleVector);
}

void MainWindow::initImage(const image_t &image) {
  setImage(image.imageLocation);
}

void MainWindow::mousePressEvent(QMouseEvent * /*event*/) {}

void MainWindow::moveTimetableElement() {
  while (isElementDrag_) {
    QPoint pos = this->mapToGlobal(this->cursor().pos());

    int posMain = this->mapToGlobal(ui->groupBoxWorkZone->pos()).x();

    if (pos.x() > posMain && pos.x() < posMain + 700) {
      QPoint pos1 = bigWidgetInWorkZone_->mapFromGlobal(this->cursor().pos());
      workZoneScrollArea_->ensureVisible(pos1.x() - 1, workZoneScrollArea_->verticalScrollBar()->sliderPosition());
      QThread::msleep(2);
    } else if (pos.x() > posMain + ui->groupBoxWorkZone->width() - 700 &&
               pos.x() < posMain + ui->groupBoxWorkZone->width()) {
      QPoint pos1 = bigWidgetInWorkZone_->mapFromGlobal(this->cursor().pos());
      workZoneScrollArea_->ensureVisible(pos1.x() + 1, workZoneScrollArea_->verticalScrollBar()->sliderPosition());
      QThread::msleep(2);
    }
  }
}

void MainWindow::mousePressedByDragElement() {
  isElementDrag_ = true;

#if QT_VERSION >= 0x060000
  auto future = QtConcurrent::task([this]() {
    moveTimetableElement();
  });
#else
  QFuture<void> future = QtConcurrent::run(this, &MainWindow::moveTimetableElement);
#endif

  Q_UNUSED(future)
}

void MainWindow::dropNoChanges() {
  isElementDrag_ = false;
}

void MainWindow::recieveTimeTableZoneData(bool *daysCheck, const elementData_t &newElement, bool withRequest) {
  for (int i = 0; i < 6; i++) {
    if (daysCheck[i] == true) {
      // add new element data to array
      const int newElementIndex = addNewElementToArray(newElement, i, withRequest);

      // increase scroll area of this day
      if (arrDays_[i].elementsCount < 3) {
        // arrDays_[i].widgetDay->setFixedSize(DAY_WIDTH_,
        // arrDays_[i].groupBoxElementsHeight - 30);
      } else {
        arrDays_[i].groupBoxElementsHeight = ELEMENT_HEIGHT_ * (arrDays_[i].elementsCount + 1) + 25;
        arrDays_[i].widgetDay->setFixedHeight(arrDays_[i].groupBoxElementsHeight);
      }

      // add new element to layout
      addNewElementToLayout(i, newElementIndex);

      arrDays_[i].elementsScaledCount = 0;
      arrDays_[i].labelElementsCount->setText(QString::number(arrDays_[i].elementsCount));

      ++timetableElementsCount_;
    }
  }
}

void MainWindow::recieveBreeksZoneData(bool *daysCheck, const breeksData_t &newElement, int *states) {
  // if we arleady have breeks zone with this name
  for (breeksZone_t &value : arrBreeksZones_) { // value == zone
    if (value.breekText->toPlainText() == newElement.text) {
      for (int i = 0; i < DAYS_COUNT; i++) {
        Breek *breek = value.arrBreeks[i];
        if (breek->getEmojiNum() != newElement.arrNEmoji[i]) {
          breek->setEmoj(newElement.arrNEmoji[i]);
          breek->changeBreekState();
          breek->changeBreekState();

          if (daysCheck[i] == true && breek->getState() == false) {
            breek->changeBreekState();
          }
        } else {
          if (daysCheck[i] == true && breek->getState() == false) {
            breek->changeBreekState();
          }
        }
      }

      if (iCurrentDay_ < DAYS_COUNT && value.arrBreeks[iCurrentDay_]->getState()) {
        value.arrBreeksZoneDays[iCurrentDay_]->setStyleSheet(
            QStringLiteral("background: #b3defc; border-radius: 4px;"));
      }

      return;
    }
  }

  // if we don't have breeks zone with this name
  breeksZone_t newZone;
  newZone.zoneIndex = breeksZonesCount_;
  allocateMemoryForBreeks(&newZone); // breeks constructor call here (6 constructors)
  setBreeksZone(&newZone);           // make invisible and connect move event
  setDaysConnect(&newZone);          // arrBreeksZoneDays[6] connect clicked() with
                                     // breeks->changeBreekState

  // newZone.breekText->setFocus();
  newZone.idOnServer = newElement.idOnServer;
  newZone.breekText->fillCharsAndSetText(newElement.text, newElement.charStyleVector);
  newZone.breekText->moveCursor(QTextCursor::Start);
  newZone.breekText->verticalScrollBar()->minimum();

  if (iCurrentDay_ < DAYS_COUNT && newZone.arrBreeks[iCurrentDay_]->getState()) {
    newZone.arrBreeksZoneDays[iCurrentDay_]->setStyleSheet(QStringLiteral("background: #b3defc; border-radius: 4px;"));
  }

  workZoneLayout_->addWidget(newZone.breeksZoneGroupBox, workZoneLayout_->rowCount() + 1, 0, 1, 6, Qt::AlignCenter);

  breeksDescriptionZoneLayout_->addWidget(newZone.breeksDescriptionGroupBox,
                                          breeksDescriptionZoneLayout_->rowCount() + 1, 0, Qt::AlignCenter);

  arrBreeksZones_.push_back(newZone);
  connect(server.get(), &Network::ServerConnection::initBLidOnServer, this, &MainWindow::setBLIdOnServer);
  connect(arrBreeksZones_[breeksZonesCount_].breekText, &QTextEdit::textChanged,
          arrBreeksZones_[breeksZonesCount_].arrBreeksZoneDays[0], &DescriptionZoneDayButton::sendPutRequestBl);

  if (breeksZonesCount_ == 0) {
    bigWidgetHeight_ += 125;
  } else {
    bigWidgetHeight_ += 125;
  }

  bigWidgetInWorkZone_->setFixedHeight(bigWidgetHeight_);
  bigWidgetInBreeksDescriptionZone_->setFixedHeight(bigWidgetHeight_);

  ++breeksZonesCount_;

  // ADD BREEKS

  int i = 0;
  while (i < 11) {
    // breeks added to layout here
    arrBreeksZones_[breeksZonesCount_ - 1].breeksZoneLayout->addWidget(newZone.arrBreeks[i / 2], 1, i);

    // emojiHub
    EmojiHub *emojiHub = new EmojiHub;
    if (i != 10) {
      arrBreeksZones_[breeksZonesCount_ - 1].breeksZoneLayout->addWidget(emojiHub, 1, i + 1, Qt::AlignCenter);
    }

    arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2]->setEmoj(newElement.arrNEmoji[i / 2]);
    arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2]->setIndex(newZone.zoneIndex, i / 2);

    if (daysCheck[i / 2] == true) {
      arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2]->changeBreekState();
    }

    if (states != nullptr) {
      arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2]->setColorState(Conditions(states[i / 2]));

      if (states[i / 2] != 1 && states[i / 2] != 2) {
        arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2]->connectToQml(newElement.arrNEmoji[i / 2],
                                                                              Conditions(states[i / 2]));
        int iState = 0;
        switch (states[i / 2]) {
        case 0:
          iState = 2;
          break;
        case 1:
          iState = 0;
          break;
        case 3:
          iState = 1;
          break;
        }
        changeBreeksZoneLilDayState(breeksZonesCount_ - 1, i / 2, iState, false);
      }
    }

    if (i / 2 < 5) {
      connect(arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2], &Breek::doubleClicked, emojiHub,
              &EmojiHub::showThis);

      connect(emojiHub, &EmojiHub::changeEmoji, arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2],
              &Breek::changeEmoji);

      connect(emojiHub, &EmojiHub::close, arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2],
              &Breek::closeEmojiHub);

      connect(emojiHub, &EmojiHub::open, arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2], &Breek::openEmojiHub);
    }
    if (i / 2 + 1 == 5) {
      connect(arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2 + 1], &Breek::doubleClicked, emojiHub,
              &EmojiHub::showThisSt);

      connect(emojiHub, &EmojiHub::changeEmojiSt, arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2 + 1],
              &Breek::changeEmoji);

      connect(emojiHub, &EmojiHub::closeSt, arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2 + 1],
              &Breek::closeEmojiHub);

      connect(emojiHub, &EmojiHub::openSt, arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2 + 1],
              &Breek::openEmojiHub);
    }

    connect(arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2], &Breek::setZoneFocus, this,
            &MainWindow::setBreeksDescriptionZoneFocus);

    connect(arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2], &Breek::changeEmojiOnServer, this,
            &MainWindow::sendPutRequestBl);

    connect(arrBreeksZones_[breeksZonesCount_ - 1].arrBreeks[i / 2],
            &Breek::sendPutRequest,
            arrBreeksZones_[breeksZonesCount_ - 1].arrBreeksZoneDays[0],
            &DescriptionZoneDayButton::sendPutRequestBl);

    i += 2;
  }
}

void MainWindow::recieveDayAndElementIndex(const int dayElementIndex, const int elementIndex, bool isDeleting) {
  // SERVER REQUEST
  if (isDeleting) {
    QUrl url = QUrl(Network::serverUrl + Network::deleteTTElementUrl + '/' +
                    QString::number(arrDaysData_[dayElementIndex][elementIndex].idOnServer));

    server->sendDeleteRequestWithBearerToken(url, userData->getAccessToken());
  }
  //---------

  auto item = arrDays_[dayElementIndex].layoutDayElements->itemAt(elementIndex);

  if (!item->widget()->isHidden()) {
    arrDays_[dayElementIndex].layoutDayElements->removeItem(item);
    arrDays_[dayElementIndex].layoutDayElements->removeWidget(item->widget());

    if (item->widget()->size().height() > 100) {
      --arrDays_[dayElementIndex].elementsScaledCount;
    }

    delete item->widget();
    delete item;
    arrDays_[dayElementIndex].layoutDayElements->update();

    --arrDays_[dayElementIndex].elementsCount;
    --timetableElementsCount_;

    if (arrDays_[dayElementIndex].elementsCount <= 3 && arrDays_[dayElementIndex].elementsScaledCount < 3) {
      arrDays_[dayElementIndex].groupBoxElementsHeight = 370;
    } else {
      arrDays_[dayElementIndex].groupBoxElementsHeight =
          ELEMENT_HEIGHT_ * (arrDays_[dayElementIndex].elementsCount - arrDays_[dayElementIndex].elementsScaledCount) +
          (ELEMENT_HEIGHT_ + 30) * arrDays_[dayElementIndex].elementsScaledCount + 25;
      // arrDays_[dayElementIndex].groupBoxElementsHeight =
      // ELEMENT_HEIGHT_ * arrDays_[dayElementIndex].elementsCount + 25;
    }
    arrDays_[dayElementIndex].widgetDay->setFixedHeight(arrDays_[dayElementIndex].groupBoxElementsHeight);

    if (arrDays_[dayElementIndex].elementsCount == 0) {
      arrDays_[dayElementIndex].labelElementsCount->setText(QLatin1String(""));
    } else {
      arrDays_[dayElementIndex].labelElementsCount->setText(QString::number(arrDays_[dayElementIndex].elementsCount));
    }

    iterType start = arrDaysData_[dayElementIndex].begin();
    arrDaysData_[dayElementIndex].erase(start + elementIndex);

    for (int i = elementIndex; i < arrDays_[dayElementIndex].elementsCount; ++i) {
      ElementTemplate *a =
          qobject_cast<ElementTemplate *>(arrDays_[dayElementIndex].layoutDayElements->itemAt(i)->widget());
      a->setElementIndex(i);
    }
  } else {
    item->widget()->show();
  }
}

void MainWindow::recieveDayAndElementIndexAndTagColor(const int dayIndex, const int elementIndex, const int colorNum) {
  arrDaysData_[dayIndex][elementIndex].tagColorNum = colorNum;
}

void MainWindow::recieveUsername() {
  this->showMaximized();
}

void MainWindow::recieveMimeData(const elementData_t &data, const QPixmap &pixMap) {
  mimeData_.setText(data.text);
  dragElement_ = pixMap;
}

void MainWindow::dropElement(const int dayNumber, const int /*dayIndex*/, const int /*elemIndex*/,
                             const elementData_t &elemData) {
  dropNoChanges();

  bool daysCheck_[6];
  for (int i = 0; i < 6; i++) {
    if (i == dayNumber) {
      daysCheck_[i] = true;
    } else {
      daysCheck_[i] = false;
    }
  }

  recieveTimeTableZoneData(daysCheck_, elemData);
}

void MainWindow::sendElementsHeight(const int /*height*/, const int index) {
  if (arrDays_[index].elementsCount < 3) {
    arrDays_[index].widgetDay->setFixedHeight(ELEMENT_HEIGHT_ * 3);
  }
}

void MainWindow::on_reg_clicked() {
  if (ui->mailReg->text().isEmpty() || ui->passwordReg->text().isEmpty() || ui->password2Reg->text().isEmpty()) {
    QMessageBox message;
    message.setText(QStringLiteral("Вы заполнили не все поля"));
    message.exec();

    return;
  }
  if (ui->passwordReg->text() != ui->password2Reg->text()) {
    QMessageBox message;
    message.setText(QStringLiteral("Пароли не совпадают"));
    message.exec();

    return;
  }

  QJsonObject json;
  json.insert(QStringLiteral("userName"), ui->mailReg->text());
  json.insert(QStringLiteral("password"), ui->passwordReg->text());

  QUrl url = QUrl(Network::serverUrl + Network::registrationUrl);
  QJsonDocument jsonDoc(json);
  server->sendPostRequest(url, jsonDoc.toJson());

  ui->message->show();
  ui->mailReg->clear();
  ui->passwordReg->clear();
  ui->password2Reg->clear();
}

void MainWindow::on_login_clicked() {
  if (ui->mail->text().isEmpty() || ui->password->text().isEmpty()) {
    QMessageBox message;
    message.setText(QStringLiteral("Вы заполнили не все поля"));
    message.exec();

    return;
  }

  server->sendAuthRequest(ui->mail->text(), ui->password->text());
}

void MainWindow::loginReply(bool login) {
  if (login) {
    ui->authFrom->hide();
    ui->mail->clear();
    ui->password->clear();
    ui->demoLable->hide();
    ui->hideCalendar->show();
  } else {
    QMessageBox message;
    message.setText(QStringLiteral("Неверный логин или пароль"));
    message.exec();
    ui->password->clear();
  }
}

void MainWindow::clearWeekData() {
  // clear breeks lines
  while (breeksZonesCount_ != 0) {
    deleteBreeksZoneClientOnly(breeksZonesCount_ - 1);
  }

  // clear TTElements
  for (int i = 0; i < DAYS_COUNT; ++i) {
    while (arrDays_[i].elementsCount != 0) {
      recieveDayAndElementIndex(i, arrDays_[i].elementsCount - 1, false);
    }
  }
  // iCurrentDay_ = 0;

  // clear Notes
  deleteNotes();

  // delete Image(set default)
  deleteImage();
}

void MainWindow::deleteNotes() {
  //  int pageNum = ui->note->getNumberCurrentFile();
  ui->note->setCharCounter(0);
  ui->note->clear();
}

void MainWindow::deleteImage() {
  setImage(defaultImageName_);
}

void MainWindow::on_prevWeekButton_clicked() {
  setDayInfo(currentDate_.addDays(-7));
  clearAndInitWeekData(server->getUserData()->getAccessToken());
}

void MainWindow::on_nextWeekButton_clicked() {
  setDayInfo(currentDate_.addDays(7));
  clearAndInitWeekData(server->getUserData()->getAccessToken());
}

void MainWindow::on_logoutButton_clicked() {
  logout();
}

void MainWindow::on_hideCalendar_clicked() {
  if (!calendarWidget->isHidden()) {
    ui->hideCalendar->setStyleSheet(
        QStringLiteral("border-image:url(:/Images/Front/Images/show.png); background: none;"));
    calendarWidget->hide();
    calendarScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  } else {
    ui->hideCalendar->setStyleSheet(
        QStringLiteral("border-image:url(:/Images/Front/Images/hide.png); background: none;"));
    calendarWidget->show();
    calendarScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  }
}

void MainWindow::on_showInfoButton_clicked() {
  QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/BreeksApp/Breeks-presentation")));
}
