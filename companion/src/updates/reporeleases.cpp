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

#include "reporeleases.h"
#include "appdata.h"

/*
    ReleasesRawItemModel
*/

ReleasesRawItemModel::ReleasesRawItemModel() :
  RepoRawItemModel("Releases"),
  m_settingsIdx(-1),
  m_nightlyName(""),
  m_releaseChannel(-1)
{
  setSortRole(RIMR_Date);
}

void ReleasesRawItemModel::setReleaseChannel(const int channel)
{
  if (m_releaseChannel != channel) {
    m_releaseChannel = channel;
    update();
  }
}

void ReleasesRawItemModel::parseMetaData(const int mdt, QJsonDocument * jsonDoc)
{
  json = jsonDoc;

  switch (mdt) {
    case RMDT_Release:
      parseRelease();
      break;
    case RMDT_Releases:
      parseReleases();
      break;
    default;
      qDebug() << "Unexpected meta data type" << repoMetaDataTypeToString(mdt);
      break;
  }
}

void ReleasesRawItemModel::parseRelease()
{
  clear();

  if (json->isObject()) {
    const QJsonObject &obj = json->object();
    parseJsonObject(obj);
  }
}

void ReleasesRawItemModel::parseReleases()
{
  clear();

  if (json->isArray()) {
    const QJsonArray &array = json->array();
    foreach (const QJsonValue &v, array)
    {
      if (v.isObject()) {
        const QJsonObject &obj = v.toObject();
        parseJsonObject(obj);
      }
    }
  }

  sort(0, Qt::DescendingOrder);
  m_refreshRequired = false;
}

void ReleasesRawItemModel::parseJsonObject(const QJsonObject & obj)
{
  QStandardItem * item = new QStandardItem();

  QString tag = QString();
  bool prerelease = false;

  if (!obj.value("name").isUndefined()) {
    item->setText(obj.value("name").toString());
  }

  if (!obj.value("id").isUndefined()) {
    item->setData(obj.value("id").toInt(), RIMR_Id);
  }

  if (!obj.value("tag_name").isUndefined()) {
    tag = obj.value("tag_name").toString();
    item->setData(tag, RIMR_Tag);
  }

  if (!obj.value("prerelease").isUndefined()) {
    prerelease = obj.value("prerelease").toBool();
    item->setData(prerelease, RIMR_Prerelease);
  }

  if (!obj.value("created_at").isUndefined()) {
    item->setData(QDateTime::fromString(obj.value("created_at").toString(), Qt::ISODate), RIMR_Date);
  }

  setDynamicItemData(item);

  appendRow(item);
}

void ReleasesRawItemModel::setDynamicItemData(QStandardItem * item)
{
  item->setData(isAvailable(item), RIMR_Available);
}

bool ReleasesRawItemModel::isAvailable(QStandardItem * item)
{
  if (item->data(IMDR_Prerelease).toBool()) {
    if (m_releaseChannel == ComponentData::RELEASE_CHANNEL_STABLE) {
      //qDebug() << "Release channel stable" << tagname << "is pre-release" ;
      return false;
    }
    else if (m_releaseChannel == ComponentData::RELEASE_CHANNEL_PRERELEASE &&
             item->data(IMDR_Tag).toString().toLower() == m_nightlyName) {
      //qDebug() << "Release channel pre-release" << tagname << "is nightly" ;
      return false;
    }
  }
  //qDebug() << "Release available:" << tagname;
  return true;
}

/*
    ReleasesFilteredItemModel
*/

ReleasesFilteredItemModel::ReleasesFilteredItemModel() :
  RepoFilteredItemModel("Filtered Releases")
{
  setSortRole(RIMR_Date);
}

int ReleasesFilteredItemModel::channelLatestId()
{
  return rows() > 0 ? id(0) : 0;
}

/*
    ReleasesMetaData
*/

ReleasesMetaData::ReleasesMetaData(QObject * parent) :
  RepoMetaData(parent),
  m_id(0)
{
  setModels(new ReleasesRawItemModel(), new ReleasesFilteredItemModel());
}

void ReleasesMetaData::init(const QString repo, const QString nightly, const int settingsIndex, const int resultsPerPage)
{
  m_repo = repo;
  m_resultsPerPage = resultsPerPage;
  m_itemModel->setNightlyName(nightly);
  m_itemModel->setSettingsIndex(settingsIndex);
}

int ReleasesMetaData::getSetId()
{
  ReleasesFilteredItemModel *mdl = qobject_cast<ReleasesFilteredItemModel *>(m_itemFilteredModel);
  if (mdl)
    m_id = mdl->channelLatestId();

  return m_id;
}

void ReleasesMetaData::parseMetaData(int mdt, QJsonDocument * jsonDoc)
{
  ReleasesRawItemModel *mdl = qobject_cast<ReleasesRawItemModel *>(m_itemModel);
  if (mdl)
    mdl->parseMetaData(mdt, jsonDoc);
}
