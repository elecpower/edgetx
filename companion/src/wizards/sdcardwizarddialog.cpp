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

#include "sdcardwizarddialog.h"
#include "eeprominterface.h"
#include "appdata.h"
#include "helpers.h"
#include "../updates/updateinterface.h"

#include <QRegularExpression>
#include <QStorageInfo>
#include <QJsonDocument>
#include <QStandardItemModel>

SDCardWizardDialog::SDCardWizardDialog(QWidget * parent, UpdateFactories * updateFactories):
  QWizard(parent),
  updateFactories(updateFactories)
{
  setWindowIcon(CompanionIcon("wizard.png"));
  setWindowTitle(tr("SD Card Wizard"));

  setDefaultProperty("QComboBox", "currentText", SIGNAL(currentIndexChanged(int))); // override currentIndex
  setDefaultProperty("QLabel", "text", nullptr);

  setPage(Page_Welcome, new SDCardWelcomePage(this));
  setPage(Page_Options, new SDCardOptionsPage(this));
  setPage(Page_Select, new SDCardSelectPage(this));
  setPage(Page_Radio, new SDCardRadioPage(this));
  setPage(Page_Image, new SDCardImagePage(this, updateFactories));
  setPage(Page_Sounds, new SDCardSoundsPage(this, updateFactories));
  setPage(Page_Themes, new SDCardThemesPage(this, updateFactories));
  setPage(Page_Confirm, new SDCardConfirmPage(this));
  setPage(Page_Action, new SDCardActionPage(this));
  setPage(Page_Finish, new SDCardFinishPage(this));

  setPixmap(QWizard::LogoPixmap, QPixmap(QString(":/themes/yerico/48/wizard.png")));
  setPixmap(QWizard::WatermarkPixmap, QPixmap(QString(":/images/wizard/sdcardwatermark.png")));

  setWizardStyle(ModernStyle);
  setOption(QWizard::NoCancelButton, false);
  setOption(HaveHelpButton, true);
  connect(this, &QWizard::helpRequested, this, &SDCardWizardDialog::showHelp);
}

void SDCardWizardDialog::showHelp()
{
  QString message;

  switch (currentId()) {
    case Page_Welcome:
      message = tr("It is recommended that you choose a good quality SD card of between 4 and 32GB.<br>"
                   "Sizes over 32GB have been known to cause problems on some radios.");
      break;
    case Page_Options:
      message = tr("Select options");
      break;
    case Page_Select:
      message = tr("Select a SD card from the list and whether to erase the card contents.<br>"
                   "Only cards under 65GB will be listed.<br>"
                   "The chosen card must be formatted as FAT32.");
      break;
    case Page_Radio:
      message = tr("Select the radio and language");
      break;
    case Page_Image:
      message = tr("Select the release channel");
      break;
    case Page_Sounds:
      message = tr("Select the release channel after which a choice of sound packs for the language will be listed.");
      break;
    case Page_Themes:
      message = tr("Select whether you want custom themes");
      break;
    case Page_Confirm:
      message = tr("Confirmation of your choices before actioning");
      break;
    case Page_Action:
      message = tr("Reports progress of build");
      break;
    case Page_Finish:
      message = tr("The end");
      break;
    default:
      message = tr("There is no help available for this page.");
  }

  QMessageBox::information(this, tr("SD Card Wizard Help"), message);
}

SDCardCommonPage::SDCardCommonPage(QWidget * parent) :
  QWizardPage(parent)
{
  row = 0;
  grid = new QGridLayout();
  layout = new QVBoxLayout();
  layout->addLayout(grid);
  setLayout(layout);
}

