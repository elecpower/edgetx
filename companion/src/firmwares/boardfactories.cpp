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

#include "boardfactories.h"

BoardFactories::BoardFactories()
{
  registerBoardFactories();
}

BoardFactories::~BoardFactories()
{
  unregisterBoardFactories();
}

bool BoardFactories::registerBoardFactory(BoardFactory * factory)
{
  for (auto *registeredFactory : registeredBoardFactories) {
    if (registeredFactory->instance()->board() == factory->instance()->board()) {
      qDebug() << "Error - Duplicate board factory:" << factory->instance()->name();
      return false;
    }
  }

  registeredBoardFactories.append(factory);
  return true;
}

void BoardFactories::registerBoardFactories()
{
  QStringList regList;

  for (int i = 0; i < Board::Type::BOARD_TYPE_COUNT; i++) {
    BoardFactory *bf = new BoardFactory((Board::Type)i);
    if (bf->instance()->loadDefinition()) {
      if (registerBoardFactory(bf))
        regList.append(bf->instance()->name());
      else
        delete bf;
    }
    else
      delete bf;
  }

  qDebug() << "Registered board factories:" << regList;
}

void BoardFactories::unregisterBoardFactories()
{
  for (auto *registeredFactory : registeredBoardFactories)
    delete registeredFactory;
}

JsonBoard* BoardFactories::instance(Board::Type board) const
{
  for (auto *registeredFactory : registeredBoardFactories) {
    if (registeredFactory->instance()->board() == board)
      return registeredFactory->instance();
  }
  qDebug() << "Error - Board factory not found:" << Boards::getBoardName(board);
  return nullptr;
}
