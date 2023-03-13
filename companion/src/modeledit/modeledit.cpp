/*
 * Copyright (C) OpenTX
 *
 * Based on code named
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

#include "modeledit.h"
#include "helpers.h"
#include "ui_modeledit.h"
#include "setup.h"
#include "heli.h"
#include "flightmodes.h"
#include "inputs.h"
#include "mixes.h"
#include "channels.h"
#include "curves.h"
#include "verticalscrollarea.h"
#include "logicalswitches.h"
#include "customfunctions.h"
#include "telemetry.h"
#include "colorcustomscreens.h"
#include "telemetry_customscreens.h"
#include "modeloptions.h"
#include "appdata.h"
#include "compounditemmodels.h"

ModelEdit::ModelEdit(QWidget * parent, RadioData & radioData, int modelId, Firmware * firmware) :
  QDialog(parent),
  ui(new Ui::ModelEdit),
  modelId(modelId),
  radioData(radioData),
  firmware(firmware)
{
  Stopwatch s1("ModelEdit");
  gStopwatch.report("ModelEdit start constructor");

  ui->setupUi(this);
  setWindowIcon(CompanionIcon("edit.png"));
  setAttribute(Qt::WA_DeleteOnClose);
  restoreGeometry(g.modelEditGeo());

  toolbarMenuSetup();
  retranslateUi();

  GeneralSettings &generalSettings = radioData.generalSettings;
  ModelData &model = radioData.models[modelId];

  sharedItemModels = new CompoundItemModelFactory(&generalSettings, &model);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_RawSource);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_RawSwitch);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_Curve);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_GVarRef);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_ThrSource);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_CustomFuncAction);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_CustomFuncResetParam);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_TeleSource);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_CurveRefType);
  sharedItemModels->addItemModel(AbstractItemModel::IMID_CurveRefFunc);

  s1.report("Init");

  SetupPanel * setupPanel = new SetupPanel(this, model, generalSettings, firmware, sharedItemModels);
  addTab(setupPanel, tr("Setup"));
  s1.report("Setup");

  if (firmware->getCapability(Heli)) {
    addTab(new HeliPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Heli"));
    s1.report("Heli");
  }

  addTab(new FlightModesPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Flight Modes"));
  s1.report("Flight Modes");

  addTab(new InputsPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Inputs"));
  s1.report("Inputs");

  addTab(new MixesPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Mixes"));
  s1.report("Mixes");

  ChannelsPanel * channelsPanel = new ChannelsPanel(this, model, generalSettings, firmware, sharedItemModels);
  addTab(channelsPanel, tr("Outputs"));
  s1.report("Outputs");

  addTab(new CurvesPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Curves"));
  s1.report("Curves");

  addTab(new LogicalSwitchesPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Logical Switches"));
  s1.report("Logical Switches");

  addTab(new CustomFunctionsPanel(this, &model, generalSettings, firmware, sharedItemModels), tr("Special Functions"));
  s1.report("Special Functions");

  if (firmware->getCapability(Telemetry)) {
    addTab(new TelemetryPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Telemetry"));
    s1.report("Telemetry");
  }

  if (Boards::getCapability(firmware->getBoard(), Board::HasColorLcd)) {
    addTab(new ColorCustomScreensPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Custom Screens"));
    s1.report("ColorLcd Custom Screens");
  }
  else if (firmware->getCapability(TelemetryCustomScreens)) {
    addTab(new TelemetryCustomScreensPanel(this, model, generalSettings, firmware, sharedItemModels), tr("Custom Screens"));
    s1.report("Telemetry Custom Screens");
  }

  addTab(new ModelOptionsPanel(this, model, generalSettings, firmware), tr("Enabled Features"));
  s1.report("Enabled Features");

  connect(setupPanel, &SetupPanel::extendedLimitsToggled, channelsPanel, &ChannelsPanel::refreshExtendedLimits);
  connect(ui->tabWidget, &QTabWidget::currentChanged, this, &ModelEdit::onTabIndexChanged);

  onTabIndexChanged(ui->tabWidget->currentIndex());  // make sure to trigger update on default tab panel

  gStopwatch.report("ModelEdit end constructor");
}

ModelEdit::~ModelEdit()
{
  delete ui;
  delete sharedItemModels;
}

void ModelEdit::closeEvent(QCloseEvent *event)
{
  g.modelEditGeo( saveGeometry() );
}

void ModelEdit::addTab(GenericPanel *panel, QString text)
{
  panels << panel;
  QWidget * widget = new QWidget(ui->tabWidget);
  QVBoxLayout *baseLayout = new QVBoxLayout(widget);
  VerticalScrollArea * area = new VerticalScrollArea(widget, panel);
  baseLayout->addWidget(area);
  ui->tabWidget->addTab(widget, text);
  connect(panel, &GenericPanel::modified, this, &ModelEdit::modified);
  connect(panel, &GenericPanel::updateToolbarMenu, this, &ModelEdit::toolbarMenuUpdate);
  connect(this, &ModelEdit::toolbarAction, panel, &GenericPanel::onAction);
}

void ModelEdit::onTabIndexChanged(int index)
{
  toolbarMenuUpdate(0); //  disable all buttons in case the new tab does not emit updateToolbarMenu

  if (index < panels.size())
    panels.at(index)->update();
}

void ModelEdit::launchSimulation()
{
  startSimulation(this, radioData, modelId);
}

QAction * ModelEdit::addAct(ToolbarMenuActions actId, const QString & icon, const char * slot, const QKeySequence & shortcut, QObject * slotObj)
{
  QAction * newAction = new QAction(this);
  newAction->setMenuRole(QAction::NoRole);
  if (!icon.isEmpty())
    newAction->setIcon(CompanionIcon(icon));
  if (!shortcut.isEmpty())
    newAction->setShortcut(shortcut);
  if (!slotObj)
    slotObj = this;
  if (slot)
    connect(newAction, SIGNAL(triggered()), slotObj, slot);
  action.replace(actId, newAction);
  return newAction;
}

void ModelEdit::toolbarMenuSetup()
{
  foreach (QAction * act, action) {
    if (act)
      act->deleteLater();
  }
  action.clear();
  action.fill(NULL, TBM_ACT_ENUM_END);

  addAct(TBM_ACT_COPY,     "copy.png",     SLOT(copy()),                    tr("Ctrl+Alt+C"));
  addAct(TBM_ACT_PASTE,    "paste.png",    SLOT(paste()),                   tr("Ctrl+Alt+V"));
  addAct(TBM_ACT_SIMULATE, "simulate.png", SLOT(launchSimulation()),        tr("Alt+Shift+S"));

  QToolButton * btn;
  QSize tbIcnSz(16, 16);
  QString tbCss = "QToolBar {border: 1px solid palette(midlight);}";

  if (toolbarMenu)
    toolbarMenu->deleteLater();

  toolbarMenu = new QToolBar(this);
  toolbarMenu->setFloatable(false);
  toolbarMenu->setIconSize(tbIcnSz);
  toolbarMenu->setStyleSheet(tbCss);
  toolbarMenu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  toolbarMenu->addAction(getAction(TBM_ACT_SIMULATE));
  toolbarMenu->addSeparator();
  toolbarMenu->addAction(getAction(TBM_ACT_COPY));
  toolbarMenu->addAction(getAction(TBM_ACT_PASTE));

  if ((btn = qobject_cast<QToolButton *>(toolbarMenu->widgetForAction(action[TBM_ACT_SIMULATE])))) {
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  }

  ui->toolbarMenuLayout->addWidget(toolbarMenu);

}

void ModelEdit::toolbarMenuUpdate(int mask)
{
  mask |= TBM_MASK_SIMULATE; // always available
  action[TBM_ACT_SIMULATE]->setEnabled(mask & TBM_MASK_SIMULATE);
  action[TBM_ACT_COPY]->setEnabled(mask & TBM_MASK_COPY);
  action[TBM_ACT_PASTE]->setEnabled(mask & TBM_MASK_PASTE);
}

void ModelEdit::retranslateUi()
{
  action[TBM_ACT_COPY]->setText(tr("Copy"));
  action[TBM_ACT_PASTE]->setText(tr("Paste"));
  action[TBM_ACT_SIMULATE]->setText(tr("Simulate"));
}

QAction * ModelEdit::getAction(const ToolbarMenuActions type)
{
  if (type < TBM_ACT_ENUM_END)
    return action[type];
  else
    return NULL;
}

void ModelEdit::copy()
{
  qDebug() << "Trace";
  emit toolbarAction(TBM_ACT_COPY);
}

void ModelEdit::paste()
{
  qDebug() << "Trace";
  emit toolbarAction(TBM_ACT_PASTE);
}