SDCardRepoPage::SDCardRepoPage(QWidget * parent, UpdateFactories * updateFactories, const int factoryId) :
  SDCardCommonPage(parent),
  factories(updateFactories),
  factoryId(factoryId)
{
  status = new QDialog(parent);
  status->setWindowTitle(tr("Downloading Release Metadata"));
  QVBoxLayout *msglayout = new QVBoxLayout(status);
  msg = new QLabel(status);
  msg->setFixedWidth(400);
  msg->setContentsMargins(50, 50, 50, 50);
  msglayout->addWidget(msg);

  factories->resetEnvironment(factoryId);
  factoryParams = factories->getParams(factoryId);

  lblReleaseChannel = new QLabel(tr("Release Channel"));
  cboReleaseChannel = new QComboBox();
  cboReleaseChannel->addItems(ComponentData::releaseChannelsList());

  lblRelease = new QLabel(tr("Release"));
  cboRelease = new QComboBox();

  connect(cboRelease, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](const int index) {
    factories->setReleaseId(factoryId, cboRelease->currentText());
    releaseChanged(index);
  });

  connect(cboReleaseChannel, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](const int index) {
    status->show();
    msg->setText(tr("Retrieving release information for %1").arg(factories->name(factoryId)));
    factories->setReleaseChannel(factoryId, index);
    {
      const QSignalBlocker blocker(cboRelease);
      cboRelease->clear();
      cboRelease->addItems(factories->releases(factoryId));
      cboRelease->setCurrentIndex(-1);
    }
    cboRelease->setCurrentIndex(0);
    status->hide();
  });

  grid->addWidget(lblReleaseChannel, row, 0);
  grid->addWidget(cboReleaseChannel, row++, 1);
  grid->addWidget(lblRelease, row, 0);
  grid->addWidget(cboRelease, row++, 1);

}

void SDCardRepoPage::initializePage()
{
  {
    const QSignalBlocker blocker(cboReleaseChannel);
    cboReleaseChannel->setCurrentIndex(-1);
  }
  cboReleaseChannel->setCurrentIndex(factoryParams->releaseChannel);
}

SDCardWelcomePage::SDCardWelcomePage(QWidget * parent) :
  SDCardCommonPage(parent)
{
  setTitle(tr("Welcome"));
  setSubTitle(tr("This wizard will guide you through the process of creating an EdgeTX SD card"));

  chkAdvancedMode = new QCheckBox(tr("Advanced mode"));
  registerField("advancedmode", chkAdvancedMode);

  QLabel *lbl1 = new QLabel(tr("The SD card MUST be formatted as FAT32."));
  lbl1->setWordWrap(true);
  QLabel *lbl2 = new QLabel(tr("It is recommended you choose a good quality SD card between 4 and 32GB."));
  lbl2->setWordWrap(true);
  QLabel *lbl3 = new QLabel(tr("Note: Sizes over 32GB have been known to cause problems on some radios."));
  lbl3->setWordWrap(true);

  grid->addWidget(lbl1, row++, 0);
  grid->addWidget(new QLabel(), row++, 0);
  grid->addWidget(lbl2, row++, 0);
  grid->addWidget(new QLabel(), row++, 0);
  grid->addWidget(lbl3, row++, 0);
  grid->addWidget(new QLabel("<br>"), row++, 0);
  grid->addWidget(chkAdvancedMode, row++, 0);
}

int SDCardWelcomePage::nextId() const
{
  return SDCardWizardDialog::Page_Options;
}

SDCardOptionsPage::SDCardOptionsPage(QWidget * parent) :
  SDCardCommonPage(parent)
{
  setTitle(tr("Options"));
  setSubTitle(tr("Choose some options"));

  grid->addWidget(new QLabel(tr("Option 1")), row, 0);
  grid->addWidget(new QCheckBox(), row++, 1);
  grid->addWidget(new QLabel(tr("Option 2")), row, 0);
  grid->addWidget(new QCheckBox(), row++, 1);

}

int SDCardOptionsPage::nextId() const
{
  return SDCardWizardDialog::Page_Select;
}

SDCardSelectPage::SDCardSelectPage(QWidget * parent) :
  SDCardCommonPage(parent)
{
  setTitle(tr("Select SD Card"));
  setSubTitle(" ");

  QLabel *lblDrive = new QLabel(tr("Drive/Device:"));
  cboDrive = new QComboBox();
  //registerField("sdDrive*", cboDrive);    //  TODO swap these lines around when testing finished
  registerField("sdDrive", cboDrive);       //  TODO swap these lines around when testing finished

  chkErase = new QCheckBox();
  registerField("sdErase", chkErase);

  QLabel *lblEraseWarning = new QLabel(tr("Warning: Erase will remove all directories and files except for RADIO and MODELS directories!"));
  lblEraseWarning->setWordWrap(true);
  lblEraseWarning->setStyleSheet("color: red");

  grid->addWidget(lblDrive, row, 0);
  grid->addWidget(cboDrive, row++, 1);
  grid->addWidget(new QLabel(), row++, 0);
  grid->addWidget(new QLabel(tr("Erase")), row, 0);
  grid->addWidget(chkErase, row++, 1);

  grid->addWidget(new QLabel("<br>"), row++, 0);
  grid->addWidget(lblEraseWarning, row++, 0, 1, 2);
}

