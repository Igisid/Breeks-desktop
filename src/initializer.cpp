//#include "init.h"

#include <QApplication>
#include <QDir>
#include <QIcon>

static void init() {
  QString path = QDir::current().path() + "/eeicon.ico"; // Release
  QIcon icon(path);
  qApp->setWindowIcon(icon);
}

Q_COREAPP_STARTUP_FUNCTION(init)
