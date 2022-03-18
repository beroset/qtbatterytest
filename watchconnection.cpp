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

#include "watchconnection.h"

WatchConnection::WatchConnection(QObject *parent) : QObject(parent)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_control = nullptr;
    m_currentDevice = nullptr;

    m_batteryService = new BatteryService();

    m_isConnected = false;

    m_reconnectTimer.setSingleShot(true);
    QObject::connect(&m_reconnectTimer, &QTimer::timeout, this, &WatchConnection::reconnect);
}

void WatchConnection::setDevice(Watch *device)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if(m_currentDevice != device) {
        m_currentDevice = device;
        emit currentWatchChanged();
    }

    m_connectionAttempts = 0;
    scheduleReconnect();
}

void WatchConnection::scheduleReconnect()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if (m_connectionAttempts == 0)
        reconnect();
    else if (m_connectionAttempts < 25)
        m_reconnectTimer.start(1000 * 10);
    else if (m_connectionAttempts < 35)
        m_reconnectTimer.start(1000 * 60);
    else
        m_reconnectTimer.start(1000 * 60 * 15);
}

void WatchConnection::reconnect()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = 0;
    }

    if (m_currentDevice) {
        m_control = QLowEnergyController::createCentral(m_currentDevice->getDevice(), this);
        m_control->setRemoteAddressType(QLowEnergyController::PublicAddress);
        connect(m_control, &QLowEnergyController::serviceDiscovered, this, &WatchConnection::serviceDiscovered);
        connect(m_control, &QLowEnergyController::discoveryFinished, this, &WatchConnection::serviceScanDone);
//        connect(m_control, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &WatchConnection::connectionError);
        connect(m_control, &QLowEnergyController::connected, this, &WatchConnection::deviceConnected);
        connect(m_control, &QLowEnergyController::disconnected, this, &WatchConnection::deviceDisconnected);

        m_control->connectToDevice();
    }
    m_connectionAttempts++;
}

void WatchConnection::connectionError(QLowEnergyController::Error err)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    emit error(err);
}

void WatchConnection::deviceConnected()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_isConnected = true;
    emit connected();
    m_control->discoverServices();
    m_connectionAttempts = 1;
}

void WatchConnection::deviceDisconnected()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_isConnected = false;
    emit disconnected();
    if (!m_reconnectTimer.isActive())
        scheduleReconnect();
}

bool WatchConnection::isConnected()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    return m_isConnected;
}

Watch *WatchConnection::currentWatch()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    return m_currentDevice;
}

void WatchConnection::serviceDiscovered(const QBluetoothUuid &gatt)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_batteryService->serviceDiscovered(gatt);
}

void WatchConnection::serviceScanDone()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_batteryService->serviceScanDone(m_control);

    emit servicesDiscovered();
}


BatteryService *WatchConnection::batteryService()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    return m_batteryService;
}

void WatchConnection::disconnectService() {
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_batteryService->disconnectService();

    if(m_control)
        m_control->disconnectFromDevice();
}
