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

#include "repoassets.h"

//  static
//  itemModel can either be the raw or filtered model
void AssetsItemModel::dumpContents(QAbstractItemModel * itemModel, QString & name)
{
  qDebug() << "Contents of" << name;

  if (itemModel) {
    for (int i = 0; i < rowCount(); ++i) {
      qDebug().noquote()
               << "row:"         << i
               << "name:"        << itemModel->data(itemModel->index(i, 0), Qt::DisplayRole).toString()
               << "id:"          << itemModel->data(itemModel->index(i, 0), RIMR_Id).toInt()
               << "date:"        << itemModel->data(itemModel->index(i, 0), RIMR_Date).toDateTime()
               << "content:"     << itemModel->data(itemModel->index(i, 0), RIMR_Content).toString()
               << "avail:"       << itemModel->data(itemModel->index(i, 0), RIMR_Available).toBool()
               << "flags:"       << itemModel->data(itemModel->index(i, 0), RIMR_Flags).toInt()
               << "cpfltr:"      << itemModel->data(itemModel->index(i, 0), RIMR_CopyFilter).toString()
               << "subdir:"      << itemModel->data(itemModel->index(i, 0), RIMR_SubDirectory).toString();
    }
  }
}

/*
    AssetsItemModel
*/

AssetsRawItemModel::AssetsRawItemModel() :
  RepoRawItemModel("Assets")
{

}

void AssetsRawItemModel::parseMetaData(const int mdt, QJsonDocument * jsonDoc)
{
  json = jsonDoc;

  switch (mdt) {
    case MDT_Asset:
      parseAsset();
      break;
    case MDT_ReleaseAssets:
      parseReleaseAssets();
      break;
    default;
      qDebug() << "Unexpected meta data type" << repoMetaDataTypeToString(mdt);
      break;
  }
}

void AssetsRawItemModel::parseAsset()
{
  clear();

  if (json->isObject()) {
    const QJsonObject &obj = json->object();
    parseJsonObject(obj);
  }
}

void AssetsRawItemModel::parseReleaseAssets()
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

  sort(0);
}

void AssetsRawItemModel::parseJsonObject(const QJsonObject & obj)
{
  QStandardItem * item = new QStandardItem();

  if (!obj.value("name").isUndefined()) {
    item->setText(obj.value("name").toString());
  }

  if (!obj.value("id").isUndefined()) {
    item->setData(obj.value("id").toInt(), UpdatesItemModel::IMDR_Id);
  }

  if (!obj.value("content_type").isUndefined()) {
    item->setData(obj.value("content_type").toString(), UpdatesItemModel::IMDR_Content);
  }

  if (!obj.value("updated_at").isUndefined()) {
    item->setData(QDateTime::fromString(obj.value("updated_at").toString(), Qt::ISODate), UpdatesItemModel::IMDR_Date);
  }

  item->setData(true, UpdatesItemModel::IMDR_Available);

  appendRow(item);
}

void AssetsRawItemModel::dumpContents()
{
  AssetsItemModel::dumpContents(qobject_cast<QAbstractItemModel *>(this), modelName());
}

/*
    AssetsFilteredItemModel
*/

AssetsFilteredItemModel::AssetsFilteredItemModel(UpdatesItemModel * sourceModel) :
  RepoFilteredItemModel(sourceModel, "Filtered Assets")
{

}

bool AssetsFilteredItemModel::setFlags(const int id, const int flags)
{
  return setMetaDataValue(id, UpdatesItemModel::IMDR_Flags, QVariant(flags));
}

bool AssetsFilteredItemModel::setSubDirectory(const int id, const QString path)
{
  return setMetaDataValue(id, UpdatesItemModel::IMDR_SubDirectory, QVariant(path));
}

bool AssetsFilteredItemModel::setCopyFilter(const int id, const QString filter)
{
  return setMetaDataValue(id, UpdatesItemModel::IMDR_CopyFilter, QVariant(filter));
}

void AssetsFilteredItemModel::dumpContents()
{
  AssetsItemModel::dumpContents(qobject_cast<QAbstractItemModel *>(this), modelName());
}

/*
    AssetsMetaData
*/

AssetsMetaData::AssetsMetaData(QObject * parent) :
  RepoMetaData(parent)
{
  itemModel = new AssetsItemModel();
  filteredItemModel = new AssetsFilteredItemModel(itemModel);
}

void AssetsMetaData::init(const QString repo, const int resultsPerPage)
{
  m_repo = repo;
  m_resultsPerPage = resultsPerPage;
}

int AssetsMetaData::getSetId(int row)
{
  m_id = filteredItemModel->id(row);
  return m_id;
}

int AssetsMetaData::getSetId(QVariant value, Qt::MatchFlags flags, int role)
{
  m_id = filteredItemModel->id(value, flags, role);
  return m_id;
}
