/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2018 Michail Vourlakos <mvourlakos@gmail.org>
 *
 * This file is part of the libappletdecoration library
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "previewsettings.h"

#include "buttonsmodel.h"
#include "previewbridge.h"

#include <KLocalizedString>
#include <QFontDatabase>
#include <memory>

BorderSizesModel::BorderSizesModel(QObject *parent) : QAbstractListModel(parent)
{
}

BorderSizesModel::~BorderSizesModel() = default;

QVariant BorderSizesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_borders.count() || index.column() != 0)
    {
        return QVariant();
    }

    if (role != Qt::DisplayRole && role != Qt::UserRole)
    {
        return QVariant();
    }

    return QVariant::fromValue<KDecoration3::BorderSize>(m_borders.at(index.row()));
}

int BorderSizesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_borders.count();
}

QHash<int, QByteArray> BorderSizesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
    return roles;
}

PreviewSettings::PreviewSettings(KDecoration3::DecorationSettings *parent)
    : QObject(), KDecoration3::DecorationSettingsPrivate(parent), m_alphaChannelSupported(true), m_onAllDesktopsAvailable(true), m_closeOnDoubleClick(false),
      m_leftButtons(
          new ButtonsModel(QVector<KDecoration3::DecorationButtonType>({ KDecoration3::DecorationButtonType::Minimize, KDecoration3::DecorationButtonType::Close,
                                                                         KDecoration3::DecorationButtonType::Menu, KDecoration3::DecorationButtonType::ApplicationMenu,
                                                                         KDecoration3::DecorationButtonType::OnAllDesktops }),
                           this)),
      m_rightButtons(
          new ButtonsModel(QVector<KDecoration3::DecorationButtonType>({ KDecoration3::DecorationButtonType::ContextHelp, KDecoration3::DecorationButtonType::Minimize,
                                                                         KDecoration3::DecorationButtonType::Maximize, KDecoration3::DecorationButtonType::Close }),
                           this)),
      m_availableButtons(
          new ButtonsModel(QVector<KDecoration3::DecorationButtonType>({ KDecoration3::DecorationButtonType::Menu, KDecoration3::DecorationButtonType::ApplicationMenu,
                                                                         KDecoration3::DecorationButtonType::OnAllDesktops, KDecoration3::DecorationButtonType::Minimize,
                                                                         KDecoration3::DecorationButtonType::Maximize, KDecoration3::DecorationButtonType::Close,
                                                                         KDecoration3::DecorationButtonType::ContextHelp, KDecoration3::DecorationButtonType::Shade,
                                                                         KDecoration3::DecorationButtonType::KeepBelow, KDecoration3::DecorationButtonType::KeepAbove }),
                           this)),
      m_borderSizes(new BorderSizesModel(this)), m_borderSize(int(KDecoration3::BorderSize::Normal)), m_font(QFontDatabase::systemFont(QFontDatabase::TitleFont))
{
    connect(this, &PreviewSettings::alphaChannelSupportedChanged, parent, &KDecoration3::DecorationSettings::alphaChannelSupportedChanged);
    connect(this, &PreviewSettings::onAllDesktopsAvailableChanged, parent, &KDecoration3::DecorationSettings::onAllDesktopsAvailableChanged);
    connect(this, &PreviewSettings::closeOnDoubleClickOnMenuChanged, parent, &KDecoration3::DecorationSettings::closeOnDoubleClickOnMenuChanged);
    connect(this, &PreviewSettings::fontChanged, parent, &KDecoration3::DecorationSettings::fontChanged);
    auto updateLeft = [this, parent]() { parent->decorationButtonsLeftChanged(decorationButtonsLeft()); };
    auto updateRight = [this, parent]() { parent->decorationButtonsRightChanged(decorationButtonsRight()); };
    connect(m_leftButtons, &QAbstractItemModel::rowsRemoved, this, updateLeft);
    connect(m_leftButtons, &QAbstractItemModel::rowsMoved, this, updateLeft);
    connect(m_leftButtons, &QAbstractItemModel::rowsInserted, this, updateLeft);
    connect(m_rightButtons, &QAbstractItemModel::rowsRemoved, this, updateRight);
    connect(m_rightButtons, &QAbstractItemModel::rowsMoved, this, updateRight);
    connect(m_rightButtons, &QAbstractItemModel::rowsInserted, this, updateRight);
}

