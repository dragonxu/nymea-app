#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QAbstractListModel>
#include <QUuid>

#include "engine.h"

class DeviceDescriptor: public QObject {
    Q_OBJECT
    Q_PROPERTY(QUuid id READ id CONSTANT)
    Q_PROPERTY(QUuid deviceId READ deviceId CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(Params* params READ params CONSTANT)
public:
    DeviceDescriptor(const QUuid &id, const QUuid &deviceId, const QString &name, const QString &description, QObject *parent = nullptr);

    QUuid id() const;
    QUuid deviceId() const;
    QString name() const;
    QString description() const;
    Params* params() const;

private:
    QUuid m_id;
    QUuid m_deviceId;
    QString m_name;
    QString m_description;
    Params *m_params = nullptr;
};

class DeviceDiscovery : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Engine* engine READ engine WRITE setEngine)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString displayMessage READ displayMessage NOTIFY busyChanged)
public:
    enum Roles {
        RoleId,
        RoleDeviceId,
        RoleName,
        RoleDescription
    };

    DeviceDiscovery(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;


    Q_INVOKABLE void discoverDevices(const QUuid &deviceClassId, const QVariantList &discoveryParams = {});

    Q_INVOKABLE DeviceDescriptor* get(int index) const;

    Engine* engine() const;
    void setEngine(Engine *jsonRpcClient);

    bool busy() const;
    QString displayMessage() const;

private slots:
    void discoverDevicesResponse(const QVariantMap &params);

signals:
    void busyChanged();
    void countChanged();
    void engineChanged();

private:
    Engine *m_engine = nullptr;
    bool m_busy = false;
    QString m_displayMessage;

    bool contains(const QUuid &deviceDescriptorId) const;
    QList<DeviceDescriptor*> m_foundDevices;
};

class DeviceDiscoveryProxy: public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(DeviceDiscovery* deviceDiscovery READ deviceDiscovery WRITE setDeviceDiscovery NOTIFY deviceDiscoveryChanged)
    Q_PROPERTY(bool showAlreadyAdded READ showAlreadyAdded WRITE setShowAlreadyAdded NOTIFY showAlreadyAddedChanged)
    Q_PROPERTY(bool showNew READ showNew WRITE setShowNew NOTIFY showNewChanged)
    Q_PROPERTY(QUuid filterDeviceId READ filterDeviceId WRITE setFilterDeviceId NOTIFY filterDeviceIdChanged)

public:
    DeviceDiscoveryProxy(QObject *parent = nullptr);

    DeviceDiscovery* deviceDiscovery() const;
    void setDeviceDiscovery(DeviceDiscovery* deviceDiscovery);

    bool showAlreadyAdded() const;
    void setShowAlreadyAdded(bool showAlreadyAdded);

    bool showNew() const;
    void setShowNew(bool showNew);

    QUuid filterDeviceId() const;
    void setFilterDeviceId(const QUuid &filterDeviceId);

    Q_INVOKABLE DeviceDescriptor* get(int index) const;

signals:
    void countChanged();
    void deviceDiscoveryChanged();
    void showAlreadyAddedChanged();
    void showNewChanged();
    void filterDeviceIdChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    DeviceDiscovery* m_deviceDiscovery = nullptr;
    bool m_showAlreadyAdded = false;
    bool m_showNew = true;
    QUuid m_filterDeviceId;
};

#endif // DEVICEDISCOVERY_H
