/*
  HttpJob.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2011-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HTTPJOB_H
#define HTTPJOB_H

#include <QObject>
#include <QUrl>

namespace QKeychain {
class Job;
}

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QAuthenticator;

class HttpJob : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError = 0,
        Canceled,
        NotConfigured,
        AuthenticationFailed,
        SomethingWentWrong,
        HostNotFound
    };

    enum PasswordRequestReason {
        NoPasswordFound,
        PasswordIncorrect
    };

    explicit HttpJob(QObject *parent = nullptr);
    ~HttpJob() override;

    QString username() const;
    void setUsername(const QString &value);

    QString password() const;
    void setPassword(const QString &value);

    QString errorString() const;
    int error() const;

    void start();
    void cancel();
    void provideRequestedPassword(const QString &password);
    void passwordRequestCanceled();

Q_SIGNALS:
    void finished(HttpJob *);
    /**
     * the actual communication was started
     */
    void transferStarted();

    /**
     * The job requests the password from the user
     *
     * Must be replied to via provideRequestedPassword() or passwordRequestCanceled() to continue.
     */
    void passwordRequested(PasswordRequestReason);

protected Q_SLOTS:
    virtual void executeRequest(QNetworkAccessManager *) = 0;

protected:
    void emitFinishedOrRestart();
    void setErrorAndEmitFinishedOrRestart(int code, const QString &errorString);
    void setErrorFromReplyAndEmitFinishedOrRestart(QNetworkReply *reply);

private Q_SLOTS:
    void doStart();
    void doCancel();
    void passwordRead(QKeychain::Job *);
    void passwordWritten();
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_username;
    QString m_password;
    int m_errorCode = NoError;
    QString m_errorString;
    bool m_lastAuthenticationFailed = false;
    bool m_authenticationDoneAlready = false;
    bool m_passwordReadError = false;
};

#endif
