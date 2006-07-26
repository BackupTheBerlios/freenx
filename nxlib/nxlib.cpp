/***************************************************************************
                                  nxlib.cpp
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

/*
 0 - invoke nxssh
 1 - connect to server
 2 - list sessions
 3 - prompt if session found
 4 - start session
 5 - close
*/

#include "nxlib.h"

#include <iostream>

#include <QFile>
#define NXSSH_BIN "nxssh"
#define CLIENT_VERSION "1.5.0"
using namespace std;

// Default NoMachine certificate
QByteArray cert("-----BEGIN DSA PRIVATE KEY-----\nMIIBuwIBAAKBgQCXv9AzQXjxvXWC1qu3CdEqskX9YomTfyG865gb4D02ZwWuRU/9\nC3I9/bEWLdaWgJYXIcFJsMCIkmWjjeSZyTmeoypI1iLifTHUxn3b7WNWi8AzKcVF\naBsBGiljsop9NiD1mEpA0G+nHHrhvTXz7pUvYrsrXcdMyM6rxqn77nbbnwIVALCi\nxFdHZADw5KAVZI7r6QatEkqLAoGBAI4L1TQGFkq5xQ/nIIciW8setAAIyrcWdK/z\n5/ZPeELdq70KDJxoLf81NL/8uIc4PoNyTRJjtT3R4f8Az1TsZWeh2+ReCEJxDWgG\nfbk2YhRqoQTtXPFsI4qvzBWct42WonWqyyb1bPBHk+JmXFscJu5yFQ+JUVNsENpY\n+Gkz3HqTAoGANlgcCuA4wrC+3Cic9CFkqiwO/Rn1vk8dvGuEQqFJ6f6LVfPfRTfa\nQU7TGVLk2CzY4dasrwxJ1f6FsT8DHTNGnxELPKRuLstGrFY/PR7KeafeFZDf+fJ3\nmbX5nxrld3wi5titTnX+8s4IKv29HJguPvOK/SI7cjzA+SqNfD7qEo8CFDIm1xRf\n8xAPsSKs6yZ6j1FNklfu\n-----END DSA PRIVATE KEY-----");

NXLib::NXLib(QObject *parent) : QObject(parent)
{
}

NXLib::~NXLib()
{
}

void NXLib::invokeNXSSH(QString publicKey, QString serverHost, bool encryption)
{
	QStringList arguments;
	
	if (publicKey == "default") {
		QFile certFile("/tmp/cert.pub");
		certFile.open(QIODevice::WriteOnly);
		certFile.write(cert);
		certFile.close();
		arguments << "-nx" << "-i" << "/tmp/cert.pub";
	} else {
		arguments << "-i" << publicKey;
	}

	if (encryption == true)
		arguments << "-B";
	
	connect(&nxsshProcess, SIGNAL(started()), this, SLOT(processStarted()));
	connect(&nxsshProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	connect(&nxsshProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(processParseStdout()));
	connect(&nxsshProcess, SIGNAL(readyReadStandardError()), this, SLOT(processParseStderr()));

	nxsshProcess.setEnvironment(nxsshProcess.systemEnvironment());

	arguments << QString("nx@" + serverHost);
	nxsshProcess.start(NXSSH_BIN, arguments);
}

void NXLib::processStarted()
{
	QString message;
	message = tr("Started nxssh process");
	emit callbackMessage(message);
}

void NXLib::processError(QProcess::ProcessError error)
{
	QString message;
	switch (error) {
		case QProcess::FailedToStart:
			message = tr("The process failed to start");
			break;
		case QProcess::Crashed:
			message = tr("The process has crashed");
			break;
		case QProcess::Timedout:
			message = tr("The process timed out");
			break;
		case QProcess::WriteError:
			message = tr("There was an error writing to the process");
			break;
		case QProcess::ReadError:
			message = tr("There was an error reading from the process");
			break;
		case QProcess::UnknownError:
			message = tr("There was an unknown error with the process");
			break;
	}
	
	emit callbackMessage(message);
}
void NXLib::processParseStdout()
{
	QString message = nxsshProcess.readAllStandardOutput().data();
	emit processOutput(message, QProcess::StandardOutput);
	cout << "Stdout: " << message.toStdString() << endl;
}

void NXLib::processParseStderr()
{
	QString message = nxsshProcess.readAllStandardError().data();
	emit processOutput(message, QProcess::StandardError);
	cout << "Stderr: " << message.toStdString() << endl;
}

qint64 NXLib::write(const char *data)
{
	emit processInput(data);
	cout << data << endl;
	return nxsshProcess.write(data, sizeof(data));
}
