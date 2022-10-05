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

#include <QtWidgets>

class UpdateFactories;
class UpdateParameters;

class SDCardWizardDialog : public QWizard
{
  Q_OBJECT

  public:
    SDCardWizardDialog(QWidget * parent, UpdateFactories * updateFactories);
    virtual ~SDCardWizardDialog() {}

    enum WizardPage {
      Page_None = -1,
      Page_Welcome,
      Page_Options,
      Page_Select,
      Page_Radio,
      Page_Image,
      Page_Sounds,
      Page_Themes,
      Page_Confirm,
      Page_Action,
      Page_Finish
    };

  private slots:
    void showHelp();

  private:
    UpdateFactories *updateFactories;
};

class SDCardCommonPage : public QWizardPage
{
  Q_OBJECT

  public:
    SDCardCommonPage(QWidget * parent);
    virtual ~SDCardCommonPage() {}

  protected:
    QGridLayout *grid;
    QVBoxLayout *layout;
    int row;
};

class SDCardRepoPage : public SDCardCommonPage
{
  Q_OBJECT

  public:
    SDCardRepoPage(QWidget * parent, UpdateFactories * updateFactories, const QString factoryName);
    virtual ~SDCardRepoPage() {}

  protected slots:
    virtual void releaseChanged(const int index) {}

  protected:
    UpdateFactories *factories;
    const QString factoryName;
    UpdateParameters *factoryParams;

    QDialog *status;
    QLabel *msg;
    QLabel *lblReleaseChannel;
    QComboBox *cboReleaseChannel;
    QLabel *lblRelease;
    QComboBox *cboRelease;

    void initializePage();
};

class SDCardWelcomePage : public SDCardCommonPage
{
  Q_OBJECT

  public:
    SDCardWelcomePage(QWidget * parent);
    virtual ~SDCardWelcomePage() {}

  private:
    QCheckBox *chkAdvancedMode;

    int nextId() const;
};

class SDCardOptionsPage : public SDCardCommonPage
{
  Q_OBJECT

  public:
    SDCardOptionsPage(QWidget * parent);
    virtual ~SDCardOptionsPage() {}

  private:
    QLabel *lbl;

    int nextId() const;
};

class SDCardSelectPage : public SDCardCommonPage
{
  Q_OBJECT

  public:
    SDCardSelectPage(QWidget * parent);
    virtual ~SDCardSelectPage() {}

  private:
    QComboBox *cboDrive;
    QCheckBox *chkErase;

    void initializePage();
    bool validatePage();
    void cleanupPage();
    int nextId() const;
};

class SDCardRadioPage : public SDCardCommonPage
{
  Q_OBJECT

  public:
    SDCardRadioPage(QWidget * parent);
    virtual ~SDCardRadioPage() {}

  private:
    QComboBox *cboRadio;

    int nextId() const;
};

class SDCardImagePage : public SDCardRepoPage
{
  Q_OBJECT

  public:
    SDCardImagePage(QWidget * parent, UpdateFactories * updateFactories);
    virtual ~SDCardImagePage() {}

  protected slots:
    virtual void releaseChanged(const int index) override;

  private:
    QComboBox *cboImage;

    int nextId() const;
};

class QStandardItemModel;

class SDCardSoundsPage : public SDCardRepoPage
{
  Q_OBJECT

  public:
    SDCardSoundsPage(QWidget * parent, UpdateFactories * updateFactories);
    virtual ~SDCardSoundsPage() {}

    enum ItemModelDataRoles {
      IMDR_Language = Qt::UserRole,
      IMDR_Name,
      IMDR_Directory,
    };
    Q_ENUM(ItemModelDataRoles)

  protected slots:
    virtual void releaseChanged(const int index) override;

  private:
    QComboBox *cboLang;
    QStandardItemModel *soundPacksItemModel;
    QSortFilterProxyModel *soundPacksFilteredModel;
    QLabel *selSoundPacks;
    QListView *lstSounds;

    void setSoundPacksFilter();
    bool validatePage();
    int nextId() const;
};

class SDCardThemesPage : public SDCardRepoPage
{
  Q_OBJECT

  public:
    SDCardThemesPage(QWidget * parent, UpdateFactories * updateFactories);
    virtual ~SDCardThemesPage() {}

  private:
    QCheckBox *chkInstall;

    int nextId() const;
};

class SDCardConfirmPage : public SDCardCommonPage
{
  Q_OBJECT

  public:
    SDCardConfirmPage(QWidget * parent);
    virtual ~SDCardConfirmPage() {}

  private:
    void initializePage();
    void cleanupPage();
    int nextId() const;
};

class SDCardActionPage : public QWizardPage
{
  Q_OBJECT

  public:
    SDCardActionPage(QWidget * parent);
    virtual ~SDCardActionPage() {}

  private:
    QLabel *lbl;

    bool isComplete() const;
    int nextId() const;
};

class SDCardFinishPage : public QWizardPage
{
  Q_OBJECT

  public:
    SDCardFinishPage(QWidget * parent);
    virtual ~SDCardFinishPage() {}

  private:
    int nextId() const;
};

