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

#include "boardjson.h"
#include "datahelpers.h"
#include "constants.h"

#include <QMessageBox>

static const StringTagMappingTable inputTypesLookupTable = {
    {std::to_string(Board::AIT_NONE),    "NONE"},
    {std::to_string(Board::AIT_STICK),   "STICK"},
    {std::to_string(Board::AIT_FLEX),    "FLEX"},
    {std::to_string(Board::AIT_VBAT),    "VBAT"},
    {std::to_string(Board::AIT_RTC_BAT), "RTC_BAT"},
};

static const StringTagMappingTable flexTypesLookupTable = {
    {std::to_string(Board::FLEX_NONE),        "NONE"},
    {std::to_string(Board::FLEX_POT),         "POT"},
    {std::to_string(Board::FLEX_POT_CENTER),  "POT_CENTER"},
    {std::to_string(Board::FLEX_SLIDER),      "SLIDER"},
    {std::to_string(Board::FLEX_MULTIPOS),    "MULTIPOS"},
    {std::to_string(Board::FLEX_AXIS_X),      "AXIS_X"},
    {std::to_string(Board::FLEX_AXIS_Y),      "AXIS_Y"},
    {std::to_string(Board::FLEX_SWITCH),      "SWITCH"},
};

static const StringTagMappingTable switchTypesLookupTable = {
    {std::to_string(Board::SWITCH_NOT_AVAILABLE), "NONE"},
    {std::to_string(Board::SWITCH_TOGGLE),        "TOGGLE"},
    {std::to_string(Board::SWITCH_2POS),          "2POS"},
    {std::to_string(Board::SWITCH_3POS),          "3POS"},
    {std::to_string(Board::SWITCH_FSWITCH),       "FSWITCH"},
};

BoardJson::BoardJson(Board::Type board, QString hwdefn) :
  m_board(board),
  m_hwdefn(hwdefn),
  m_inputs(new InputsTable),
  m_switches(new SwitchesTable),
  m_trims(new TrimsTable),
  m_flexCnt(0),
  m_funcSwitchesCnt(0),
  m_gyroAxesCnt(0),
  m_joystickAxesCnt(0),
  m_potsCnt(0),
  m_slidersCnt(0),
  m_sticksCnt(0),
  m_switchesCnt(0),
  m_rtcbatCnt(0),
  m_vbatCnt(0)
{

}

BoardJson::~BoardJson()
{
  delete m_inputs;
  delete m_switches;
  delete m_trims;
}

// static
void BoardJson::afterLoadFixups(Board::Type board, InputsTable * inputs, SwitchesTable * switches)
{
  // TODO json files do not contain gyro defs
  // Radio cmake directive IMU is currently used
  if (IS_TARANIS_XLITES(board) || IS_FAMILY_HORUS_OR_T16(board)) {
    if (getInputIndex(inputs, "TILT_X") < 0) {
      InputDefn defn;
      defn.type = AIT_FLEX;
      defn.tag = "TILT_X";
      defn.name = "Tltx";
      defn.shortName = "X";
      defn.flexType = FLEX_AXIS_X;
      defn.inverted = false;
      inputs->insert(inputs->end(), defn);
    }

    if (getInputIndex(inputs, "TILT_Y") < 0) {
      InputDefn defn;
      defn.type = AIT_FLEX;
      defn.tag = "TILT_Y";
      defn.name = "Tlty";
      defn.shortName = "Y";
      defn.flexType = FLEX_AXIS_Y;
      defn.inverted = false;
      inputs->insert(inputs->end(), defn);
    }
  }

  //  Flex switches are not listed in json file
  for (int i = 0; i < CPN_MAX_FLEX_SWITCHES; i++) {
    QString tag = QString("FL%1").arg(i);
    if (getSwitchIndex(switches, tag) < 0) {
      SwitchDefn defn;
      defn.tag = tag.toStdString();
      switches->insert(switches->end(), defn);
    }
  }
}

