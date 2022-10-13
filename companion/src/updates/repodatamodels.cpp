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

#include "repodatamodels.h"

QString repoMetaDataTypeToString(RepoMetaDataType val)
{
  switch ((int)val) {
    case RMDT_Release:
      return "Release";
    case RMDT_Releases:
      return "Releases";
    case RMDT_Asset:
      return "Asset";
    case RMDT_ReleaseAssets:
      return "Release Assets";
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

/*
    RepoRawItemModel
*/

RepoRawItemModel(QString name) :
  QStandardItemModel(nullptr),
  json(nullptr),
  m_name(name)
{

}

void RepoRawItemModel::update()
{
  for (int i = 0; i < rowCount(); ++i)
    setDynamicItemData(item(i));
}

void RepoRawItemModel::dumpContents()
{
  qDebug() << "Contents of" << m_modelName;

  if (itemModel) {
    for (int i = 0; i < rowCount(); ++i) {
      qDebug().noquote()
               << "row:"         << i
               << "name:"        << data(index(i, 0), Qt::DisplayRole).toString()
               << "id:"          << data(index(i, 0), RIMR_Id).toInt()
               << "date:"        << data(index(i, 0), RIMR_Date).toDateTime()
               << "tag:"         << data(index(i, 0), RIMR_Tag).toString()
               << "prerelease:"  << data(index(i, 0), RIMR_Prerelease).toBool()
               << "avail:"       << data(index(i, 0), RIMR_Available).toBool()
               << "flags:"       << data(index(i, 0), RIMR_Flags).toInt();
    }
  }
}

/*
    RepoFilteredItemModel
*/

RepoFilteredItemModel::RepoFilteredItemModel(UpdatesItemModel * sourceModel, QString name) :
  QSortFilterProxyModel(nullptr),
  filterFlags(0),
  m_name(name)
{
  filterText.setPattern("");
  filterText.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  setFilterRole(RIMR_Available);
  setFilterKeyColumn(0);
  setDynamicSortFilter(true);
  setSourceModel(sourceModel);
}

void RepoFilteredItemModel::setFilterFlags(const int flags)
{
  if (filterFlags != flags) {
    filterFlags = flags;
    filterText.setPattern("");
    invalidateFilter();
  }
}

void RepoFilteredItemModel::setFilterPattern(const QString & pattern)
{
  if (filterText.pattern() != pattern) {
    filterText.setPattern(pattern);
    filterFlags = 0;
    invalidateFilter();
  }
}

bool RepoFilteredItemModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
{
  const QModelIndex & srcIdx = sourceModel()->index(sourceRow, 0, sourceParent);

  if (!srcIdx.isValid() || !sourceModel()->data(srcIdx, filterRole()).toBool())
    return false;

  if (!filterFlags && filterText.pattern().isEmpty())
    return true;

  if (filterFlags) {
    bool ok;
    const int flags = sourceModel()->data(srcIdx, RIMR_Flags).toInt(&ok);

    return (ok && (!flags || (filterFlags & flags)));
  }
  else {
    return sourceModel()->data(srcIdx, Qt::DisplayRole).toString().contains(filterText);
  }
}

QMap<int, QVariant> RepoFilteredItemModel::itemMetaData(const int id)
{
  const QModelIndexList idxlst = match(index(0, 0), RIMR_Id, QVariant(id), 1, Qt::MatchExactly);

  if (!idxlst.isEmpty())
    return itemData(idxlst.at(0));

  return QMap<int, QVariant>();
}

QVariant RepoFilteredItemModel::metaDataValue(const int id, const int role)
{
  //qDebug() << "id:" << id << "role:";

  const QModelIndexList idxlst = match(index(0, 0), RIMR_Id, QVariant(id), 1, Qt::MatchExactly);

  if (!idxlst.isEmpty())
    return data(idxlst.at(0), role);

  return QVariant();
}

bool RepoFilteredItemModel::setMetaDataValue(const int id, const int role, const QVariant value)
{
  //qDebug() << "id:" << id << "role:" << role << "value:" << value;

  const QModelIndexList idxlst = match(index(0, 0), RIMR_Id, QVariant(id), 1, Qt::MatchExactly);

  if (!idxlst.isEmpty())
    return setData(idxlst.at(0), value, role);

  return false;
}

int RepoFilteredItemModel::id(const int row)
{
  const QModelIndex & idx = index(row, 0);

  if (!idx.isValid())
    return 0;

  bool ok;
  const int id = data(idx, RIMR_Id).toInt(&ok);

  if (ok)
    return id;

  return 0;
}

int RepoFilteredItemModel::id(const QVariant value, const Qt::MatchFlags flags, const int role)
{
  const QModelIndexList idxlst = match(index(0, 0), role, value, 1, flags);

  if (!idxlst.isEmpty()) {
    bool ok;
    const int id = data(idxlst.at(0), RIMR_Id).toInt(&ok);

    if (ok)
      return id;
  }

  return 0;
}

QStringList RepoFilteredItemModel::list()
{
  QStringList strl;

  for (int i = 0; i < rowCount(); i++)
    strl << data(index(i, 0)).toString();

  return strl;
}

void RepoFilteredItemModel::dumpContents()
{
  qDebug() << "Contents of" << m_modelName;

  if (itemModel) {
    for (int i = 0; i < rowCount(); ++i) {
      qDebug().noquote()
               << "row:"         << i
               << "name:"        << data(index(i, 0), Qt::DisplayRole).toString()
               << "id:"          << data(index(i, 0), RIMR_Id).toInt()
               << "date:"        << data(index(i, 0), RIMR_Date).toDateTime()
               << "tag:"         << data(index(i, 0), RIMR_Tag).toString()
               << "prerelease:"  << data(index(i, 0), RIMR_Prerelease).toBool()
               << "avail:"       << data(index(i, 0), RIMR_Available).toBool()
               << "flags:"       << data(index(i, 0), RIMR_Flags).toInt();
    }
  }
}
