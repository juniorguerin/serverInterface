#include <QApplication>
#include "dialog.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  Dialog *dialog = new Dialog;
  dialog->show();
  return app.exec();
}
