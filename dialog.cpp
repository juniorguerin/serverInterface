#include <QtGui>
#include "dialog.h"

Dialog::Dialog(QWidget *parent)
  : QDialog(parent)
{
  rootLabel = new QLabel(tr("&root path:"));
  rootLineEdit = new QLineEdit;
  rootLabel->setBuddy(rootLineEdit);

  portLabel = new QLabel(tr("&port number:"));
  portLineEdit = new QLineEdit;
  portLineEdit->setValidator(new QIntValidator(1, 65535, this));
  portLabel->setBuddy(portLineEdit);

  velocityLabel = new QLabel(tr("&velocity (B/s):"));
  velocityLineEdit = new QLineEdit;
  velocityLineEdit->setValidator(new QIntValidator(1, 10240000, this));
  velocityLabel->setBuddy(velocityLineEdit);

  okButton = new QPushButton(tr("&ok"));
  okButton->setDefault(true);
  okButton->setEnabled(false);
  
  cancelButton = new QPushButton(tr("&cancel"));
  cancelButton->setEnabled(true);

  closeButton = new QPushButton(tr("&close"));
  closeButton->setEnabled(true);
 
  connect(rootLineEdit, SIGNAL(textChanged(const QString &)), this,
          SLOT(alterOkButtonStatus()));
  connect(portLineEdit, SIGNAL(textChanged(const QString &)), this,
          SLOT(alterOkButtonStatus()));
  connect(velocityLineEdit, SIGNAL(textChanged(const QString &)), this,
          SLOT(alterOkButtonStatus()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(closeClicked()));

  QHBoxLayout *rootHLayout = new QHBoxLayout;
  rootHLayout->addWidget(rootLabel);
  rootHLayout->addWidget(rootLineEdit);

  QHBoxLayout *portHLayout = new QHBoxLayout;
  portHLayout->addWidget(portLabel);
  portHLayout->addWidget(portLineEdit);
  
  QHBoxLayout *velocityHLayout = new QHBoxLayout;
  velocityHLayout->addWidget(velocityLabel);
  velocityHLayout->addWidget(velocityLineEdit);

  QHBoxLayout *buttonHLayout = new QHBoxLayout;
  buttonHLayout->addWidget(okButton);
  buttonHLayout->addWidget(cancelButton);
  buttonHLayout->addWidget(closeButton);
 
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(rootHLayout);
  mainLayout->addLayout(portHLayout);
  mainLayout->addLayout(velocityHLayout);
  mainLayout->addLayout(buttonHLayout);
  setLayout(mainLayout);

  setWindowTitle(tr("Server configurations"));
  setFixedWidth(sizeHint().width());
}

void Dialog::okClicked()
{
  long pidNumber; 
  
  if ((0 < (pidNumber = verifyPidFile())) && !verifyFields())
  {
    createConfigFile();
    sendSignal((pid_t) pidNumber);
  }
}

void Dialog::sendSignal(pid_t pid)
{
  QMessageBox messageBox;
  
  if (kill(pid, SIGHUP))
    messageBox.information(this, "Could not signal",
                           "Could not signal");
}

int Dialog::createConfigFile()
{
  QMessageBox messageBox;
  string root(rootLineEdit->text().toStdString());
  string port(portLineEdit->text().toStdString());
  string velocity(velocityLineEdit->text().toStdString());
  string configMessage(root + "\n" + port + "\n" + velocity + "\n");
  QString configPathQStr(configPathStr.c_str());
  QString configFileQStr(configFileStr.c_str());
  QFile configFile(configPathQStr + configFileQStr);

  if (configFile.exists())
    configFile.remove();

  if (!configFile.open(QIODevice::WriteOnly))
  {
    messageBox.information(this, "Could not open",
                           "Could not open the config file");
    return -1;
  }

  configFile.write(configMessage.c_str());
  configFile.close();

  return 0;
}

long Dialog::verifyPidFile()
{
  QMessageBox messageBox;
  char *charPid = new char[pidLength];
  char *endPtr;
  long pidNumber;
  int charPidLen;
  QString configPathQStr(configPathStr.c_str());
  QString pidFileQStr(pidFileStr.c_str());
  QFile pidFile(configPathQStr + pidFileQStr);
  
  if (!pidFile.exists())
  {
    messageBox.information(this, "File not found",
                           "Could not find the PID file");
    return -1;
  }

  if (!pidFile.open(QFile::ReadOnly))
  {
    messageBox.information(this, "Could not open",
                           "Could not open the pid file");
    return -1;
  }

  if (0 >= pidFile.readLine(charPid, sizeof(charPid)))
  {
    messageBox.information(this, "Invalid read",
                           "Could not read");
    return -1;
  }

  /* readLine sempre coloca \n no final, se nao encontrar */
  charPidLen = strlen(charPid);
  pidNumber = strtol(charPid, &endPtr, numberBase);
  if (charPidLen != endPtr - charPid)
  {
    messageBox.information(this, "Invalid PID",
                           "Invalid PID number");
    return -1;
  }

  pidFile.close();
  return  pidNumber;
}

int Dialog::verifyFields()
{
  QMessageBox messageBox;
  string velStr(velocityLineEdit->text().toStdString());
  string portStr(portLineEdit->text().toStdString());
  QString rootStr = rootLineEdit->text();
  QFile rootPath(rootStr);
  int vel = strtol(velStr.c_str(), NULL, numberBase);
  int port = strtol(portStr.c_str(), NULL, numberBase);

  if (rootStr.size() > 0 && !rootPath.exists())
  {
    messageBox.information(this, "Invalid root path",
                           "It's not a valid server root path");
    return -1;
  }

  if (velStr.size() > 0 && (!vel || vel > 10240000))
  {
    messageBox.information(this, "Invalid parameter", 
                           "Invalid velocity number!"
                           "\nInterval: (0, 10240000]");
    return -1;
  }

  if (portStr.size() > 0 && (!port || port < 1024 || port > 65535))
  {
    messageBox.information(this, "Invalid parameter", 
                           "Invalid port number!"
                           "\nInterval: (1024, 65535]");
    return -1;
  }

  return 0;
}

void Dialog::closeClicked()
{
  QMessageBox::StandardButton reply;
  QMessageBox messageBox;

  reply = messageBox.question(this, "Close", "Are you sure?", 
                              QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes)
    close();
}

void Dialog::cancelClicked()
{
  QMessageBox messageBox;
  QMessageBox::StandardButton reply;

  reply = messageBox.information(this, "Clear fileds", "Are you sure?",
                                 QMessageBox::Yes | QMessageBox::No);
  if (QMessageBox::Yes == reply)
  {
    rootLineEdit->clear();
    portLineEdit->clear();
    velocityLineEdit->clear();
  }
}

void Dialog::alterOkButtonStatus()
{
  QString root = rootLineEdit->text();
  QString port = portLineEdit->text();
  QString velocity = velocityLineEdit->text();

  if (root.size() > 0 || port.size() > 0 || velocity.size() > 0)
    okButton->setEnabled(true);
  else
    okButton->setEnabled(false);
}
