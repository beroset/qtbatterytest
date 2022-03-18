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

#include "batteryservice.h"

BatteryService::BatteryService(QObject *parent) 
    : QObject(parent)
{ 
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
}

void BatteryService::onServiceDiscovered()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_lvlChrc = m_service->characteristic(QBluetoothUuid::CharacteristicType::BatteryLevel);
    qDebug() << "Valid characteristic = " << m_lvlChrc.isValid();

    notification = m_lvlChrc.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
    qDebug() << "Valid notification = " << notification.isValid();
    m_service->writeDescriptor(notification, QByteArray::fromHex("0100"));
    // wait for the write to complete?

}

quint8 BatteryService::level()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if(m_service && m_lvlChrc.isValid()) {
        QByteArray ba = m_lvlChrc.value();
        return ba.at(0);
    } else
        return 0;
}

void BatteryService::onCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if (c.uuid() == QBluetoothUuid::CharacteristicType::BatteryLevel) {
        quint8 val = value[0];

        emit levelChanged(val);
    }
}

void BatteryService::onCharacteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    qDebug() << "name = " << c.name();
    qDebug() << "value = " << value;
    if (c.uuid() == QBluetoothUuid::CharacteristicType::BatteryLevel) {
        quint8 val = value[0];
        emit levelChanged(val);
    }
}

void BatteryService::serviceDiscovered(const QBluetoothUuid &uuid)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if(uuid == m_uuid)
    {
        qDebug() << "Found battery service";
        m_foundService = true;
    }
}

void BatteryService::serviceScanDone(QLowEnergyController *ctrl)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if (m_service) {
        delete m_service;
        m_service = 0;
    }

    if (m_foundService)
        m_service = ctrl->createServiceObject(m_uuid, this);

    if (m_service) {
        connect(m_service, &QLowEnergyService::stateChanged, this, &BatteryService::serviceStateChanged);
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &BatteryService::characteristicChanged);
        connect(m_service, &QLowEnergyService::characteristicRead, this, &BatteryService::characteristicRead);
        connect(m_service, &QLowEnergyService::descriptorWritten, this, &BatteryService::descriptorWritten);
        m_service->discoverDetails();
    }
}

void BatteryService::disconnectService()
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_foundService = false;
    delete m_service;
    m_service = nullptr;
}

void BatteryService::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    qDebug() << "State = " << s;
    if(s == QLowEnergyService::RemoteServiceDiscovered)
        onServiceDiscovered();
}

void BatteryService::characteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    onCharacteristicChanged(c, value);
}

void BatteryService::characteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    onCharacteristicRead(c, value);
}

void BatteryService::descriptorWritten(const QLowEnergyDescriptor &c, const QByteArray &value)
{
    qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    qDebug() << "Descriptor: " << c.name();
    qDebug() << "Value: " << value;
    m_service->readCharacteristic(m_lvlChrc);

    if(m_lvlChrc.isValid())
    {
      qDebug() << "about to emit ready()";
      emit ready();
    }
}
