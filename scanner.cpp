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

#include "scanner.h"

#include "batteryservice.h"

Scanner::Scanner(QObject *parent) : QObject(parent)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &Scanner::addDevice);
//    connect(discoveryAgent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, [=](QBluetoothDeviceDiscoveryAgent::Error error){ this->scanError(error); });
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &Scanner::scanFinished);
}

Scanner::~Scanner()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    delete discoveryAgent;
    qDeleteAll(devices);
    devices.clear();
}

void Scanner::startScan()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    qDeleteAll(devices);
    devices.clear();
    discoveryAgent->start();
}

void Scanner::stopScan()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    discoveryAgent->stop();
}

void Scanner::addDevice(const QBluetoothDeviceInfo &info)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if ((info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
            && info.serviceUuids().contains(QBluetoothUuid::ServiceClassUuid::BatteryService)) {
        Watch *w = new Watch(info);
        devices.append(w);
        emit watchFound(w);
    }
}

void Scanner::scanFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    emit finished();
}

void Scanner::scanError(QBluetoothDeviceDiscoveryAgent::Error err)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    emit error(err);
}

QList<Watch *> Scanner::pairedWatches()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    return devices;
}
