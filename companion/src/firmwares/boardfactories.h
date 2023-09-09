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

#include "boards.h"
#include "jsonboard.h"

class BoardFactory
{
  public:
    explicit BoardFactory(Board::Type board) :
      m_instance(new JsonBoard(board))
      {}

    virtual ~BoardFactory() {}

    JsonBoard* instance() const { return m_instance; }

  private:
    JsonBoard *m_instance;
};

class BoardFactories
{
  public:
    explicit BoardFactories();
    virtual ~BoardFactories();

    void registerBoardFactories();
    bool registerBoardFactory(BoardFactory * factory);
    void unregisterBoardFactories();

    JsonBoard* instance(Board::Type board) const;

  private:
    QList<BoardFactory *> registeredBoardFactories;
};
