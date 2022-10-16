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

#include "reporeleases.h"
#include "repoassets.h"
#include "reponetwork.h"

#include <QtCore>

constexpr char GH_API_REPOS[]                 {"https://api.github.com/repos"};
constexpr char GH_API_REPOS_EDGETX[]          {"https://api.github.com/repos/EdgeTX"};

constexpr char GH_ACCEPT_HEADER_BINARY[]      {"application/octet-stream"};
constexpr char GH_ACCEPT_HEADER_METADATA[]    {"application/vnd.github.v3+json"};
constexpr char GH_ACCEPT_HEADER_RAW[]         {"application/vnd.github.v3.raw"};

class RepoMetaData : public QObject
{
    Q_OBJECT

  public:
    explicit RepoMetaData(QObject * parent);
    virtual ~RepoMetaData();

    ReleasesMetaData *releases;
    AssetsMetaData *assets;
    RepoNetwork *network;   //  private??????

    const QString urlContent(const QString & path) { return QString("%1/contents/%2").arg(m_repo).arg(path); }

  protected:
    QString m_repo;
    int m_currentReleaseId;
    int m_currentAssetId;
};
