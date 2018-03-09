#include "ruleaction.h"

#include "ruleactionparam.h"
#include "ruleactionparams.h"

RuleAction::RuleAction(QObject *parent) : QObject(parent)
{
    m_ruleActionParams = new RuleActionParams(this);
}

QUuid RuleAction::deviceId() const
{
    return m_deviceId;
}

void RuleAction::setDeviceId(const QUuid &deviceId)
{
    if (m_deviceId != deviceId) {
        m_deviceId = deviceId;
        emit deviceIdChanged();
    }
}

QUuid RuleAction::actionTypeId() const
{
    return m_actionTypeId;
}

void RuleAction::setActionTypeId(const QUuid &actionTypeId)
{
    if (m_actionTypeId != actionTypeId) {
        m_actionTypeId = actionTypeId;
        emit actionTypeIdChanged();
    }
}

RuleActionParams *RuleAction::ruleActionParams() const
{
    return m_ruleActionParams;
}

RuleAction *RuleAction::clone() const
{
    RuleAction *ret = new RuleAction();
    ret->setActionTypeId(actionTypeId());
    ret->setDeviceId(deviceId());
    for (int i = 0; i < ruleActionParams()->rowCount(); i++) {
        ret->ruleActionParams()->addRuleActionParam(ruleActionParams()->get(i)->clone());
    }
    return ret;
}