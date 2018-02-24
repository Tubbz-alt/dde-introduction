/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "worker.h"
#include <QDebug>
#include <QJsonDocument>

Worker *Worker::Instance()
{
    static Worker * instance = new Worker;
    return instance;
}

void Worker::onWMChanged(const QString &wm)
{
    m_model->setWmType(wm == "deepin wm" ? Model::WM_3D : Model::WM_2D);
}

void Worker::onDisplayModeChanged(int mode)
{
    m_model->setDesktopMode((Model::DesktopMode)mode);
}

void Worker::onIconRefreshed(const QString &name)
{
    if (name == "icon") {
        QDBusPendingReply<QString> icon = m_iconInter->List("icon");
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(icon, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *w) {
            if (w->isError()) {
                qDebug() << w->error().message();
            } else {
               QDBusPendingReply<QString> reply = *w;
               onIconListChanged(reply.value());
            }

            watcher->deleteLater();
        });
    }
}

void Worker::onIconListChanged(const QString &value)
{
    const QJsonArray &array = QJsonDocument::fromJson(value.toUtf8()).array();

    for (const QJsonValue &value : array) {
        m_model->addIcon(IconStruct::fromJson(value.toObject()));
    }
}

Worker::Worker(QObject *parent)
    : QObject(parent)
    , m_model(Model::Instance())
    , m_iconInter(new Icon("com.deepin.daemon.Appearance",
                           "/com/deepin/daemon/Appearance",
                           QDBusConnection::sessionBus(), this))
    , m_wmInter(new WMSwitcher("com.deepin.WMSwitcher",
                               "/com/deepin/WMSwitcher",
                               QDBusConnection::sessionBus(), this))
    , m_dockInter(new Dock("com.deepin.dde.daemon.Dock",
                           "/com/deepin/dde/daemon/Dock",
                           QDBusConnection::sessionBus(), this))
{
    connect(m_iconInter, &Icon::Refreshed, this, &Worker::onIconRefreshed);
    connect(m_iconInter, &Icon::IconThemeChanged, m_model, &Model::setCurrentIcon);
    connect(m_wmInter, &WMSwitcher::WMChanged, this, &Worker::onWMChanged);
    connect(m_dockInter, &Dock::DisplayModeChanged, this, &Worker::onDisplayModeChanged);

    m_iconInter->setSync(false);
    m_wmInter->setSync(false);
    m_dockInter->setSync(false);

    m_model->setCurrentIcon(m_iconInter->iconTheme());

    onWMChanged(m_wmInter->CurrentWM());
    onDisplayModeChanged(m_dockInter->displayMode());

    onIconRefreshed("icon");
}