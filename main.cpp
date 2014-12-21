/****************************************************************************
**
** Copyright (C) 2014 Bhathiya Perea
**
** This file is part of Express Remote
**
**
****************************************************************************/



#include <QCoreApplication>
#include <QObject>
#include <QStringList>
#include <QDebug>
#include <iostream>
#include "argparser.h"
#include "expressremote.h"

void printHelp() {
  std::cout << "Express Remote                  " << std::endl;
  std::cout << "     -Written by Bhathiya Perera" << std::endl;
  std::cout << "--------------------------------" << std::endl;
  std::cout << "Usage :                         " << std::endl;
  std::cout << "remote -u <user> -p <pass> "
               "-h <host> [-P <port> (default:22)] "
               "-c <command> -f <file_to_upload> "
               "[-d <destination_full path>]" << std::endl;
  std::cout << "--------------------------------" << std::endl;
  std::cout << "-c, -f there can only be one of "
               "these options used at a time." << std::endl;
  std::cout << "-f option may have a single -d "
               "option." << std::endl;
  std::cout << "--------------------------------" << std::endl << std::endl;
  std::cout << "Credits:" << std::endl;
  std::cout << "QTCreator's QSsh code" << std::endl;
  std::cout << "Botan library" << std::endl;
  std::cout << "Remote icon from "
               "http://www.iconarchive.com/show/"
               "vista-hardware-devices-icons-by-icons-land/"
               "Remote-Control-icon.html" << std::endl;
  std::cout << std::endl;
}
int main(int argc, char *argv[]) {

  QCoreApplication app(argc, argv);

  ArgParser args;

  bool success = true;
  success &= (args.GetCntOf("-h") == 1);
  success &= (args.GetCntOf("-u") == 1);
  success &= (args.GetCntOf("-p") == 1);
  success &= (args.GetCntOf("-P") == 0 || args.GetCntOf("-P") == 1);
  success &= ((args.GetCntOf("-c") == 1 && args.GetCntOf("-f") == 0) ||
              (args.GetCntOf("-c") == 0 && args.GetCntOf("-f") == 1));
  if (args.GetCntOf("-f") == 1) {
    success &= (args.GetCntOf("-d") == 1 || args.GetCntOf("-d") == 0);
  }

  if (!success) {
    printHelp();
    return 1;
  }

  QString host = args.TakeArgument("-h").at(0);
  QString user = args.TakeArgument("-u").at(0);
  QString pass = args.TakeArgument("-p").at(0);
  uint uiPort = 22;
  if (args.GetCntOf("-P") == 1) {
    bool converted = false;
    QString port = args.TakeArgument("-P").at(0);
    uiPort = port.toUInt(&converted);
    success &= converted;
  }
  QString cmd("");
  QString file("");
  QString dest("");
  bool isUpload = false;
  if (args.GetCntOf("-f") == 1) {
    file = args.TakeArgument("-f").at(0);
    isUpload = true;
  }
  if (args.GetCntOf("-c") == 1) {
    cmd = args.TakeArgument("-c").at(0);
  }
  if (args.GetCntOf("-d") == 1) {
    dest = args.TakeArgument("-d").at(0);
  }

  success &= (args.GetPositionalArguments().length() == 0);
  success &= (args.GetArgumentsMap().size() == 0);

  if (!success) {
    printHelp();
    return 1;
  }

  ExpressRemote remote;
  remote.SetData(host, uiPort, user, pass, cmd, file, dest);
  if (isUpload) {
    remote.StartRemoteUpload();
  } else {
    remote.StartRemoteProcess();
  }

  return app.exec();
}