// called from Boards::getCapability if no capability match
const int BoardJson::getCapability(const Board::Capability capability) const
{
  switch (capability) {
    case Board::FlexInputs:
      return m_flexCnt;

    case Board::FunctionSwitches:
      return m_funcSwitchesCnt;

    case Board::FlexSwitches:
      {
      int sw = (IS_FAMILY_HORUS_OR_T16(m_board) || IS_TARANIS(m_board)) ? 2 : 0;
      assert(sw <= CPN_MAX_FLEX_SWITCHES);
      return sw;
      }

    case GyroAxes:
      return m_gyroAxesCnt;

    case Gyros:
      return getCapability(Board::GyroAxes) / 2;

    case HasRTC:
      return m_rtcbatCnt;

    case HasVBat:
      return m_vbatCnt;

    case Board::Inputs:
      return m_inputs->size();

    case Board::NumTrims:
      return m_trims->size();

    case Board::Pots:
      return m_potsCnt;

    case Board::Sliders:
      return m_slidersCnt;

    case Board::Sticks:
      return m_sticksCnt;

    case Board::Switches:
      return m_switchesCnt;

    default:
      return 0;
  }
}

const int BoardJson::getInputIndex(const QString tag) const
{
  return getInputIndex(m_inputs, tag);
}

// static
int BoardJson::getInputIndex(const InputsTable * inputs, QString tag)
{
  for (int i = 0; i < (int)inputs->size(); i++) {
    if (inputs->at(i).tag.c_str() == tag)
      return i;
  }

  return -1;
}

const QString BoardJson::getInputName(int index) const
{
  return getInputName(m_inputs, index);
}

// static
QString BoardJson::getInputName(const InputsTable * inputs, int index)
{
  if (index > -1 && index < (int)inputs->size())
    return inputs->at(index).name.c_str();

  return CPN_STR_UNKNOWN_ITEM;
}

const QString BoardJson::getInputTag(int index) const
{
  return getInputTag(m_inputs, index);
}

// static
QString BoardJson::getInputTag(const InputsTable * inputs, int index)
{
  if (index > -1 && index < (int)inputs->size())
    return inputs->at(index).tag.c_str();

  return CPN_STR_UNKNOWN_ITEM;
}

const int BoardJson::getSwitchIndex(const QString tag) const
{
  return getSwitchIndex(m_switches, tag);
}

// static
int BoardJson::getSwitchIndex(const SwitchesTable * switches, QString tag)
{
  for (int i = 0; i < (int)switches->size(); i++) {
    if (switches->at(i).tag.c_str() == tag)
      return i;
  }

  return -1;
}

const QString BoardJson::getSwitchName(int index) const
{
  return getSwitchName(m_switches, index);
}

// static
QString BoardJson::getSwitchName(const SwitchesTable * switches, int index)
{
  if (index > -1 && index < (int)switches->size())
    return switches->at(index).name.c_str();

  return CPN_STR_UNKNOWN_ITEM;
}

const QString BoardJson::getSwitchTag(int index) const
{
  return getSwitchName(m_switches, index);
}

// static
QString BoardJson::getSwitchTag(const SwitchesTable * switches, int index)
{
  if (index > -1 && index < (int)switches->size())
    return switches->at(index).tag.c_str();

  return CPN_STR_UNKNOWN_ITEM;
}

const int BoardJson::getInputsCalibrated() const
{
  return getInputsCalibrated(m_inputs);
}

// static
int BoardJson::getInputsCalibrated(const InputsTable * inputs)
{
  unsigned int cnt = 0;

  for (const auto &defn : *inputs) {
    if (isInputCalibrated(defn)) cnt++;
  }

  return cnt;
}

const int BoardJson::getInputTagOffset(QString tag)
{
  return getInputTagOffset(m_inputs, tag);
}

// static
int BoardJson::getInputTagOffset(const InputsTable * inputs, QString tag)
{
  for (int i = 0; i < (int)inputs->size(); i++) {
    if (tag == inputs->at(i).tag.c_str())
      return i;
  }

  return 0;
}

const int BoardJson::getInputPotIndex(int index)
{
  if (getCapability(Board::Pots) > 0)
    return getInputTagOffset(m_inputs, QString("P%1").arg(index));

  return -1;
}

