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

class RepoMetaData : public QObject
{
    Q_OBJECT

  public:
    explicit RepoMetaData(QObject * parent);
    virtual ~RepoMetaData() {}

  signals:
    void idChanged(int id);

  protected:
    void setModels(RepoRawItemModel * itemModel, RepoFilteredItemModel * filteredItemModel);

    virtual int getSetId() = 0;
    void setId(int id) { m_id = id; }
    int id() { return m_id; }

    int getSetId(int row);
    int getSetId(QVariant value, Qt::MatchFlags flags = Qt::MatchExactly, int role = Qt::DisplayRole);

    int count() { return m_filteredItemModel->rows(); }
    QStringList list() { return m_filteredItemModel->list(); }

    QString date() { return m_id ? m_filteredItemModel->date(m_id); }
    QString name() { return m_id ? m_filteredItemModel->name(m_id) : ""; }
    QString version() { return m_id ? m_filteredItemModel->version(m_id) : "0.0"; }

    bool refreshRequired() {return m_id == 0 ? true : m_itemModel->refreshRequired(); }

    void dumpModelRaw() { m_itemModel->dumpContents(); }
    void dumpModelFiltered() { m_filteredItemModel->dumpContents(); }

  private:
    RepoRawItemModel *m_itemModel;
    RepoFilteredItemModel *m_filteredItemModel;
    QString m_repo;
    int m_resultsPerPage;
    int m_id;
};
