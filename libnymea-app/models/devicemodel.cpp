/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free Software
* Foundation, GNU version 3. This project is distributed in the hope that it
* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "devicemodel.h"

#include "types/statetype.h"

DeviceModel::DeviceModel(QObject *parent) : QAbstractListModel(parent)
{

}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.count();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (role == RoleId) {
        return m_list.at(index.row());
    }
    if (role == RoleType) {
        StateType* stateType = m_device->thingClass()->stateTypes()->getStateType(m_list.at(index.row()));
        if (stateType) {
            return TypeStateType;
        }
        ActionType* actionType = m_device->thingClass()->actionTypes()->getActionType(m_list.at(index.row()));
        if (actionType) {
            return TypeActionType;
        }
        EventType* eventType = m_device->thingClass()->eventTypes()->getEventType(m_list.at(index.row()));
        if (eventType) {
            return TypeEventType;
        }
    }
    if (role == RoleDisplayName) {
        StateType* stateType = m_device->thingClass()->stateTypes()->getStateType(m_list.at(index.row()));
        if (stateType) {
            return stateType->displayName();
        }
        ActionType* actionType = m_device->thingClass()->actionTypes()->getActionType(m_list.at(index.row()));
        if (actionType) {
            return actionType->displayName();
        }
        EventType* eventType = m_device->thingClass()->eventTypes()->getEventType(m_list.at(index.row()));
        if (eventType) {
            return eventType->displayName();
        }
    }
    if (role == RoleWritable) {
        ActionType* actionType = m_device->thingClass()->actionTypes()->getActionType(m_list.at(index.row()));
        return actionType != nullptr;
    }
    return QVariant();
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(RoleId, "id");
    roles.insert(RoleType, "type");
    roles.insert(RoleDisplayName, "displayName");
    roles.insert(RoleWritable, "writable");
    return roles;
}

QVariant DeviceModel::getData(int index, int role) const
{
    if (index < 0 || index >= m_list.count()) {
        return QVariant();
    }
    return data(this->index(index), role);
}

Device *DeviceModel::device() const
{
    return m_device;
}

void DeviceModel::setDevice(Device *device)
{
    if (m_device != device) {
        m_device = device;
        emit deviceChanged();
        updateList();
    }
}

bool DeviceModel::showStates() const
{
    return m_showStates;
}

void DeviceModel::setShowStates(bool showStates)
{
    if (m_showStates != showStates) {
        m_showStates = showStates;
        emit showStatesChanged();
        updateList();
    }
}

bool DeviceModel::showActions() const
{
    return m_showActions;
}

void DeviceModel::setShowActions(bool showActions)
{
    if (m_showActions != showActions) {
        m_showActions = showActions;
        emit showActionsChanged();
        updateList();
    }
}

bool DeviceModel::showEvents() const
{
    return m_showEvents;
}

void DeviceModel::setShowEvents(bool showEvents)
{
    if (m_showEvents != showEvents) {
        m_showEvents = showEvents;
        emit showEventsChanged();
        updateList();
    }
}

void DeviceModel::updateList()
{
    if (!m_device) {
        beginResetModel();
        m_list.clear();
        endResetModel();
        emit countChanged();
        return;
    }
    beginResetModel();
    m_list.clear();
    if (m_showStates) {
        for (int i = 0; i < m_device->thingClass()->stateTypes()->rowCount(); i++) {
            m_list.append(m_device->thingClass()->stateTypes()->get(i)->id());
        }
    }

    if (m_showActions) {
        for (int i = 0; i < m_device->thingClass()->actionTypes()->rowCount(); i++) {
            if (!m_list.contains(m_device->thingClass()->actionTypes()->get(i)->id())) {
                m_list.append(m_device->thingClass()->actionTypes()->get(i)->id());
            }
        }
    }
    if (m_showEvents) {
        for (int i = 0; i < m_device->thingClass()->eventTypes()->rowCount(); i++) {
            if (!m_list.contains(m_device->thingClass()->eventTypes()->get(i)->id())) {
                m_list.append(m_device->thingClass()->eventTypes()->get(i)->id());
            }
        }
    }

    endResetModel();
    emit countChanged();
}
