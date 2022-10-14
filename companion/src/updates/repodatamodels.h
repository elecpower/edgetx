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

#include <QtCore>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QJsonDocument>

enum RepoMetaDataType {
  RMDT_Release,
  RMDT_Releases,
  RMDT_Asset,
  RMDT_ReleaseAssets
};

QString repoMetaDataTypeToString(RepoMetaDataType val);

enum RepoItemModelRole {
  RIMR_Id = Qt::UserRole,
  RIMR_Date,
  RIMR_Tag,
  RIMR_Prerelease,
  RIMR_Content,
  RIMR_Available,
  RIMR_Flags,
  RIMR_CopyFilter,
  RIMR_SubDirectory
};

class RepoRawItemModel : public QStandardItemModel
{
  Q_OBJECT

  public:
    explicit RepoRawItemModel(QString name = "");
    virtual ~RepoRawItemModel() {}

    virtual void parseMetaData(const int mdt, QJsonDocument * json) = 0;

    QString modelName() { return m_modelName; }

    void dumpContents();

  protected:
    virtual bool isAvailable(QStandardItem * item) = 0;
    virtual void setDynamicItemData(QStandardItem * item) = 0;

    void update();

  private:
    QString m_modelName;
    bool m_refreshRequired;
};

class RepoFilteredItemModel: public QSortFilterProxyModel
{
    Q_OBJECT

  public:
    explicit RepoFilteredItemModel(QString name = "");
    virtual ~RepoFilteredItemModel() {}

    QStringList list();

    int id(const int row);
    int id(const QVariant value, const Qt::MatchFlags flags = Qt::MatchExactly, const int role = Qt::DisplayRole);
    int rows() { return rowCount(); }

    QString date(const int id) { return metaDataValue(id, RIMR_Date).toString(); }
    QString name(const int id) { return metaDataValue(id, Qt::DisplayRole).toString(); }

    void dumpContents();

  protected slots:
    void setFilterFlags(const int flags);
    void setFilterPattern(const QString & pattern);

  protected:
    int filterFlags;
    QRegularExpression filterText;

    QString modelName() { return m_modelName; }

    bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const override;
    QMap<int, QVariant> itemMetaData(int id);
    QVariant metaDataValue(const int id, const int role);
    bool setMetaDataValue(const int id, const int role, const QVariant value);

  private:
    QString m_modelName;
};
