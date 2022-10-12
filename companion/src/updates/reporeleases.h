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

class ReleasesItemModel : public QObject
{
    Q_OBJECT

  public:
    explicit ReleasesItemModel() = default;
    virtual ~ReleasesItemModel() {}

    static void dumpContents(QAbstractItemModel * itemModel, QString & name);
};

class ReleasesRawItemModel : public RepoRawItemModel
{
  Q_OBJECT

  friend class ReleasesMetaData;

  public:
    explicit ReleasesRawItemModel();
    virtual ~ReleasesRawItemModel() {}

    virtual void parseMetaData(const int mdt, QJsonDocument * json);

    void setNightlyName(const QString name) { m_nightlyName = name.toLower(); }
    void setSettingsIndex(const int index) { m_settingsIdx = index; }
    void setReleaseChannel(const int channel);

    int settingsIndex() { return m_settingsIdx; }
    bool refreshRequired() { return m_refreshRequired; }

    void dumpContents();

  private:
    int m_settingsIdx;
    QString m_nightlyName;
    int m_releaseChannel;
    bool m_refreshRequired;

    virtual void setDynamicItemData(QStandardItem * item) override;

    bool isReleaseAvailable(QStandardItem * item);

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

    void dumpContents() { ReleasesItemModel::dumpContents(this, modelName()); }

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

    void init(const QString repo, const QString nightly, const int settingsIndex, const int resultsPerPage);

    bool refreshRequired();
    void setReleaseChannel(const int channel) { itemModel->setReleaseChannel(channel); }

    void setId(int id) { m_id = id; }
    int id() { return m_id; }

    int getSetId();
    int getSetId(int row);
    int getSetId(QVariant value, Qt::MatchFlags flags = Qt::MatchExactly, int role = Qt::DisplayRole);

    void parseMetaData(int mdt, QJsonDocument * jsonDoc) { itemModel->parseMetaData(mdt, jsonDoc); }

    int count() { return filteredItemModel->rows(); }
    QStringList list() { return filteredItemModel->list(); }
    void dumpModelRaw() { itemModel->dumpContents(); }
    void dumpModelFiltered() { filteredItemModel->dumpContents(); }

    QString date() { return filteredItemModel->date(m_id); }
    QString name();
    bool prerelease() { return filteredItemModel->prerelease(m_id); }
    QString version();

    const QString urlRelease() { return QString("%1/%2").arg(urlReleases()).arg(m_id); }

  private:
    ReleasesItemModel *itemModel;
    ReleasesFilteredItemModel *filteredItemModel;
    int m_id;
};
