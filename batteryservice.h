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

#ifndef BATTERYSERVICE_H
#define BATTERYSERVICE_H

#include <QObject>

#include <QBluetoothUuid>
#include <QLowEnergyService>
#include <QLowEnergyController>

class BatteryService : public QObject
{
    Q_OBJECT

public:
    BatteryService(QObject *parent = nullptr);

    quint8 level();

    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceScanDone(QLowEnergyController *ctrl);
    void disconnectService();

signals:
    void levelChanged(quint8 newLvl);
    void ready();

private slots:
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void characteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &c, const QByteArray &value);

protected:
    void onServiceDiscovered();
    void onCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void onCharacteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value);

    bool m_foundService;
    QLowEnergyService *m_service = nullptr;
    QBluetoothUuid m_uuid = QBluetoothUuid::ServiceClassUuid::BatteryService;

private:
    QLowEnergyCharacteristic m_lvlChrc;
    QLowEnergyDescriptor notification;
};

#endif // BATTERYSERVICE_H
