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

#include "updatehelpers.h"

QString UpdateHelpers::semanticVersion(QString version)
{
  QStringList strl = version.split(".");

  while (strl.count() < 4) {
    strl.append("0");
  }

  return strl.join(".");
}

void UpdateHelpers::reportProgress(ProgressWidget * progress, QString & text, int type)
{
  if (params->logLevel == QtDebugMsg ||
     (params->logLevel == QtInfoMsg && type > QtDebugMsg) ||
     (type < QtInfoMsg && type >= params->logLevel)) {
    if (progress)
      progress->addMessage(text, type);
    else
      qDebug() << text;
  }

  if (type == QtCriticalMsg || type == QtFatalMsg)
    criticalMsg(text);
}

void UpdateHelpers::progressMessage(ProgressWidget * progress, QString & text)
{
  if (progress)
    progress->setInfo(text);
  else
    qDebug() << text;
}

void UpdateHelpers::criticalMsg(ProgressWidget * progress, QString & msg)
{
  QMessageBox::critical(progress, tr("Update Interface"), msg);
}

