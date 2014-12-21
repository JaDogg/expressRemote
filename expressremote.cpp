/****************************************************************************
**
** Copyright (C) 2014 Bhathiya Perea
**
** This file is part of Express Remote
**
**
****************************************************************************/

#include <QCoreApplication>
#include <QFileInfo>
#include <iostream>
#include "expressremote.h"
#include <sshpseudoterminal.h>

using namespace QSsh;

ExpressRemote::ExpressRemote()
    : m_connection(nullptr), m_remoteRunner(nullptr),
      m_timeoutTimer(new QTimer(this)) {
  // default timeout is 5000ms, 5s
  m_timeoutTimer->setInterval(5000);
  connect(m_timeoutTimer, SIGNAL(timeout()), SLOT(HandleTimeout()));
}

void ExpressRemote::SetData(const QString &host, uint port, const QString &user,
                            const QString &pass, const QString &cmd,
                            const QString &file, const QString &dest) {
  SshConnectionParameters server;
  server.host = host;
  server.port = port;
  server.password = pass;
  server.userName = user;
  server.timeout = 30;                      // default timeout is 30
  server.options &= ~SshIgnoreDefaultProxy; // do not ignore default proxy
  server.authenticationType =
      SshConnectionParameters::AuthenticationTypeTryAllPasswordBasedMethods;
  m_cmd = cmd;
  m_file = file;
  m_dest = dest;
  m_sshParams = server;
}

void ExpressRemote::StartRemoteProcess() {

  m_remoteRunner = new SshRemoteProcessRunner(this);
  connect(m_remoteRunner, SIGNAL(connectionError()),
          SLOT(HandleConnectionError()));
  connect(m_remoteRunner, SIGNAL(processStarted()),
          SLOT(HandleProcessStarted()));
  connect(m_remoteRunner, SIGNAL(readyReadStandardOutput()),
          SLOT(HandleProcessStdout()));
  connect(m_remoteRunner, SIGNAL(readyReadStandardError()),
          SLOT(HandleProcessStderr()));
  connect(m_remoteRunner, SIGNAL(processClosed(int)),
          SLOT(HandleProcessClosed(int)));
  std::cout << "Executing..."
            << "\"" << qPrintable(m_cmd) << "\"" << std::endl;
  m_remoteRunner->runInTerminal(m_cmd.toUtf8(), SshPseudoTerminal(),
                                m_sshParams);
  m_timeoutTimer->start();
}

void ExpressRemote::StartRemoteUpload() {
  m_connection = new QSsh::SshConnection(m_sshParams);
  connect(m_connection, SIGNAL(connected()), SLOT(HandleConnected()));
  connect(m_connection, SIGNAL(disconnected()), SLOT(HandleDisconnected()));
  connect(m_connection, SIGNAL(error(QSsh::SshError)),
          SLOT(HandleConnectionError()));
  std::cout << "Connecting..." << std::endl;
  m_connection->connectToHost();
  m_timeoutTimer->start();
}
void ExpressRemote::HandleConnected() {
  std::cout << "Connected" << std::endl;
  m_sftp = m_connection->createSftpChannel();

  connect(m_sftp.data(), SIGNAL(initialized()), this,
          SLOT(HandleChannelInitialized()));
  connect(m_sftp.data(), SIGNAL(channelError(QString)), this,
          SLOT(HandleChannelInitializationFailure(QString)));
  connect(m_sftp.data(), SIGNAL(finished(QSsh::SftpJobId, QString)), this,
          SLOT(HandleJobFinished(QSsh::SftpJobId, QString)));
  connect(m_sftp.data(),
          SIGNAL(fileInfoAvailable(QSsh::SftpJobId, QList<QSsh::SftpFileInfo>)),
          SLOT(HandleFileInfo(QSsh::SftpJobId, QList<QSsh::SftpFileInfo>)));
  connect(m_sftp.data(), SIGNAL(closed()), this, SLOT(HandleChannelClosed()));
  m_sftp->initialize();
  m_timeoutTimer->start();
}
void ExpressRemote::HandleChannelClosed() {
  std::cout << "SFTP channel closed" << std::endl;
  qApp->quit();
}
void ExpressRemote::HandleFileInfo(SftpJobId, QList<SftpFileInfo>) {
  std::cout << "FileInfo" << std::endl;
}
void ExpressRemote::HandleJobFinished(SftpJobId, QString err) {
  if (err.isEmpty()) {
    std::cout << "SFTP job finished successfully" << std::endl;
  } else {
    std::cerr << "SFTP job finished with error '" << qPrintable(err) << "'"
              << std::endl;
  }
  qApp->quit();
}
void ExpressRemote::HandleChannelInitializationFailure(QString err) {
  std::cerr << "SFTP initialization failed " << qPrintable(err) << std::endl;
  qApp->quit();
}
void ExpressRemote::HandleChannelInitialized() {
  std::cout << "SFTP channel initialized" << std::endl;
  QFileInfo fileInfo(m_file);
  if (!fileInfo.exists()) {
    std::cerr << "File '" << qPrintable(m_file) << "' does not exist"
              << std::endl;
    qApp->quit();
    return;
  }

  const SftpJobId job =
      m_sftp->uploadFile(fileInfo.absoluteFilePath(),
                         m_dest.isEmpty() ? "/" + fileInfo.fileName() : m_dest,
                         SftpOverwriteMode::SftpOverwriteExisting);
  if (job == SftpInvalidJob) {
    std::cerr << "SFTP job is invalid " << std::endl;
    qApp->quit();
  } else {
    m_timeoutTimer->start();
  }
}

void ExpressRemote::HandleConnectionError() {
  std::cerr << "Connection error occured" << std::endl;
  qApp->quit();
}
void ExpressRemote::HandleDisconnected() {
  std::cout << "Connection disconnected" << std::endl;
  qApp->quit();
}
void ExpressRemote::HandleProcessStarted() {
  std::cout << "Process started" << std::endl;
  std::cout << "---------------------------" << std::endl;
}

void ExpressRemote::HandleProcessStdout() {
  std::cout << m_remoteRunner->readAllStandardOutput().data();
}

void ExpressRemote::HandleProcessStderr() {
  std::cerr << m_remoteRunner->readAllStandardError().data();
}

void ExpressRemote::HandleProcessClosed(int exitStatus) {
  std::cout << "---------------------------" << std::endl;
  std::cout << "Process closed with " << exitStatus << std::endl;
  qApp->quit();
}
void ExpressRemote::HandleTimeout() {
  std::cerr << "Error: Timeout waiting for progress." << std::endl;
  qApp->quit();
}
ExpressRemote::~ExpressRemote() {
  std::cout << "Express remote shutting down.." << std::endl;
  if (m_timeoutTimer->isActive()) {
    m_timeoutTimer->stop();
  }
  delete m_timeoutTimer;
  if (m_connection != nullptr) {
    delete m_connection;
  }
  if (m_remoteRunner != nullptr) {
    delete m_remoteRunner;
  }
}
