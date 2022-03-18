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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "scanner.h"
#include "watch.h"
#include "watchconnection.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

private slots:
    void onWatchFound(Watch *);

    void batteryServiceReady();
    void levelChanged(quint8 battlevel);

private:
    Watch *m_watch;
    WatchConnection *m_watchConnection;
    Scanner *m_sc;

    BatteryService *m_batteryService;
};

#endif // CONTROLLER_H
