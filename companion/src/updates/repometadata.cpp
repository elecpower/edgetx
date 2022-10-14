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

#include "repometadata.h"

RepoMetaData::RepoMetaData(QObject * parent) :
  QObject(parent),
  m_id(0)
{

}

void RepoMetaData::setModels(RepoRawItemModel * itemModel, RepoFilteredItemModel * filteredItemModel)
{
  m_itemModel = itemModel;
  m_filteredItemModel = filteredItemModel;
  m_filteredItemModel->setModel(m_itemModel);
}

int RepoMetaData::getSetId(int row)
{
  m_id = m_filteredItemModel->id(row);
  return m_id;
}

int RepoMetaData::getSetId(QVariant value, Qt::MatchFlags flags, int role)
{
  m_id = m_filteredItemModel->id(value, flags, role);
  return m_id;
}
