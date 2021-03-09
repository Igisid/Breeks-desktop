#include "Front/mainwindow.h"

int MainWindow::addNewElementToArray(const elementData_t& newElement, const int index,
                                     bool withRequest) {
  if (newElement.idOnServer != -1 && withRequest) {
    //server request
    QJsonObject json;
    json.insert("tagColorNum", newElement.tagColorNum);
    json.insert("mainText", newElement.text);

    QJsonArray jArr;
    foreach(charStyle_t ch, newElement.charStyleVector) {
      QJsonObject jChar;
      jChar.insert("bold", ch.bold);
      jChar.insert("italic", ch.italic);
      jChar.insert("underline", ch.underline);
      jChar.insert("strike", ch.strike);
      jChar.insert("item", ch.item);
      jChar.insert("star", ch.star);
      jChar.insert("sColor", ch.sColor);
      jChar.insert("spellChecker", ch.spellChecker);
      jArr.push_back(jChar);
    }
    QJsonDocument jDoc;
    jDoc.setArray(jArr);
    json.insert("effects", QString(jDoc.toJson()));

    json.insert("timeFrom", newElement.timeStart);
    json.insert("timeTo", newElement.timeEnd);
    json.insert("date", QDateTime(arrDays_[index].date).toMSecsSinceEpoch());
    QJsonDocument jsonDoc(json);

    QUrl url = QUrl(Network::serverUrl + Network::editTTElementUrl + '/' + QString::number(newElement.idOnServer));
    server->sendPutRequestWithBearerToken(url , jsonDoc.toJson(), userData->getAccessToken());
  }
  //---------

  if (arrDaysData_[index].size() == 0) {
    arrDaysData_[index].push_back(newElement);
    return 0;
  }

  iterType start = arrDaysData_[index].begin();
  iterType end = arrDaysData_[index].end();

  int newElementIndex = 0;

  for (iterType j = start; j < end; ++j) {
    newElementIndex++;

    if (newElement.timeStart > j->timeStart) {
      if (j + 1 == end) {
        arrDaysData_[index].push_back(newElement);
        break;
      }
    }
    else if (newElement.timeStart < j->timeStart) {
      arrDaysData_[index].insert(j, newElement);
      break;
    }
    else if (newElement.timeStart == j->timeStart && newElement.timeEnd < j->timeEnd) {
      arrDaysData_[index].insert(j, newElement);
      break;
    }
    else if ((newElement.timeEnd == j->timeEnd) && (newElement.text < j->text)) {
      arrDaysData_[index].insert(j, newElement);
      break;
    }

    if (j + 1 == end) {
      arrDaysData_[index].push_back(newElement);
      break;
    }
  }

  return newElementIndex;
}

void MainWindow::addNewElementToLayout(const int index, const int newElementIndex) {
  //clear layout
  for (int j = 0; j < arrDays_[index].elementsCount; ++j) {
    auto item = arrDays_[index].layoutDayElements->itemAt(0);
    arrDays_[index].layoutDayElements->removeItem(item);
    arrDays_[index].layoutDayElements->removeWidget(item->widget());
    delete item->widget();
    delete item;
    arrDays_[index].layoutDayElements->update();
  }

  ++arrDays_[index].elementsCount;

  for (int j = 0; j < arrDays_[index].elementsCount; ++j) {
    //create new element object
    ElementTemplate *elem = new ElementTemplate;

    connect(server.get(), &Network::ServerConnection::initTEidOnServer, elem, &ElementTemplate::setId);
    connect(elem, &ElementTemplate::updateId, this, &MainWindow::updateTTElementIdOnServer);
    connect(elem, &ElementTemplate::sendEditRequest, this, &MainWindow::sendPutRequestTte);

    connect(elem, &ElementTemplate::sendMimeData, this, &MainWindow::recieveMimeData);
    connect(elem, &ElementTemplate::dropNoChanges, this, &MainWindow::dropNoChanges);
    connect(elem, &ElementTemplate::defineDayMoveFrom, this, &MainWindow::defineDayMoveFrom);
    connect(elem, &ElementTemplate::deleteItem, this, &MainWindow::recieveDayAndElementIndex);

    elem->setText(arrDaysData_[index][j].text, arrDaysData_[index][j].charStyleVector);
    elem->setTime(arrDaysData_[index][j].timeStart, arrDaysData_[index][j].timeEnd);

    elem->setTagColor(arrDaysData_[index][j].tagColor);
    elem->setIdOnServer(arrDaysData_[index][j].idOnServer);
    elem->setDayAndElementIndex(index, j);

    // for deleting element from arrDaysData_
    connect(elem, &ElementTemplate::sendDayAndElementIndex, this, &MainWindow::recieveDayAndElementIndex);

    // for updating info about element's tag color
    connect(elem,
            &ElementTemplate::sendDayAndElementIndexAndTagColor,
            this,
            &MainWindow::recieveDayAndElementIndexAndTagColor);

    // for change day heigth after scaling element
    connect(elem, &ElementTemplate::changeElementsLayoutHeight, this, &MainWindow::changeElementsLayoutHeight);

    // for changing time
    connect(elem, &ElementTemplate::changeTime, this, &MainWindow::recieveTimetableElementDayAndElemIndexAndTime);

    connect(elem, &ElementTemplate::changeText, this, &MainWindow::recieveTimetableDayAndElementIndexAndText);

    arrDays_[index].layoutDayElements->addWidget(elem, Qt::AlignCenter);

    if (j == newElementIndex) {
      const int a = 150 * newElementIndex; //150 is lucky coefficient
      arrDays_[index].scrollArea->ensureVisible(0, a);
    }
  }
}
