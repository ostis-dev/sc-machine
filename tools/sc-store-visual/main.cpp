#include <QtGui/QApplication>
#include <QLayout>

#include "definition.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  MainWindow w;
  w.show();

  return a.exec();
}
