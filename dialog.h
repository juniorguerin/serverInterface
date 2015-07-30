#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <climits>
#include <cstdlib>
#include <string>
#include <cstring>
#include <signal.h>

using namespace std;

const string configPathStr("/home/nilson.junior/Documentos/treinamento/serverConfig/");
const string pidFileStr("servidorWeb.pid");
const string configFileStr("servidorWebConfig.txt");
const int pidLength = 10;
const int numberBase = 10;

class QLabel;
class QLineEdit;
class QPushButton;

class Dialog : public QDialog
{
  Q_OBJECT

public:
  Dialog(QWidget *parent = 0);

private slots:
  void alterOkButtonStatus();
  void okClicked();
  void cancelClicked();
  void closeClicked();

private:
  int createConfigFile();
  int verifyFields();
  void sendSignal(pid_t pid);
  long verifyPidFile();
  QLabel *rootLabel;
  QLineEdit *rootLineEdit;
  QLabel *portLabel;
  QLineEdit *portLineEdit;
  QLabel *velocityLabel;
  QLineEdit *velocityLineEdit;
  QPushButton *okButton;
  QPushButton *cancelButton;
  QPushButton *closeButton;
};

#endif
