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
#include "repometadata.h"

class ReleasesRawItemModel : public RepoRawItemModel
{
  Q_OBJECT

  public:

  protected:
    friend class ReleasesMetaData;

    explicit ReleasesRawItemModel();
    virtual ~ReleasesRawItemModel() {}

    virtual void parseMetaData(const int mdt, QJsonDocument * json);

    void setNightlyName(const QString name) { m_nightlyName = name.toLower(); }
    void setSettingsIndex(const int index) { m_settingsIdx = index; }
    void setReleaseChannel(const int channel);

    int settingsIndex() { return m_settingsIdx; }

  private:
    int m_settingsIdx;
    QString m_nightlyName;
    int m_releaseChannel;

    virtual bool isAvailable(QStandardItem * item);
    virtual void setDynamicItemData(QStandardItem * item);

    void parseRelease();
    void parseReleases();
    void parseJsonObject(const QJsonObject & obj);
};

class ReleasesFilteredItemModel: public RepoFilteredItemModel
{
    Q_OBJECT

  public:

  protected:
    friend class ReleasesMetaData;

    explicit ReleasesFilteredItemModel(UpdatesItemModel * sourceModel);
    virtual ~ReleasesFilteredItemModel() {}

    int channelLatestId();
    bool prerelease(const int id) { return metaDataValue(id, RIMR_Prerelease).toBool(); }
    QString version(const int id) { return metaDataValue(id, RIMR_Tag).toString(); }

  private:
};

class ReleasesMetaData : public RepoMetaData
{
    Q_OBJECT

  public:
    explicit ReleasesMetaData(QObject * parent);
    virtual ~ReleasesMetaData() {}

    virtual void parseMetaData(int mdt, QJsonDocument * jsonDoc);
    virtual int getSetId();

    void init(const QString repo, const QString nightly, const int settingsIndex, const int resultsPerPage);

    void setReleaseChannel(const int channel) { m_itemModel->setReleaseChannel(channel); }

    bool prerelease() { return m_filteredItemModel->prerelease(m_id); }

    const QString urlRelease() { return QString("%1/%2").arg(urlReleases()).arg(m_id); }

  private:

};