void SDCardSelectPage::initializePage()
{
  cboDrive->clear();
  cboDrive->addItem("");

  //  QStorageInfo v5.12.9 is unable to determine the device type
  //  could use libusb however cross platform effort vs benefit not really worth it
  //  so perform own filtering to lessen drive/device choices

  // 64GB to filter out most SSD and HDDs though really should keep under 32GB due to radio issues
  const qint64 maxBytes = (qint64)65536000000;

  QRegularExpression deviceIncl("^([d-z]:|/(dev|media)/)", QRegularExpression::CaseInsensitiveOption);  //  possible devices
  QRegularExpression rootPathExcl("^/boot/", QRegularExpression::CaseInsensitiveOption);  //  exclusions
  QRegularExpression fstIncl("^(fat32|vfat|msdos)", QRegularExpression::CaseInsensitiveOption);  // Win: "FAT32"; Linux: "vfat"; macOS: "msdos"

  foreach(const QStorageInfo & si, QStorageInfo::mountedVolumes()) {
    //qDebug() << si.rootPath() << si.name() << si.device() << si.displayName() << si.fileSystemType() << si.isReady() << si.isReadOnly() << si.bytesTotal();
    if (!si.isReady() || si.isReadOnly() || si.bytesTotal() > maxBytes ||
        QString(si.rootPath()).contains(rootPathExcl) ||
        !QString(si.device()).contains(deviceIncl) ||
        !QString(si.fileSystemType()).contains(fstIncl))
      continue;

    cboDrive->addItem(si.rootPath());
  }

  cboDrive->setCurrentIndex(0);
}

bool SDCardSelectPage::validatePage()
{
  return cboDrive->currentIndex() > -1;
}

void SDCardSelectPage::cleanupPage()
{
  //cboDrive->setCurrentIndex(-1);
  //chkErase->setChecked(false);
}

int SDCardSelectPage::nextId() const
{
  return SDCardWizardDialog::Page_Radio;
}

SDCardRadioPage::SDCardRadioPage(QWidget * parent) :
  SDCardCommonPage(parent)
{
  setTitle(tr("Radio"));
  setSubTitle(" ");

  const QStringList profileId = g.currentProfile().fwType().split('-');

  QString profileFlavour;
  if (profileId.size() > 1) {
    profileFlavour = profileId.at(1);
    profileFlavour = profileFlavour.replace('+', 'p');
  }

  const QString currLang = profileId.last();

  cboRadio = new QComboBox();

  foreach(Firmware * firmware, Firmware::getRegisteredFirmwares()) {
    const QStringList fwId = firmware->getId().split('-');
    QString fwFlavour;

    if (fwId.size() > 1) {
      fwFlavour = fwId.at(1);
      fwFlavour = fwFlavour.replace('+', 'p');
    }

    cboRadio->addItem(firmware->getName(), fwFlavour);
    if (profileFlavour == fwFlavour) {
      cboRadio->setCurrentIndex(cboRadio->count() - 1);
    }
  }

  registerField("radio", cboRadio);

  QLabel *lblFlavour = new QLabel(this);
  lblFlavour->setVisible(false);
  lblFlavour->setText(cboRadio->currentData().toString());

  registerField("fwFlavour", lblFlavour);

  connect(cboRadio, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](const int index) {
    lblFlavour->setText(cboRadio->currentData().toString());
  });

  grid->addWidget(new QLabel(tr("Radio")), row, 0);
  grid->addWidget(cboRadio, row++, 1);

}

int SDCardRadioPage::nextId() const
{
  return SDCardWizardDialog::Page_Image;
}