PreviewSettings::~PreviewSettings() = default;

QAbstractItemModel *PreviewSettings::availableButtonsModel() const
{
    return m_availableButtons;
}

QAbstractItemModel *PreviewSettings::leftButtonsModel() const
{
    return m_leftButtons;
}

QAbstractItemModel *PreviewSettings::rightButtonsModel() const
{
    return m_rightButtons;
}

bool PreviewSettings::isAlphaChannelSupported() const
{
    return m_alphaChannelSupported;
}

bool PreviewSettings::isOnAllDesktopsAvailable() const
{
    return m_onAllDesktopsAvailable;
}

void PreviewSettings::setAlphaChannelSupported(bool supported)
{
    if (m_alphaChannelSupported == supported)
    {
        return;
    }

    m_alphaChannelSupported = supported;
    emit alphaChannelSupportedChanged(m_alphaChannelSupported);
}

void PreviewSettings::setOnAllDesktopsAvailable(bool available)
{
    if (m_onAllDesktopsAvailable == available)
    {
        return;
    }

    m_onAllDesktopsAvailable = available;
    emit onAllDesktopsAvailableChanged(m_onAllDesktopsAvailable);
}

void PreviewSettings::setCloseOnDoubleClickOnMenu(bool enabled)
{
    if (m_closeOnDoubleClick == enabled)
    {
        return;
    }

    m_closeOnDoubleClick = enabled;
    emit closeOnDoubleClickOnMenuChanged(enabled);
}

QVector<KDecoration3::DecorationButtonType> PreviewSettings::decorationButtonsLeft() const
{
    return m_leftButtons->buttons();
}

QVector<KDecoration3::DecorationButtonType> PreviewSettings::decorationButtonsRight() const
{
    return m_rightButtons->buttons();
}

void PreviewSettings::addButtonToLeft(int row)
{
    QModelIndex index = m_availableButtons->index(row);

    if (!index.isValid())
    {
        return;
    }

    m_leftButtons->add(index.data(Qt::UserRole).value<KDecoration3::DecorationButtonType>());
}

void PreviewSettings::addButtonToRight(int row)
{
    QModelIndex index = m_availableButtons->index(row);

    if (!index.isValid())
    {
        return;
    }

    m_rightButtons->add(index.data(Qt::UserRole).value<KDecoration3::DecorationButtonType>());
}

void PreviewSettings::setBorderSizesIndex(int index)
{
    if (m_borderSize == index)
    {
        return;
    }

    m_borderSize = index;
    emit borderSizesIndexChanged(index);
    emit decorationSettings()->borderSizeChanged(borderSize());
}

KDecoration3::BorderSize PreviewSettings::borderSize() const
{
    return m_borderSizes->index(m_borderSize).data(Qt::UserRole).value<KDecoration3::BorderSize>();
}

void PreviewSettings::setFont(const QFont &font)
{
    if (m_font == font)
    {
        return;
    }

    m_font = font;
    emit fontChanged(m_font);
}

Settings::Settings(QObject *parent) : QObject(parent)
{
    connect(this, &Settings::bridgeChanged, this, &Settings::createSettings);
}

Settings::~Settings() = default;

void Settings::setBridge(PreviewBridge *bridge)
{
    if (m_bridge == bridge)
    {
        return;
    }

    m_bridge = bridge;
    emit bridgeChanged();
}

PreviewBridge *Settings::bridge() const
{
    return m_bridge.data();
}

void Settings::createSettings()
{
    if (m_bridge.isNull())
    {
        m_settings.reset();
    }
    else
    {
        m_settings = std::make_shared<KDecoration3::DecorationSettings>(m_bridge.data());
        m_previewSettings = m_bridge->lastCreatedSettings();
        m_previewSettings->setBorderSizesIndex(m_borderSize);
        connect(this, &Settings::borderSizesIndexChanged, m_previewSettings, &PreviewSettings::setBorderSizesIndex);
    }

    emit settingsChanged();
}

std::shared_ptr<KDecoration3::DecorationSettings> Settings::settings() const
{
    return m_settings;
}

KDecoration3::DecorationSettings *Settings::settingsPointer() const
{
    return m_settings.get();
}

void Settings::setBorderSizesIndex(int index)
{
    if (m_borderSize == index)
    {
        return;
    }

    m_borderSize = index;
    emit borderSizesIndexChanged(m_borderSize);
}