const int BoardJson::getInputSliderIndex(int index)
{
  if (getCapability(Board::Sliders) > 0)
    return getInputTagOffset(m_inputs, QString("SL%1").arg(index));

  return -1;
}

const int BoardJson::getInputTypeOffset(Board::AnalogInputType type)
{
  return getInputTypeOffset(m_inputs, type);
}

// static
int BoardJson::getInputTypeOffset(const InputsTable * inputs, Board::AnalogInputType type)
{
  for (int i = 0; i < (int)inputs->size(); i++) {
    if (type == inputs->at(i).type)
      return i;
  }

  return 0;
}

const Board::InputInfo BoardJson::getInputInfo(int index) const
{
  return getInputInfo(m_inputs, index);
}

// static
Board::InputInfo BoardJson::getInputInfo(const InputsTable * inputs, int index)
{
  Board::InputInfo info;

  if (index >= 0 && index < (int)inputs->size()) {
    InputDefn defn = inputs->at(index);
    info.type = defn.type;
    info.tag = defn.tag;
    info.name = defn.name;
    info.shortName = defn.shortName;
    info.flexType = defn.flexType;
    info.inverted = defn.inverted;
  }

  return info;
}

const Board::SwitchInfo BoardJson::getSwitchInfo(int index) const
{
  return getSwitchInfo(m_switches, index);
}

// static
Board::SwitchInfo BoardJson::getSwitchInfo(const SwitchesTable * switches, int index)
{
  Board::SwitchInfo info;

  if (index >= 0 && index < (int)switches->size()) {
    SwitchDefn defn = switches->at(index);
    info.type = defn.type;
    info.tag = defn.tag;
    info.name = defn.name;
    info.inverted = defn.inverted;
  }

  return info;
}

const bool BoardJson::isInputAvailable(int index) const
{
  return isInputAvailable(m_inputs->at(index));
}

// static
bool BoardJson::isInputAvailable(const InputDefn & defn)
{
  return (defn.type == Board::AIT_STICK ||
          (defn.type == Board::AIT_FLEX && defn.flexType != Board::FLEX_NONE && !isJoystickAxis(defn)));
}

const bool BoardJson::isInputCalibrated(int index) const
{
  return isInputCalibrated(m_inputs->at(index));
}

// static
bool BoardJson::isInputCalibrated(const InputDefn & defn)
{
  return (isStick(defn) || isPot(defn) || isSlider(defn));
}

const bool BoardJson::isInputConfigurable(int index) const
{
  return isInputConfigurable(m_inputs->at(index));
}

// static
bool BoardJson::isInputConfigurable(const InputDefn & defn)
{
  return (isStick(defn) || isPot(defn) || isSlider(defn));
}

const bool BoardJson::isInputIgnored(int index) const
{
  return isInputIgnored(m_inputs->at(index));
}

// static
bool BoardJson::isInputIgnored(const InputDefn & defn)
{
  return isJoystickAxis(defn);
}

const bool BoardJson::isInputPot(int index) const
{
  return !isStick(m_inputs->at(index));
}

const bool BoardJson::isInputStick(int index) const
{
  return isStick(m_inputs->at(index));
}

// static
bool BoardJson::isFlex(const InputDefn & defn)
{
  return defn.type == Board::AIT_FLEX;
}

// static
bool BoardJson::isFlexSwitch(const InputDefn & defn)
{
  return defn.type == Board::AIT_FLEX && defn.flexType == Board::FLEX_SWITCH;
}

// static
bool BoardJson::isFuncSwitch(const SwitchDefn & defn)
{
  return defn.type == Board::SWITCH_FSWITCH;
}

// static
bool BoardJson::isGyroAxis(const InputDefn & defn)
{
  const char* val = defn.tag.data();
  size_t len = defn.tag.size();

  return (defn.type == Board::AIT_FLEX && len > 5 &&
          val[0] == 'T' && val[1] == 'I'  && val[2] == 'L' && val[3] == 'T' && val[4] == '_' && (val[5] == 'X' || val[5] == 'Y'));
}

