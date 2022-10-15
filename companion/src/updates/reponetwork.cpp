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

#include "reponetwork.h"
#include "constants.h"

#include <QMessageBox>
#include <QtNetwork/QNetworkProxyFactory>

RepoNetwork::RepoNetwork(QWidget * parent) :
  QWidget(parent),
  reply(nullptr),
  buffer(new QByteArray()),
  file(nullptr)
{
  QNetworkProxyFactory::setUseSystemConfiguration(true);
}

RepoNetwork::~RepoNetwork()
{
  delete buffer;
  if (reply)
    delete reply;
  if (file)
    delete file;
}

bool RepoNetwork::download(const DownloadDataType type, const int subtype, const QString & urlStr,
                               const char * header, const QString & filePath)
{
  if (progress) {
    progress->setValue(0);
    progress->setMaximum(100);
  }

  buffer->clear();
  downloadSuccess = false;

  if (type == DDT_SaveToFile) {
    file = new QFile(filePath);
    if (!file->open(QIODevice::WriteOnly)) {
      QMessageBox::critical(nullptr, CPN_STR_APP_NAME, tr("Unable to open the download file %1 for writing.\nError: %2").arg(filePath).arg(file->errorString()));
      return false;
    }
  }
  else
    file = nullptr;

  url.setUrl(urlStr);

  if (!url.isValid()) {
    QMessageBox::critical(nullptr, CPN_STR_APP_NAME, tr("Invalid URL: %1").arg(urlStr));
    return false;
  }
  else
    QMessageBox::critical(nullptr, CPN_STR_APP_NAME, tr("URL: %1").arg(urlStr));

  request.setUrl(url);
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork);
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

  request.setRawHeader(QByteArray("Accept"), QByteArray(header));

  reply = manager.get(request);

  connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [=] (QNetworkReply::NetworkError code) {
    //  leave it to the finished slot to deal with error condition
    QMessageBox::critical(nullptr, CPN_STR_APP_NAME, tr("Network error has occurred. Error code: %1").arg(code));
  });

  connect(reply, &QNetworkReply::readyRead, [=]() {
    if (type == DDT_SaveToFile) {
      file->write(reply->readAll());
    }
    else {
      const QByteArray qba = reply->readAll();
      buffer->append(qba);
    }
  });

  connect(reply, &QNetworkReply::finished, [&]() {
    onDownloadFinished(reply, type, subtype);
  });

  connect(reply, &QNetworkReply::downloadProgress, [=](const qint64 bytesRead, const qint64 totalBytes) {
    if (progress) {
      progress->setMaximum(totalBytes);
      progress->setValue(bytesRead);
    }
  });

  while (!reply->isFinished()) {
    qApp->processEvents();
  }

  return true;
}

void RepoNetwork::onDownloadFinished(QNetworkReply * reply, DownloadDataType type, int subtype)
{
  if (type == DDT_SaveToFile) {
    file->flush();
    file->close();
  }

  if (progress)
    progress->setValue(progress->maximum());

  if (reply->error()) {
    QMessageBox::critical(nullptr, CPN_STR_APP_NAME, tr("Unable to download %1.\nError:%2\n%3").arg(downloadDataTypeToString(type)).arg(reply->error()).arg(reply->errorString()));

    if (type == DDT_SaveToFile) {
      file->remove();
      delete file;
      file = nullptr;
    }

    return;
  }

  if (type == DDT_MetaData) {
    parseMetaData(subtype);
  }
  else if (type == DDT_SaveToFile) {
    delete file;
    file = nullptr;
    downloadSuccess = true;
  }
  else {
    downloadSuccess = true;
  }
}

void RepoNetwork::parseMetaData(int mdt)
{
  //reportProgress(tr("Parse and load data models with metadata"), QtDebugMsg);

  QJsonDocument *json = new QJsonDocument();

  if (!convertDownloadToJson(json)) {
    return;
  }

  switch (mdt) {
    case MDT_Release:
    case MDT_Releases:
      releases->parseMetaData(mdt, json);
      break;
    case MDT_Asset:
    case MDT_ReleaseAssets:
      assets->parseMetaData(mdt, json);
      break;
  }

  delete json;
  downloadSuccess = true;
}

bool RepoNetwork::convertDownloadToJson(QJsonDocument * json)
{
  QJsonParseError res;

  *json = QJsonDocument::fromJson(*buffer, &res);

  if (res.error || json->isNull()) {
    reportProgress(tr("Unable to convert downloaded data to json format.\nError:%1\n%2").arg(res.error).arg(res.errorString()), QtCriticalMsg);
    return false;
  }

  return true;
}
