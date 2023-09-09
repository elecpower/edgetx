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

#include "jsonboard.h"
#include "datahelpers.h"
#include "constants.h"

// static
static const JsonBoard::BoardDefinitionsTable boardDefinitionsTable = {
    {Board::BOARD_UNKNOWN,              CPN_STR_UNKNOWN_ITEM,        ""},       // this must be the first entry
    {Board::BOARD_TARANIS_X7,           "Taranis X7/X7S",            "x7"},
    {Board::BOARD_TARANIS_X7_ACCESS,    "Taranis X7/X7S Access",     "x7access"},
    {Board::BOARD_TARANIS_X9D,          "Taranis X9D",               "x9d"},
    {Board::BOARD_TARANIS_X9DP,         "Taranis X9D+",              "x9d+"},
    {Board::BOARD_TARANIS_X9DP_2019,    "Taranis X9D+ 2019",         "x9d+2019"},
    {Board::BOARD_TARANIS_X9E,          "Taranis X9E",               "x9e"},
    {Board::BOARD_TARANIS_X9LITE,       "Taranis X9-Lite",           "x9lite"},
    {Board::BOARD_TARANIS_X9LITES,      "Taranis X9-Lite S",         "x9lites"},
    {Board::BOARD_X10,                  "Horus X10/X10S",            "x10"},
    {Board::BOARD_X10_EXPRESS,          "Horus X10/X10S Express",    "x10express"},
    {Board::BOARD_HORUS_X12S,           "Horus X12S",                "x12s"},
    {Board::BOARD_TARANIS_XLITE,        "Taranis X-Lite",            "xlite"},
    {Board::BOARD_TARANIS_XLITES,       "Taranis X-Lite S/PRO",      "xlites"},
    {Board::BOARD_JUMPER_T12,           "Jumper T12",                "t12"},
    {Board::BOARD_JUMPER_T16,           "Jumper T16",                "t16"},
    {Board::BOARD_JUMPER_T18,           "Jumper T18",                "t18"},
    {Board::BOARD_JUMPER_TLITE,         "Jumper T-Lite",             "tlite"},
    {Board::BOARD_JUMPER_TLITE_F4,      "Jumper T-Lite (F4 MCU)",    "tlitef4"},
    {Board::BOARD_JUMPER_TPRO,          "Jumper T-Pro",              "tpro"},
    {Board::BOARD_RADIOMASTER_BOXER,    "Radiomaster Boxer",         "boxer"},
    {Board::BOARD_RADIOMASTER_T8,       "Radiomaster T8",            "t8"},
    {Board::BOARD_RADIOMASTER_TX12,     "Radiomaster TX12",          "tx12"},
    {Board::BOARD_RADIOMASTER_TX12_MK2, "Radiomaster TX12 Mark II",  "tx12mk2"},
    {Board::BOARD_RADIOMASTER_TX16S,    "Radiomaster TX16S",         "tx16s"},
    {Board::BOARD_RADIOMASTER_ZORRO,    "Radiomaster Zorro",         "zorro"},
    {Board::BOARD_BETAFPV_LR3PRO,       "BETAFPV LR3PRO",            "lr3pro"},
    {Board::BOARD_IFLIGHT_COMMANDO8,    "iFlight Commando 8",        "commando8"},
    {Board::BOARD_FLYSKY_NV14,          "FlySky NV14",               "nv14"},
    //{Board::BOARD_FLYSKY_EL18,          "FlySky EL18",               "el18"},
    //{Board::BOARD_FLYSKY_PL18,          "FlySky PL18",               "pl18"},
};

// static
static const StringTagMappingTable inputTypesLookupTable = {
    {std::to_string(JsonBoard::INPUT_STICK),   "STICK"},
    {std::to_string(JsonBoard::INPUT_FLEX),    "FLEX"},
    {std::to_string(JsonBoard::INPUT_SWITCH),  "SWITCH"},
    {std::to_string(JsonBoard::INPUT_VBAT),    "VBAT"},
    {std::to_string(JsonBoard::INPUT_RTC_BAT), "RTC_BAT"},
};

// static
static const StringTagMappingTable flexDefaultsLookupTable = {
    {std::to_string(JsonBoard::FLEX_POT),        "POT"},
    {std::to_string(JsonBoard::FLEX_POT_CENTER), "POT_CENTER"},
    {std::to_string(JsonBoard::FLEX_MULTIPOS),   "MULTIPOS"},
    {std::to_string(JsonBoard::FLEX_SLIDER),     "SLIDER"},
};

// static
static const StringTagMappingTable switchTypesLookupTable = {
    {std::to_string(Board::SWITCH_NOT_AVAILABLE), "NONE"},
    {std::to_string(Board::SWITCH_TOGGLE),        "TOGGLE"},
    {std::to_string(Board::SWITCH_2POS),          "2POS"},
    {std::to_string(Board::SWITCH_3POS),          "3POS"},
    {std::to_string(Board::SWITCH_FSWITCH),       "FSWITCH"},
};