// static
bool BoardJson::isJoystickAxis(const InputDefn & defn)
{
  const char* val = defn.tag.data();
  size_t len = defn.tag.size();

  return (defn.type == Board::AIT_FLEX && len > 2 &&
          val[0] == 'J' && val[1] == 'S' && (val[2] == 'x' || val[2] == 'y'));
}

// static
bool BoardJson::isMultipos(const InputDefn & defn)
{
  return defn.type == Board::AIT_FLEX && defn.flexType == Board::FLEX_MULTIPOS;
}

// static
bool BoardJson::isPot(const InputDefn & defn)
{
  const char* val = defn.tag.data();
  size_t len = defn.tag.size();

  return (defn.type == Board::AIT_FLEX &&
          ((len > 1 && val[0] == 'P' && val[1] >= '0' && val[1] <= '9') ||
           (len > 3 && val[0] == 'E' && val[1] == 'X' && val[2] == 'T' && val[3] >= '0' && val[3] <= '9')));
}

// static
bool BoardJson::isSlider(const InputDefn & defn)
{
  const char* val = defn.tag.data();
  size_t len = defn.tag.size();

  return (defn.type == Board::AIT_FLEX && len > 2 &&
          val[0] == 'S' && val[1] == 'L' && val[2] >= '0' && val[2] <= '9');
}

// static
bool BoardJson::isRTCBat(const InputDefn & defn)
{
  return defn.type == Board::AIT_RTC_BAT;
}

// static
bool BoardJson::isStick(const InputDefn & defn)
{
  return defn.type == Board::AIT_STICK;
}

// static
bool BoardJson::isSwitch(const SwitchDefn & defn)
{
  return defn.type != Board::SWITCH_FSWITCH;
}

// static
bool BoardJson::isVBat(const InputDefn & defn)
{
  return defn.type == Board::AIT_VBAT;
}

bool BoardJson::loadDefinition()
{
  // safety net for BoardFactory::instance
  if (m_board == Board::BOARD_UNKNOWN)
    return true;

  if (!loadFile(m_board, m_hwdefn, m_inputs, m_switches, m_trims))
    return false;

  afterLoadFixups(m_board, m_inputs, m_switches);

  m_flexCnt = setFlexCount(m_inputs);
  m_gyroAxesCnt = setGyroAxesCount(m_inputs);
  m_joystickAxesCnt = setJoystickAxesCount(m_inputs);
  m_potsCnt = setPotsCount(m_inputs);
  m_slidersCnt = setSlidersCount(m_inputs);
  m_sticksCnt = setSticksCount(m_inputs);
  m_funcSwitchesCnt = setFuncSwitchesCount(m_switches);
  m_switchesCnt = setSwitchesCount(m_switches);
  m_rtcbatCnt = setRTCBatCount(m_inputs);
  m_vbatCnt = setVBatCount(m_inputs);

  // json files do not normally specify stick labels so load legacy labels
  for (int i = 0; i < getCapability(Board::Sticks); i++) {
    if (m_inputs->at(i).name.empty())
      m_inputs->at(i).name = setStickName(i);
  }

  qDebug() << "Board:" << Boards::getBoardName(m_board) <<
              "inputs:" << getCapability(Board::Inputs) <<
              "sticks:" << getCapability(Board::Sticks) <<
              "flex:" << getCapability(Board::FlexInputs) <<
              "pots:" << getCapability(Board::Pots) <<
              "sliders:" << getCapability(Board::Sliders) <<
              "gyro axes:" << getCapability(Board::GyroAxes) <<
              "joystick axes:" << getCapability(Board::JoystickAxes) <<
              "trims:" << getCapability(Board::NumTrims) <<
              "switches:" << getCapability(Board::Switches) <<
              "funcswitches:" << getCapability(Board::FunctionSwitches) <<
              "rtcbat:" << getCapability(Board::HasRTC) <<
              "vbat:" << getCapability(Board::HasVBat);

  return true;
}