SDCardImagePage::SDCardImagePage(QWidget * parent, UpdateFactories * updateFactories) :
  SDCardRepoPage(parent, updateFactories, UpdateInterface::CID_SDCard)
{
  setTitle(tr("Image"));
  setSubTitle(tr("Select the base SD card image to be loaded"));

  cboImage = new QComboBox();
  registerField("sdimage", cboImage);

  grid->addWidget(new QLabel(tr("SD card image")), row, 0);
  grid->addWidget(cboImage, row++, 1);
}

void SDCardImagePage::releaseChanged(const int index)
{
  QStringList imageList;
  QString radioImage;
  QJsonDocument *json = new QJsonDocument();
  factories->getRepoJsonFile(factoryId, "sdcard.json", json);
  /*
  {
    "targets": [
      ["Flysky NV14", "nv14-", "c320x480"],
      ["FrSky Horus X10", "x10-", "c480x272"],
  */
  if (!json->isNull() && json->isObject()) {
    const QJsonObject &obj = json->object();

    if (obj.value("targets").isArray()) {
      const QJsonArray &t = obj.value("targets").toArray();

      foreach (const QJsonValue &v, t) {
        if (v.isArray()) {
          const QJsonArray &i = v.toArray();
          if (i.size() > 2) {
            if (!imageList.contains(i.at(2).toString().toLower()))
              imageList.append(i.at(2).toString().toLower());
            if (i.at(1).toString().toLower().startsWith(field("fwFlavour").toString()))
              radioImage = i.at(2).toString().toLower();
          }
        }
      }
    }
  }

  imageList.sort();

  cboImage->clear();
  cboImage->addItems(imageList);
  cboImage->setCurrentIndex(cboImage->findText(radioImage));

  delete json;
}

int SDCardImagePage::nextId() const
{
  return SDCardWizardDialog::Page_Sounds;
}

SDCardSoundsPage::SDCardSoundsPage(QWidget * parent, UpdateFactories * updateFactories) :
  SDCardRepoPage(parent, updateFactories, UpdateInterface::CID_Sounds)
{
  setTitle(tr("Sounds"));
  setSubTitle(tr("Select one or more sound packs"));

  cboLang = new QComboBox();

  const QString currLang = g.currentProfile().fwType().split('-').last();

  for (const char *lang : getCurrentFirmware()->getFirmwareBase()->languageList()) {
    cboLang->addItem(lang);
    if (currLang == lang)
      cboLang->setCurrentIndex(cboLang->count() - 1);
  }

  soundPacksItemModel = new QStandardItemModel(this);
  soundPacksFilteredModel = new QSortFilterProxyModel(this);
  soundPacksFilteredModel->setSourceModel(soundPacksItemModel);
  soundPacksFilteredModel->setFilterRole(IMDR_Language);
  setSoundPacksFilter();

  lstSounds = new QListView();
  lstSounds->setModel(soundPacksFilteredModel);
  lstSounds->setSelectionMode(QAbstractItemView::ExtendedSelection);

  connect(cboLang, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](const int index) {
    setSoundPacksFilter();
  });

  grid->addWidget(new QLabel(tr("Language")), row, 0);
  grid->addWidget(cboLang, row++, 1);
  grid->addWidget(new QLabel(tr("Sound packs")), row, 0, Qt::AlignTop);
  grid->addWidget(lstSounds, row++, 1);

  selSoundPacks = new QLabel(this);
  selSoundPacks->setVisible(false);
  registerField("soundpacks", selSoundPacks);
}

void SDCardSoundsPage::releaseChanged(const int index)
{
  QJsonDocument *json = new QJsonDocument();
  factories->getReleaseJsonAsset(factoryId, "sounds.json", json);
  /*
  {
      "language": "en-GB",
      "name": "British English Female",
      "description": "British English Female Voice (en-GB-Libby)",
      "directory": "en_gb-libby"
  },
  */
  soundPacksItemModel->clear();

  if (!json->isNull() && json->isArray()) {
    const QJsonArray &arr = json->array();

    foreach (const QJsonValue &v, arr) {
      if (v.isObject()) {
        const QJsonObject obj = v.toObject();
        QStandardItem * item = new QStandardItem();

        if (!obj.value("language").isUndefined())
          item->setData(obj.value("language").toString(), IMDR_Language);
        if (!obj.value("name").isUndefined())
          item->setData(obj.value("name").toString(), IMDR_Name);
        if (!obj.value("description").isUndefined())
          item->setData(obj.value("description").toString(), Qt::DisplayRole);
        if (!obj.value("directory").isUndefined())
          item->setData(obj.value("directory").toString(), IMDR_Directory);

        soundPacksItemModel->appendRow(item);
      }
    }
  }

  delete json;
  setSoundPacksFilter();
}

