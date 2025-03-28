/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2018 Michail Vourlakos <mvourlakos@gmail.org>
 *
 * This file is part of the libappletdecoration library
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef KDECOARTIONS_PREVIEW_SETTINGS_H
#define KDECOARTIONS_PREVIEW_SETTINGS_H

#include <KDecoration3/DecorationSettings>
#include <KDecoration3/Private/DecorationSettingsPrivate>
#include <QAbstractListModel>
#include <QObject>
#include <QPointer>

class ButtonsModel;
class PreviewBridge;

class BorderSizesModel : public QAbstractListModel
{
    Q_OBJECT
  public:
    explicit BorderSizesModel(QObject *parent = 0);
    virtual ~BorderSizesModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

  private:
    QList<KDecoration3::BorderSize> m_borders =
        QList<KDecoration3::BorderSize>({ KDecoration3::BorderSize::None, KDecoration3::BorderSize::NoSides, KDecoration3::BorderSize::Tiny,
                                          KDecoration3::BorderSize::Normal, KDecoration3::BorderSize::Large, KDecoration3::BorderSize::VeryLarge,
                                          KDecoration3::BorderSize::Huge, KDecoration3::BorderSize::VeryHuge, KDecoration3::BorderSize::Oversized });
};

class PreviewSettings
    : public QObject
    , public KDecoration3::DecorationSettingsPrivate
{
    Q_OBJECT
    Q_PROPERTY(bool onAllDesktopsAvailable READ isOnAllDesktopsAvailable WRITE setOnAllDesktopsAvailable NOTIFY onAllDesktopsAvailableChanged)
    Q_PROPERTY(bool alphaChannelSupported READ isAlphaChannelSupported WRITE setAlphaChannelSupported NOTIFY alphaChannelSupportedChanged)
    Q_PROPERTY(bool closeOnDoubleClickOnMenu READ isCloseOnDoubleClickOnMenu WRITE setCloseOnDoubleClickOnMenu NOTIFY closeOnDoubleClickOnMenuChanged)
    Q_PROPERTY(QAbstractItemModel *leftButtonsModel READ leftButtonsModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *rightButtonsModel READ rightButtonsModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *availableButtonsModel READ availableButtonsModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *borderSizesModel READ borderSizesModel CONSTANT)
    Q_PROPERTY(int borderSizesIndex READ borderSizesIndex WRITE setBorderSizesIndex NOTIFY borderSizesIndexChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
  public:
    explicit PreviewSettings(KDecoration3::DecorationSettings *parent);
    virtual ~PreviewSettings();
    bool isAlphaChannelSupported() const override;
    bool isOnAllDesktopsAvailable() const override;
    bool isCloseOnDoubleClickOnMenu() const override
    {
        return m_closeOnDoubleClick;
    }
    KDecoration3::BorderSize borderSize() const override;

    void setOnAllDesktopsAvailable(bool available);
    void setAlphaChannelSupported(bool supported);
    void setCloseOnDoubleClickOnMenu(bool enabled);

    QAbstractItemModel *leftButtonsModel() const;
    QAbstractItemModel *rightButtonsModel() const;
    QAbstractItemModel *availableButtonsModel() const;
    QAbstractItemModel *borderSizesModel() const
    {
        return m_borderSizes;
    }

    QVector<KDecoration3::DecorationButtonType> decorationButtonsLeft() const override;
    QVector<KDecoration3::DecorationButtonType> decorationButtonsRight() const override;

    Q_INVOKABLE void addButtonToLeft(int row);
    Q_INVOKABLE void addButtonToRight(int row);

    int borderSizesIndex() const
    {
        return m_borderSize;
    }
    void setBorderSizesIndex(int index);

    QFont font() const override
    {
        return m_font;
    }
    void setFont(const QFont &font);

  Q_SIGNALS:
    void onAllDesktopsAvailableChanged(bool);
    void alphaChannelSupportedChanged(bool);
    void closeOnDoubleClickOnMenuChanged(bool);
    void borderSizesIndexChanged(int);
    void fontChanged(const QFont &);

  private:
    bool m_alphaChannelSupported;
    bool m_onAllDesktopsAvailable;
    bool m_closeOnDoubleClick;
    ButtonsModel *m_leftButtons;
    ButtonsModel *m_rightButtons;
    ButtonsModel *m_availableButtons;
    BorderSizesModel *m_borderSizes;
    int m_borderSize;
    QFont m_font;
};

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PreviewBridge *bridge READ bridge WRITE setBridge NOTIFY bridgeChanged)
    Q_PROPERTY(KDecoration3::DecorationSettings *settings READ settingsPointer NOTIFY settingsChanged)
    Q_PROPERTY(int borderSizesIndex READ borderSizesIndex WRITE setBorderSizesIndex NOTIFY borderSizesIndexChanged)
  public:
    explicit Settings(QObject *parent = nullptr);
    virtual ~Settings();

    PreviewBridge *bridge() const;
    void setBridge(PreviewBridge *bridge);

    std::shared_ptr<KDecoration3::DecorationSettings> settings() const;
    KDecoration3::DecorationSettings *settingsPointer() const;
    int borderSizesIndex() const
    {
        return m_borderSize;
    }
    void setBorderSizesIndex(int index);

  Q_SIGNALS:
    void bridgeChanged();
    void settingsChanged();
    void borderSizesIndexChanged(int);

  private:
    void createSettings();
    QPointer<PreviewBridge> m_bridge;
    std::shared_ptr<KDecoration3::DecorationSettings> m_settings;
    PreviewSettings *m_previewSettings = nullptr;
    int m_borderSize = 3;
};

#endif