// static
bool BoardJson::loadFile(Board::Type board, QString hwdefn, InputsTable * inputs, SwitchesTable * switches, TrimsTable * trims)
{
  if (board == Board::BOARD_UNKNOWN) {
    return false;
  }

  // required because of the way the Firmware class is used
  if (hwdefn.isEmpty()) {
    return false;
  }

  QString path = QString(":/hwdefs/%1.json").arg(hwdefn);
  QFile file(path);

  if (!file.exists()) {
    QMessageBox::critical(nullptr, tr("Load Board Hardware Definition"),
                          tr("Board: %1\nError: Unable to load file %2").arg(Boards::getBoardName(board)).arg(path));
    return false;
  }

  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(nullptr, tr("Load Board Hardware Definition"),
                          tr("Board: %1\nError: Unable to open file %2").arg(Boards::getBoardName(board)).arg(path));
    return false;
  }

  QByteArray buffer = file.readAll();
  file.close();

  if (buffer.isEmpty()) {
    QMessageBox::critical(nullptr, tr("Load Board Hardware Definition"),
                          tr("Board: %1\nError: Unable to read file %2").arg(Boards::getBoardName(board)).arg(path));
    return false;
  }

  QJsonParseError res;
  QJsonDocument *json = new QJsonDocument();
  *json = QJsonDocument::fromJson(buffer, &res);

  if (res.error || json->isNull() || !json->isObject()) {
    QMessageBox::critical(nullptr, tr("Load Board Hardware Definition"),
                          tr("Board: %1\nError: %2 is not a valid json formatted file.\nError code: %3\nError description: %4").arg(Boards::getBoardName(board)).arg(path).arg(res.error).arg(res.errorString()));
    delete json;
    return false;
  }

  const QJsonObject &obj = json->object();

  if (obj.value("adc_inputs").isObject()) {
    const QJsonObject &adcinputs = obj.value("adc_inputs").toObject();

    if (adcinputs.value("inputs").isArray()) {
      const QJsonArray &in = adcinputs.value("inputs").toArray();

      for (const QJsonValue &input : in)
      {
        if (input.isObject()) {
          const QJsonObject &o = input.toObject();
          InputDefn defn;

          if (!o.value("name").isUndefined())
            defn.tag = o.value("name").toString().toStdString();

          if (!o.value("type").isUndefined()) {
            std::string type = o.value("type").toString().toStdString();
            defn.type = (AnalogInputType)DataHelpers::getStringTagMappingIndex(inputTypesLookupTable, type.c_str());
          }

          if (!o.value("inverted").isUndefined())
            defn.inverted = o.value("inverted").toBool();

          if (!o.value("label").isUndefined())
            defn.name = o.value("label").toString().toStdString();

          if (!o.value("short_label").isUndefined())
            defn.shortName = o.value("short_label").toString().toStdString();

          if (!o.value("default").isUndefined()) {
            std::string dflt = o.value("default").toString().toStdString();
            if (defn.type == AIT_FLEX) {
              int idx = DataHelpers::getStringTagMappingIndex(flexTypesLookupTable, dflt.c_str());
              defn.flexType = (FlexType)(idx < 0 ? FLEX_NONE : idx);
            }
          }

          inputs->insert(inputs->end(), defn);

//          qDebug() << "name:" << defn.name.c_str() <<
//                      "type:" << defn.stype.c_str() << ">" <<
//                      DataHelpers::getStringNameMappingTag(inputTypesLookupTable, std::to_string(defn.type).c_str()).c_str() <<
//                      "label:" << defn.label.c_str() << "short:" << defn.shortLabel.c_str() << "inverted:" << defn.inverted <<
//                      "dflt:" << defn.dflt.c_str() << ">" <<
//                      DataHelpers::getStringNameMappingTag(flexTypesLookupTable, std::to_string(defn.flexType).c_str()).c_str();
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

        if (!o.value("name").isUndefined()) {
          sw.name = o.value("name").toString().toStdString();
          sw.tag = sw.name;
        }

        if (!o.value("type").isUndefined()) {
          std::string type = o.value("type").toString().toStdString();
          int idx = DataHelpers::getStringTagMappingIndex(switchTypesLookupTable, type.c_str());
          sw.type = idx < 0 ? Board::SWITCH_NOT_AVAILABLE : (Board::SwitchType)idx;
        }

        if (!o.value("flags").isUndefined())
          sw.flags = o.value("flags").toInt();

        if (!o.value("inverted").isUndefined())
          sw.inverted = o.value("inverted").toBool();

        if (!o.value("default").isUndefined()) {
          std::string dflt = o.value("default").toString().toStdString();
          int idx = DataHelpers::getStringTagMappingIndex(switchTypesLookupTable, dflt.c_str());
          sw.dflt = idx < 0 ? Board::SWITCH_NOT_AVAILABLE : (Board::SwitchType)idx;
        }

        if (o.value("display").isArray()) {
          const QJsonArray &d = obj.value("display").toArray();
          sw.display.x = (unsigned int)d.at(0).toInt(0);
          sw.display.y = (unsigned int)d.at(1).toInt(0);
        }

        switches->insert(switches->end(), sw);

//        qDebug() << "name:" << sw.name.c_str() << "type:" << sw.stype.c_str() << ">" << Boards::switchTypeToString(sw.type) <<
//                    "flags:" << sw.flags << "default:" << sw.sdflt.c_str() << ">" << Boards::switchTypeToString(sw.dflt) <<
//                    "inverted:" << sw.inverted << "display:" << QString("%1").arg(sw.display.x) << "," << QString("%1").arg(sw.display.y);
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

        if (!o.value("name").isUndefined()) {
          t.name = o.value("name").toString().toStdString();
          t.tag = t.name;
        }

        trims->insert(trims->end(), t);

//        qDebug() << "name:" << t.name.c_str();
      }
    }
  }

  delete json;
  return true;
}

