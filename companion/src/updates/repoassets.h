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

class AssetsRawItemModel : public RepoRawItemModel
{
  Q_OBJECT

  public:

  protected:
    friend class AssetsMetaData;

    explicit AssetsRawItemModel();
    virtual ~AssetsRawItemModel() {}

    virtual void parseMetaData(const int mdt, QJsonDocument * json);

  private:
    bool m_refreshRequired;

    virtual bool isAvailable(QStandardItem * item) {}
    virtual void setDynamicItemData(QStandardItem * item) {}

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

    explicit AssetsFilteredItemModel() : RepoFilteredItemModel("Filtered Assets") {}
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

    virtual void parseMetaData(int mdt, QJsonDocument * jsonDoc);

    void init(const QString repo, const int resultsPerPage);

    void setReleaseId(int id);

    bool setFlags(int flags) { return m_filteredItemModel->setFlags(m_id, flags); }

    void setFilterFlags(int flags) { m_filteredItemModel->setFilterFlags(flags); }
    void setFilterPattern(const QString & pattern) { m_filteredItemModel->setFilterPattern(pattern); }

    bool setSubDirectory(const QString path) { return m_filteredItemModel->setSubDirectory(m_id, path); }
    bool setCopyFilter(const QString filter) { return m_filteredItemModel->setCopyFilter(m_id, filter); }

    QString filename() { return m_filteredItemModel->metaDataValue(m_id, Qt::DisplayRole).toString(); }
    QString contentType() { return m_filteredItemModel->metaDataValue(m_id, RIMR_Content).toString(); }
    QString subDirectory() { return m_filteredItemModel->metaDataValue(m_id, RIMR_SubDirectory).toString(); }
    QString copyFilter() { return m_filteredItemModel->metaDataValue(m_id, RIMR_CopyFilter).toString(); }
    int flags() { return m_filteredItemModel->metaDataValue(m_id, RIMR_Flags).toInt(); }

    const QString urlReleaseAssets(int releaseId) {
                                   return QString("%1/%2/assets").arg(urlReleases()).arg(releaseId) % (m_resultsPerPage > -1 ?
                                   QString("\?per_page=%1").arg(m_resultsPerPage) : ""); }
    const QString urlAsset() { return QString("%1/assets/%2").arg(urlReleases()).arg(m_id); }

  private:
    int m_releaseId;
};
