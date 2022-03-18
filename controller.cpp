/*
 * Copyright (C) 2018 - Florent Revest <revestflo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "controller.h"

Controller::Controller(QObject *parent) : QObject(parent)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_watchConnection = new WatchConnection();

    m_batteryService = m_watchConnection->batteryService();
    m_sc = new Scanner(this);
    connect(m_sc, SIGNAL(watchFound(Watch*)), this, SLOT(onWatchFound(Watch*)));

    m_sc->startScan();
}

void Controller::onWatchFound(Watch *w)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_sc->stopScan();
    m_watchConnection->setDevice(w);
    qInfo() << w->getName() << "found";
    connect(m_batteryService, SIGNAL(ready()), this, SLOT(batteryServiceReady()));
    connect(m_batteryService, &BatteryService::levelChanged, this, &Controller::levelChanged);
}

void Controller::batteryServiceReady()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    auto lvl = m_batteryService->level();
    qInfo() << "Battery service:";
    qInfo() << " level="  << lvl;
}

void Controller::levelChanged(quint8 battlevel)
{
    qInfo() << "Battery level changed to " << battlevel;
}
