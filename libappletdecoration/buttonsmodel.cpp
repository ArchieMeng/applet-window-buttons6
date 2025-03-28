/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2018  Michail Vourlakos <mvourlakos@gmail.org>
 *
 * This file is part of the libappletdecoration library
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "buttonsmodel.h"

#include <KLocalizedString>
#include <QFontDatabase>

ButtonsModel::ButtonsModel(const QVector<KDecoration3::DecorationButtonType> &buttons, QObject *parent) : QAbstractListModel(parent), m_buttons(buttons)
{
}

ButtonsModel::ButtonsModel(QObject *parent)
    : ButtonsModel(QVector<KDecoration3::DecorationButtonType>({
                       KDecoration3::DecorationButtonType::Menu,
                       KDecoration3::DecorationButtonType::ApplicationMenu,
                       KDecoration3::DecorationButtonType::OnAllDesktops,
                       KDecoration3::DecorationButtonType::Minimize,
                       KDecoration3::DecorationButtonType::Maximize,
                       KDecoration3::DecorationButtonType::Close,
                       KDecoration3::DecorationButtonType::ContextHelp,
                       KDecoration3::DecorationButtonType::Shade,
                       KDecoration3::DecorationButtonType::KeepBelow,
                       KDecoration3::DecorationButtonType::KeepAbove,
                   }),
                   parent)
{
}

ButtonsModel::~ButtonsModel() = default;

int ButtonsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_buttons.count();
}

static QString buttonToName(KDecoration3::DecorationButtonType type)
{
    switch (type)
    {
        case KDecoration3::DecorationButtonType::Menu: return i18n("Menu");
        case KDecoration3::DecorationButtonType::ApplicationMenu: return i18n("Application menu");
        case KDecoration3::DecorationButtonType::OnAllDesktops: return i18n("On all desktops");
        case KDecoration3::DecorationButtonType::Minimize: return i18n("Minimize");
        case KDecoration3::DecorationButtonType::Maximize: return i18n("Maximize");
        case KDecoration3::DecorationButtonType::Close: return i18n("Close");
        case KDecoration3::DecorationButtonType::ContextHelp: return i18n("Context help");
        case KDecoration3::DecorationButtonType::Shade: return i18n("Shade");
        case KDecoration3::DecorationButtonType::KeepBelow: return i18n("Keep below");
        case KDecoration3::DecorationButtonType::KeepAbove: return i18n("Keep above");
        default: return QString();
    }
}

QVariant ButtonsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_buttons.count() || index.column() != 0)
        return QVariant();

    const auto &button = m_buttons.at(index.row());

    switch (role)
    {
        case Qt::DisplayRole: return buttonToName(button);
        case Qt::UserRole: return QVariant::fromValue(int(button));
    }

    return QVariant();
}

QHash<int, QByteArray> ButtonsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
    roles.insert(Qt::UserRole, QByteArrayLiteral("button"));
    return roles;
}

void ButtonsModel::remove(int row)
{
    if (row < 0 || row >= m_buttons.count())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_buttons.removeAt(row);
    endRemoveRows();
}

void ButtonsModel::down(int index)
{
    if (m_buttons.count() < 2 || index == m_buttons.count() - 1)
        return;

    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index + 2);
    m_buttons.insert(index + 1, m_buttons.takeAt(index));
    endMoveRows();
}

void ButtonsModel::up(int index)
{
    if (m_buttons.count() < 2 || index == 0)
        return;

    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
    m_buttons.insert(index - 1, m_buttons.takeAt(index));
    endMoveRows();
}

void ButtonsModel::add(KDecoration3::DecorationButtonType type)
{
    beginInsertRows(QModelIndex(), m_buttons.count(), m_buttons.count());
    m_buttons.append(type);
    endInsertRows();
}

void ButtonsModel::add(int index, int type)
{
    beginInsertRows(QModelIndex(), index + 1, index + 1);
    m_buttons.insert(index + 1, KDecoration3::DecorationButtonType(type));
    endInsertRows();
}

void ButtonsModel::move(int sourceIndex, int targetIndex)
{
    if (sourceIndex == qMax(0, targetIndex))
    {
        return;
    }

    /* When moving an item down, the destination index needs to be incremented
       by one, as explained in the documentation:
       http://doc.qt.nokia.com/qabstractitemmodel.html#beginMoveRows */
    if (targetIndex > sourceIndex)
    {
        // Row will be moved down
        beginMoveRows(QModelIndex(), sourceIndex, sourceIndex, QModelIndex(), targetIndex + 1);
    }
    else
    {
        beginMoveRows(QModelIndex(), sourceIndex, sourceIndex, QModelIndex(), qMax(0, targetIndex));
    }

    m_buttons.move(sourceIndex, qMax(0, targetIndex));
    endMoveRows();
}