JsonBoard::JsonBoard(Board::Type board) :
  Boards(board),
  m_analogs(new AnalogsTable),
  m_switches(new SwitchesTable),
  m_trims(new TrimsTable),
  m_funcSwitchesCnt(0),
  m_potsCnt(0),
  m_slidersCnt(0),
  m_sticksCnt(0),
  m_switchesCnt(0)
{
  m_boardDef = getBoardDefinition(boardDefinitionsTable, board);
  //qDebug() << m_boardDef.name.c_str();
}

JsonBoard::~JsonBoard()
{
  delete m_analogs;
  delete m_switches;
  delete m_trims;
}

// static
JsonBoard::BoardDefinition JsonBoard::getBoardDefinition(const BoardDefinitionsTable & tbl, Board::Type board)
{
  auto it =
    find_if(tbl.cbegin(), tbl.cend(), [=](const BoardDefinition& bd) {
      if (bd.board == board) return true;
      return false;
    });

  if (it != tbl.end())
    return *it;

  return tbl[0];  // BOARD_UNKNOWN
}

// virtual
const int JsonBoard::getCapability(const Board::Capability capability) const
{
  switch (capability) {
    case Board::FunctionSwitches:
      return m_funcSwitchesCnt;

    case Board::NumFunctionSwitchesPositions:
      return m_funcSwitchesCnt * 3;

    case Board::MaxAnalogs:
      return m_analogs->size();

    case Board::Pots:
      return m_potsCnt;

    case Board::Sliders:
      return m_slidersCnt;

    case Board::Sticks:
      return m_sticksCnt;

    case Board::Switches:
      return m_switchesCnt;

    case Board::SwitchPositions:
      return m_switchesCnt * 3;

    case Board::NumTrims:
      return m_trims->size();

    case Board::NumTrimSwitches:
      return m_trims->size() * 2;

    default:
      return Boards::getCapability(capability);
  }
}

// static
unsigned int JsonBoard::getPotsCount(const AnalogsTable & analogs)
{
  unsigned int cnt = 0;

  for (const auto &ana : analogs) {
    if (ana.type == "FLEX" && (ana.dflt == "POT" || ana.dflt == "POT_CENTER" || ana.dflt == "MULTIPOS"))
      cnt++;
  }

  return cnt;
}

// static
unsigned int JsonBoard::getSlidersCount(const AnalogsTable & analogs)
{
  unsigned int cnt = 0;

  for (const auto &ana : analogs) {
    if (ana.type == "FLEX" && ana.dflt == "SLIDER")
      cnt++;
  }

  return cnt;
}

// static
unsigned int JsonBoard::getSticksCount(const AnalogsTable & analogs)
{
  unsigned int cnt = 0;

  for (const auto &ana : analogs) {
    if (ana.type == "STICK")
      cnt++;
  }

  return cnt;
}

// static
unsigned int JsonBoard::getSwitchesCount(const SwitchesTable & switches)
{
  unsigned int cnt = 0;

  for (const auto &swtch : switches) {
    if (swtch.type != Board::SWITCH_FSWITCH)
      cnt++;
  }

  return cnt;
}

// static
unsigned int JsonBoard::getFuncSwitchesCount(const SwitchesTable & switches)
{
  unsigned int cnt = 0;

  for (const auto &swtch : switches) {
    if (swtch.type == Board::SWITCH_FSWITCH)
      cnt++;
  }

  return cnt;
}

// virtual
const Board::SwitchInfo JsonBoard::getSwitchInfo(int index) const
{
  if (index < 0 || index >= (int)m_switches->size())
    return {Board::SWITCH_NOT_AVAILABLE, CPN_STR_UNKNOWN_ITEM};

  return { m_switches->at(index).type, QString(m_switches->at(index).name.c_str()) };
}

bool JsonBoard::loadDefinition()
{
  if (m_boardDef.board == Board::BOARD_UNKNOWN)
    return false;

  if (!loadFile(m_boardDef, m_analogs, m_switches, m_trims))
    return false;

  m_sticksCnt = getSticksCount(*m_analogs);
  m_potsCnt = getPotsCount(*m_analogs);
  m_slidersCnt = getSlidersCount(*m_analogs);
  m_switchesCnt = getSwitchesCount(*m_switches);
  m_funcSwitchesCnt = getFuncSwitchesCount(*m_switches);
  //qDebug() << "sticks:" << m_sticksCnt << "pots:" << m_potsCnt << "sliders:" << m_slidersCnt << "switches:" << m_switchesCnt << "funcswitches:" << m_funcSwitchesCnt;
  return true;
}