// static
unsigned int BoardJson::setFlexCount(const InputsTable * inputs)
{
  unsigned int cnt = 0;

  for (const auto &defn : *inputs) {
    if (isFlex(defn)) cnt++;
  }

  return cnt;
}

// static
unsigned int BoardJson::setGyroAxesCount(const InputsTable * inputs)
{
  unsigned int cnt = 0;

  for (const auto &defn : *inputs) {
    if (isGyroAxis(defn)) cnt++;
  }

  return cnt;
}

// static
unsigned int BoardJson::setJoystickAxesCount(const InputsTable * inputs)
{
  unsigned int cnt = 0;

  for (const auto &defn : *inputs) {
    if (isJoystickAxis(defn)) cnt++;
  }

  return cnt;
}

// static
unsigned int BoardJson::setPotsCount(const InputsTable * inputs)
{
  unsigned int cnt = 0;

  for (const auto &defn : *inputs) {
    if (isPot(defn)) cnt++;
  }

  return cnt;
}

// static
unsigned int BoardJson::setRTCBatCount(const InputsTable * inputs)
{
  for (const auto &defn : *inputs) {
    if (isRTCBat(defn)) return true;
  }

  return false;
}

// static
unsigned int BoardJson::setSlidersCount(const InputsTable * inputs)
{
  unsigned int cnt = 0;

  for (const auto &defn : *inputs) {
    if (isSlider(defn)) cnt++;
  }

  return cnt;
}

// static
unsigned int BoardJson::setSticksCount(const InputsTable * inputs)
{
  unsigned int cnt = 0;

  for (const auto &defn : *inputs) {
    if (isStick(defn)) cnt++;
  }

  return cnt;
}

// static
unsigned int BoardJson::setFuncSwitchesCount(const SwitchesTable * switches)
{
  unsigned int cnt = 0;

  for (const auto &swtch : *switches) {
    if (isFuncSwitch(swtch)) cnt++;
  }

  return cnt;
}

// static
std::string BoardJson::setStickName(int index)
{
  QStringList strl = { tr("Rud"), tr("Ele"), tr("Thr"), tr("Ail") };
  return strl.value(index, CPN_STR_UNKNOWN_ITEM).toStdString();
}

// static
unsigned int BoardJson::setSwitchesCount(const SwitchesTable * switches)
{
  unsigned int cnt = 0;

  for (const auto &swtch : *switches) {
    if (isSwitch(swtch)) cnt++;
  }

  return cnt;
}

// static
unsigned int BoardJson::setVBatCount(const InputsTable * inputs)
{
  for (const auto &defn : *inputs) {
    if (isVBat(defn)) return true;
  }

  return false;
}
