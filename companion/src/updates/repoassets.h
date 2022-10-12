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

#include "repodatamodels.h"

class AssetsItemModel : public QObject
{
    Q_OBJECT

  public:
    explicit AssetsItemModel() = default;
    virtual ~AssetsItemModel() {}

    static void dumpContents(QAbstractItemModel * itemModel, QString & name);
};

class AssetsRawItemModel : public RepoRawItemModel
{
  Q_OBJECT

  friend class AssetsMetaData;

  public:
    explicit AssetsItemModel();
    virtual ~AssetsItemModel() {}

    virtual void parseMetaData(const int mdt, QJsonDocument * json);

  private:
    void parseAsset();
    void parseReleaseAssets();
    void parseJsonObject(const QJsonObject & obj);
};

class AssetsFilteredItemModel : public RepoFilteredItemModel
{
    Q_OBJECT

  public:

  protected:
    friend class AssetsMetaData;

    explicit AssetsFilteredItemModel(UpdatesItemModel * sourceModel);
    virtual ~AssetsFilteredItemModel() {};

    bool setFlags(const int id, const int flags);
    bool setSubDirectory(const int id, const QString path);
    bool setCopyFilter(const int id, const QString filter);

    QString filename(int id) { return metaDataValue(id, Qt::DisplayRole).toString(); }

  private:
};

class AssetsMetaData : public RepoMetaData
{
    Q_OBJECT
  public:
    explicit AssetsMetaData(QObject * parent);
    virtual ~AssetsMetaData() {};

    void init(const QString repo, const int resultsPerPage);

    void setId(int id) { m_id = id; }
    int id() { return m_id; }

    int getSetId(int row);
    int getSetId(QVariant value, Qt::MatchFlags flags = Qt::MatchExactly, int role = Qt::DisplayRole);

    void parseMetaData(int mdt, QJsonDocument * jsonDoc) { itemModel->parseMetaData(mdt, jsonDoc); }

    bool setFlags(int flags) { return filteredItemModel->setFlags(m_id, flags); }

    void setFilterFlags(int flags) { filteredItemModel->setFilterFlags(flags); }
    void setFilterPattern(const QString & pattern) { filteredItemModel->setFilterPattern(pattern); }

    bool setSubDirectory(const QString path) { return filteredItemModel->setSubDirectory(m_id, path); }
    bool setCopyFilter(const QString filter) { return filteredItemModel->setCopyFilter(m_id, filter); }

    int count() { return filteredItemModel->rows(); }
    QStringList list() { return filteredItemModel->list(); }
    void dumpModelRaw() { itemModel->dumpContents(); }
    void dumpModelFiltered() { filteredItemModel->dumpContents(); }

    QString date() { return filteredItemModel->date(m_id); }
    QString name() { return filteredItemModel->name(m_id); }
    QString filename() { return filteredItemModel->metaDataValue(m_id, Qt::DisplayRole).toString(); }
    QString contentType() { return filteredItemModel->metaDataValue(m_id, UpdatesItemModel::IMDR_Content).toString(); }
    QString subDirectory() { return filteredItemModel->metaDataValue(m_id, UpdatesItemModel::IMDR_SubDirectory).toString(); }
    QString copyFilter() { return filteredItemModel->metaDataValue(m_id, UpdatesItemModel::IMDR_CopyFilter).toString(); }
    int flags() { return filteredItemModel->metaDataValue(m_id, UpdatesItemModel::IMDR_Flags).toInt(); }

    const QString urlReleaseAssets(int releaseId) {
                                   return QString("%1/%2/assets").arg(urlReleases()).arg(releaseId) % (m_resultsPerPage > -1 ?
                                   QString("\?per_page=%1").arg(m_resultsPerPage) : ""); }
    const QString urlAsset() { return QString("%1/assets/%2").arg(urlReleases()).arg(m_id); }

  private:
    AssetsItemModel *itemModel;
    AssetsFilteredItemModel *filteredItemModel;
    int m_id;
};