// static
bool JsonBoard::loadFile(BoardDefinition & boardDef, AnalogsTable * analogs, SwitchesTable * switches, TrimsTable * trims)
{
  if (boardDef.board == Board::BOARD_UNKNOWN) {
    qDebug() << "No board definition for board id:" << boardDef.board;
    return false;
  }

  if (boardDef.jsonFile.empty()) {
    qDebug() << "No json filename for board:" << boardDef.name.c_str();
    return false;
  }

  //  retrieve from application resources
  QString path = QString(":/hw_defs/%1.json").arg(boardDef.jsonFile.c_str());
  QFile file(path);

  if (!file.exists()) {
    qDebug() << "File not found:" << path;
    return false;
  }

  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Unable to open:" << path;
    return false;
  }

  QByteArray buffer = file.readAll();
  file.close();

  if (buffer.isEmpty()) {
    qDebug() << "Unable to read:" << path;
    return false;
  }

  QJsonParseError res;
  QJsonDocument *json = new QJsonDocument();
  *json = QJsonDocument::fromJson(buffer, &res);

  if (res.error || json->isNull() || !json->isObject()) {
    qDebug() << path << "is not a valid json formatted file. Error:" << res.error << res.errorString();
    delete json;
    return false;
  }

  const QJsonObject &obj = json->object();

  if (obj.value("adc_inputs").isObject()) {
    const QJsonObject &adcinputs = obj.value("adc_inputs").toObject();

    if (adcinputs.value("inputs").isArray()) {
      const QJsonArray &inputs = adcinputs.value("inputs").toArray();

      for (const QJsonValue &input : inputs)
      {
        if (input.isObject()) {
          const QJsonObject &o = input.toObject();
          AnalogDefn ana;

          if (!o.value("name").isUndefined())
            ana.name = o.value("name").toString().toStdString();

          if (!o.value("type").isUndefined())
            ana.type = o.value("type").toString().toStdString();

          if (!o.value("label").isUndefined())
            ana.label = o.value("label").toString().toStdString();

          if (!o.value("short_label").isUndefined())
            ana.shortLabel = o.value("short_label").toString().toStdString();

          if (!o.value("default").isUndefined())
            ana.dflt = o.value("default").toString().toStdString();

          analogs->insert(analogs->end(), ana);
          //qDebug() << "name:" << ana.name.c_str() << "type:" << ana.type.c_str() << "label:" << ana.label.c_str() <<
          //            "short:" << ana.shortLabel.c_str() << "default:" << ana.dflt.c_str();
        }
      }
    }
  }

  if (obj.value("switches").isArray()) {
    const QJsonArray &swtchs = obj.value("switches").toArray();

    for (const QJsonValue &swtch : swtchs)
    {
      if (swtch.isObject()) {
        const QJsonObject &o = swtch.toObject();
        SwitchDefn sw;

        if (!o.value("name").isUndefined())
          sw.name = o.value("name").toString().toStdString();

        std::string type_str;
        if (!o.value("type").isUndefined())
          type_str = o.value("type").toString().toStdString();

        if (!o.value("flags").isUndefined())
          sw.flags = o.value("flags").toInt();

        std::string dflt_str;
        if (!o.value("default").isUndefined())
          dflt_str = o.value("default").toString().toStdString();

        if (o.value("display").isArray()) {
          const QJsonArray &d = obj.value("display").toArray();
          sw.display.x = (unsigned int)d.at(0).toInt(0);
          sw.display.y = (unsigned int)d.at(1).toInt(0);
        }

        int idx = -1;

        if (!dflt_str.empty())
          idx = DataHelpers::getStringTagMappingIndex(switchTypesLookupTable, dflt_str.c_str());
        else if (!type_str.empty())
          idx = DataHelpers::getStringTagMappingIndex(switchTypesLookupTable, type_str.c_str());

        sw.type = idx < 0 ? Board::SWITCH_NOT_AVAILABLE : (Board::SwitchType)idx;

        switches->insert(switches->end(), sw);
        //qDebug() << "name:" << sw.name.c_str() << "type:" << (int)sw.type << "flags:" << sw.flags <<
        //            "default:" << sw.dflt.c_str() << "display:" << sw.display.x << "," << sw.display.y;
      }
    }
  }

  if (obj.value("trims").isArray()) {
    const QJsonArray &trms = obj.value("trims").toArray();

    for (const QJsonValue &trm : trms)
    {
      if (trm.isObject()) {
        const QJsonObject &o = trm.toObject();
        TrimDefn t;

        if (!o.value("name").isUndefined())
          t.name = o.value("name").toString().toStdString();

        trims->insert(trims->end(), t);
        //qDebug() << "name:" << t.name.c_str();
      }
    }
  }

  delete json;
  return true;
}
