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

#include "packages.h"
#include "package.h"

Packages::Packages(QObject *parent) : QAbstractListModel(parent)
{

}

int Packages::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.count();
}

QVariant Packages::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case RoleId:
        return m_list.at(index.row())->id();
    case RoleDisplayName:
        return m_list.at(index.row())->displayName();
    case RoleSummary:
        return m_list.at(index.row())->summary();
    case RoleInstalledVersion:
        return m_list.at(index.row())->installedVersion();
    case RoleCandidateVersion:
        return m_list.at(index.row())->candidateVersion();
    case RoleChangelog:
        return m_list.at(index.row())->changelog();
    case RoleUpdateAvailable:
        return m_list.at(index.row())->updateAvailable();
    case RoleRollbackAvailable:
        return m_list.at(index.row())->rollbackAvailable();
    }
    return QVariant();
}

QHash<int, QByteArray> Packages::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(RoleId, "id");
    roles.insert(RoleDisplayName, "displayName");
    roles.insert(RoleSummary, "summary");
    roles.insert(RoleInstalledVersion, "installedVersion");
    roles.insert(RoleCandidateVersion, "candidateVersion");
    roles.insert(RoleChangelog, "changelog");
    roles.insert(RoleUpdateAvailable, "updateAvailable");
    roles.insert(RoleRollbackAvailable, "rollbackAvailable");
    return roles;
}

void Packages::addPackage(Package *package)
{
    package->setParent(this);
    beginInsertRows(QModelIndex(), m_list.count(), m_list.count());
    m_list.append(package);
    connect(package, &Package::summaryChanged, this, [this, package](){
        emit dataChanged(index(m_list.indexOf(package)), index(m_list.indexOf(package)), {RoleSummary});
        emit countChanged();
    });
    connect(package, &Package::installedVersionChanged, this, [this, package](){
        emit dataChanged(index(m_list.indexOf(package)), index(m_list.indexOf(package)), {RoleInstalledVersion});
        emit countChanged();
    });
    connect(package, &Package::candidateVersionChanged, this, [this, package](){
        emit dataChanged(index(m_list.indexOf(package)), index(m_list.indexOf(package)), {RoleCandidateVersion});
        emit countChanged();
    });
    connect(package, &Package::changelogChanged, this, [this, package](){
        emit dataChanged(index(m_list.indexOf(package)), index(m_list.indexOf(package)), {RoleChangelog});
        emit countChanged();
    });
    connect(package, &Package::updateAvailableChanged, this, [this, package](){
        emit dataChanged(index(m_list.indexOf(package)), index(m_list.indexOf(package)), {RoleUpdateAvailable});
        emit countChanged();
    });
    connect(package, &Package::rollbackAvailableChanged, this, [this, package](){
        emit dataChanged(index(m_list.indexOf(package)), index(m_list.indexOf(package)), {RoleRollbackAvailable});
        emit countChanged();
    });
    endInsertRows();
    emit countChanged();
}

void Packages::removePackage(const QString &packageId)
{
    int idx = -1;
    for (int i = 0; i < m_list.count(); i++) {
        if (m_list.at(i)->id() == packageId) {
            idx = i;
            break;
        }
    }
    if (idx < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    m_list.takeAt(idx)->deleteLater();
    endRemoveRows();
    emit countChanged();
}

Package *Packages::get(int index) const
{
    if (index >= 0 && index < m_list.count()) {
        return m_list.at(index);
    }
    return nullptr;
}

Package *Packages::getPackage(const QString &packageId)
{
    foreach (Package *p, m_list) {
        if (p->id() == packageId) {
            return p;
        }
    }
    return nullptr;
}

void Packages::clear()
{
    beginResetModel();
    qDeleteAll(m_list);
    m_list.clear();
    endResetModel();
    emit countChanged();
}
