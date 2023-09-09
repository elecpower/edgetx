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

#include <QtCore>

#include <string>
#include <vector>

class JsonBoard : public Boards
{
  Q_DECLARE_TR_FUNCTIONS(JsonBoard)

  public:
    enum InputType
    {
      INPUT_STICK,
      INPUT_FLEX,
      INPUT_SWITCH,
      INPUT_VBAT,
      INPUT_RTC_BAT,
    };

    enum FlexType
    {
      FLEX_POT,
      FLEX_POT_CENTER,
      FLEX_MULTIPOS,
      FLEX_SLIDER,
    };

    struct BoardDefinition {
      Board::Type board;
      std::string name;
      std::string jsonFile;

      BoardDefinition() = default;
    };

    typedef std::vector<BoardDefinition> BoardDefinitionsTable;

      struct AnalogDefn {
      std::string name;
      std::string type;
      std::string label;
      std::string shortLabel;
      std::string dflt;
    };

    typedef std::vector<AnalogDefn> AnalogsTable;

    struct Display {
      unsigned int x;
      unsigned int y;
    };

    struct SwitchDefn {
      std::string name;
      Board::SwitchType type;
      int flags;
      bool inverted;
      std::string dflt;
      Display display;
    };

    typedef std::vector<SwitchDefn> SwitchesTable;

    struct TrimDefn {
      std::string name;
    };

    typedef std::vector<TrimDefn> TrimsTable;

    explicit JsonBoard(Board::Type board);
    virtual ~JsonBoard();

    const int getCapability(const Board::Capability capability) const override;
    const Board::SwitchInfo getSwitchInfo(int index) const override;

    Board::Type board() const { return m_boardDef.board; }
    QString name() const { return m_boardDef.name.c_str(); }

    bool loadDefinition();


private:
    BoardDefinition m_boardDef;

    AnalogsTable *m_analogs;
    SwitchesTable *m_switches;
    TrimsTable *m_trims;

    unsigned int m_funcSwitchesCnt;
    unsigned int m_potsCnt;
    unsigned int m_slidersCnt;
    unsigned int m_sticksCnt;
    unsigned int m_switchesCnt;

    static BoardDefinition getBoardDefinition(const BoardDefinitionsTable & tbl, Board::Type board);

    static unsigned int getPotsCount(const AnalogsTable & analogs);
    static unsigned int getSlidersCount(const AnalogsTable & analogs);
    static unsigned int getSticksCount(const AnalogsTable & analogs);
    static unsigned int getSwitchesCount(const SwitchesTable & switches);
    static unsigned int getFuncSwitchesCount(const SwitchesTable & switches);
    static bool loadFile(BoardDefinition & boardDef, AnalogsTable * analogs, SwitchesTable * switches, TrimsTable * trims);
};
