/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#pragma once

#include "progresswidget.h"

#include <QtCore>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class RepoNetwork : public QObject
{
    Q_OBJECT

  public:

    enum DownloadDataType {
      DDT_Binary,
      DDT_Content,
      DDT_SaveToFile,
      DDT_MetaData,
    };
    Q_ENUM(DownloadDataType)

    explicit RepoNetwork(QObject * parent);
    virtual ~RepoNetwork();

    void download(const DownloadDataType type, const int subtype, const QString & urlStr, const char * header, const QString & filePath);

  private slots:
    void onDownloadFinished(QNetworkReply * reply, DownloadDataType ddt, int subtype);

  private:
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QNetworkReply *reply;
    QByteArray *buffer;
    QFile *file;
    QUrl url;
    ProgressWidget *progress;
};