void SDCardSoundsPage::setSoundPacksFilter()
{
  soundPacksFilteredModel->setFilterRegularExpression(QRegularExpression(cboLang->currentText(), QRegularExpression::CaseInsensitiveOption));
}

bool SDCardSoundsPage::validatePage()
{
  QItemSelectionModel *selItems = lstSounds->selectionModel();

  if (!selItems->hasSelection())
    return false;

  QModelIndexList selIndexes = selItems->selectedIndexes();

  QString str;

  for (int i = 0; i < selIndexes.size(); i++) {
    if (i > 0)
      str.append("||");
    str.append(lstSounds->model()->data(selIndexes.at(i)).toString());
  }

  selSoundPacks->setText(str);

  return true;
}

int SDCardSoundsPage::nextId() const
{
  return SDCardWizardDialog::Page_Themes;
}

SDCardThemesPage::SDCardThemesPage(QWidget * parent, UpdateFactories * updateFactories) :
  SDCardRepoPage(parent, updateFactories, UpdateInterface::CID_Themes)
{
  setTitle(tr("Themes"));
  setSubTitle(tr("Select to have custom themes included"));

  chkInstall = new QCheckBox();
  registerField("themes", chkInstall);

  grid->addWidget(new QLabel(tr("Include")), row, 0);
  grid->addWidget(chkInstall, row++, 1);
}

int SDCardThemesPage::nextId() const
{
  return SDCardWizardDialog::Page_Confirm;
}

SDCardConfirmPage::SDCardConfirmPage(QWidget * parent) :
  SDCardCommonPage(parent)
{
  setTitle(tr("Confirmation"));
  setSubTitle(tr(" "));

  setCommitPage(true);
}

void SDCardConfirmPage::initializePage()
{
  grid->addWidget(new QLabel(tr("SD card")), row, 0);
  grid->addWidget(new QLabel(field("sdDrive").toString()), row++, 1);
  grid->addWidget(new QLabel(tr("Erase SD card")), row, 0);
  grid->addWidget(new QLabel(field("sdErase").toBool() ? tr("Yes") : tr("No")), row++, 1);
  grid->addWidget(new QLabel(tr("Radio")), row, 0);
  grid->addWidget(new QLabel(field("radio").toString()), row++, 1);
  grid->addWidget(new QLabel(tr("Base image")), row, 0);
  grid->addWidget(new QLabel(field("sdimage").toString()), row++, 1);
  grid->addWidget(new QLabel(tr("Sound packs")), row, 0);
  QStringList strl = field("soundpacks").toString().split("||");
  for (int i = 0; i < strl.count(); i++) {
    grid->addWidget(new QLabel(strl.at(i)), row++, 1);
  }
  grid->addWidget(new QLabel(tr("Custom themes")), row, 0);
  grid->addWidget(new QLabel(field("themes").toBool() ? tr("Yes") : tr("No")), row++, 1);
}

void SDCardConfirmPage::cleanupPage()
{
  row = 0;

  while (!grid->isEmpty()) {
    QLayoutItem* item = grid->itemAt(0);
    grid->removeItem(item);
    QWidget* widget = item->widget();
    if (widget)
      delete widget;
  }
}

int SDCardConfirmPage::nextId() const
{
  return SDCardWizardDialog::Page_Action;
}

SDCardActionPage::SDCardActionPage(QWidget * parent) :
  QWizardPage(parent)
{
  setTitle(tr("Action"));
  setSubTitle(tr(" "));

}

bool SDCardActionPage::isComplete() const
{
  return true;  //  need to block until processes have finished
}

int SDCardActionPage::nextId() const
{
  return SDCardWizardDialog::Page_Finish;
}

SDCardFinishPage::SDCardFinishPage(QWidget * parent) :
  QWizardPage(parent)
{
  setTitle(tr("Finish"));
  setSubTitle(tr(" "));

}

int SDCardFinishPage::nextId() const
{
  return -1;
}
