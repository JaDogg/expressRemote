/****************************************************************************
**
** Copyright (C) 2014 Bhathiya Perea
**
** This file is part of Express Remote
**
**
****************************************************************************/

#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <QObject>
#include <QStringList>
#include <QApplication>
#include <QStringListIterator>

class ArgParser : public QObject {
  Q_OBJECT
public:
  ArgParser();
  int GetCntOf(const QString &key);
  QStringList GetPositionalArguments();
  QStringList TakeArgument(const QString &key);
  QHash<QString, int> GetArgumentsCountMap();
  QHash<QString, QStringList> GetArgumentsMap();
  bool IsOK();
private:
  QHash<QString, int> argsCountMap;
  QHash<QString, QStringList> argsMap;
  QStringList otherArgs;
  bool isOK = true;

signals:

public slots:
};

#endif // ARGPARSER_H
