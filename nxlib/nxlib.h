/***************************************************************************
                                  nxlib.h
                             -------------------
    begin                : Sat 22nd July 2006
    copyright            : (C) 2006 by George Wright
    email                : gwright@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _NXLIB_H_
#define _NXLIB_H_

#include <QProcess>

class NXLib : public QObject
{
	Q_OBJECT
	public:
		NXLib(QObject *parent = 0);
		~NXLib();

		// publicKey is the path to the ssh public key file to authenticate with. Pass "default" to use the default NoMachine key
		// serverHost is the hostname of the NX server to connect to
		// encryption is whether to use an encrypted NX session
		void invokeNXSSH(QString publicKey = "default", QString serverHost = "", bool encryption = true);

		// Overloaded to give callback data on write
		qint64 write (const char *data);
	public slots:
		void processStarted();
		void processError(QProcess::ProcessError);
		
		void processParseStdout();
		void processParseStderr();
	signals:
		void callbackMessage(QString);
		void processOutput(QString, QProcess::ProcessChannel);
		void processInput(QString);
	private:
		QProcess nxsshProcess;
};

#endif
