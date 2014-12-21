/****************************************************************************
**
** Copyright (C) 2014 Bhathiya Perea
**
** This file is part of Express Remote
**
**
****************************************************************************/

#ifndef EXPRESSREMOTE_H
#define EXPRESSREMOTE_H

#include <sshconnection.h>
#include <sshremoteprocessrunner.h>
#include <sftpchannel.h>
#include <QObject>
#include <QTimer>

class ExpressRemote : public QObject {
  Q_OBJECT
public:
  ExpressRemote();
  ~ExpressRemote();
  void SetData(const QString &host, uint port, const QString &user,
               const QString &pass, const QString &cmd, const QString &file,
               const QString &dest);
  void StartRemoteProcess();
  void StartRemoteUpload();

private:
  QSsh::SshConnectionParameters m_sshParams;
  QSsh::SshConnection *m_connection;
  QSsh::SshRemoteProcessRunner *m_remoteRunner;
  QSharedPointer<QSsh::SftpChannel> m_sftp;
  QTimer *const m_timeoutTimer;
  QString m_cmd;
  QString m_file;
  QString m_dest;
signals:

public slots:

private slots:
  void HandleConnectionError();             // connection, process
  void HandleConnected();                   // connection
  void HandleDisconnected();                // connection
  void HandleProcessStarted();              // process
  void HandleProcessStdout();               // process
  void HandleProcessStderr();               // process
  void HandleProcessClosed(int exitStatus); // process
  void HandleTimeout();                     // timer
  void HandleChannelClosed();               // channel
  void HandleFileInfo(QSsh::SftpJobId, QList<QSsh::SftpFileInfo>); // channel
  void HandleJobFinished(QSsh::SftpJobId, QString err);            // channel
  void HandleChannelInitializationFailure(QString err);            // channel
  void HandleChannelInitialized();                                 // channel
};

#endif // EXPRESSREMOTE_H
