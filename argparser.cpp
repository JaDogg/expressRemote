/****************************************************************************
**
** Copyright (C) 2014 Bhathiya Perea
**
** This file is part of Express Remote
**
**
****************************************************************************/

#include "argparser.h"

ArgParser::ArgParser() {

  QStringListIterator argIter(QApplication::arguments());

  // eat first argument
  if (argIter.hasNext()) {
    argIter.next();
  } else {
    isOK = false;
  }
  // parse
  while (argIter.hasNext()) {
    QString cur = argIter.next();
    if (cur.startsWith("-")) {

      if (argsCountMap.contains(cur)) {
        argsCountMap[cur] = argsCountMap[cur] + 1;
      } else {
        argsCountMap[cur] = 1;
      }

      if (!argsMap.contains(cur)) {
        argsMap[cur] = QStringList();
      }

      if (argIter.hasNext()) {
        argsMap[cur] << argIter.next();
      } else {
        // no arg for the command
        isOK = false;
        break;
      }

    } else {
      otherArgs << cur;
    }
  }
}

int ArgParser::GetCntOf(const QString &key) {
  if (argsCountMap.contains(key)) {
    return argsCountMap[key];
  } else {
    return 0;
  }
}

QStringList ArgParser::GetPositionalArguments() { return otherArgs; }

QStringList ArgParser::TakeArgument(const QString &key) {
  argsCountMap.remove(key);
  return argsMap.take(key);
}

QHash<QString, int> ArgParser::GetArgumentsCountMap() { return argsCountMap; }

QHash<QString, QStringList> ArgParser::GetArgumentsMap() { return argsMap; }

bool ArgParser::IsOK() { return isOK; }
