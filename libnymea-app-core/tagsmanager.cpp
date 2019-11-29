#include "tagsmanager.h"
#include "types/tag.h"
#include "engine.h"

TagsManager::TagsManager(JsonRpcClient *jsonClient, QObject *parent):
    JsonHandler(parent),
    m_jsonClient(jsonClient),
    m_tags(new Tags(this))
{
    jsonClient->registerNotificationHandler(this, "handleTagsNotification");
}

QString TagsManager::nameSpace() const
{
    return "Tags";
}

void TagsManager::init()
{
    m_busy = true;
    emit busyChanged();
    m_tags->clear();
    m_jsonClient->sendCommand("Tags.GetTags", this, "getTagsReply");
}

bool TagsManager::busy() const
{
    return m_busy;
}

Tags *TagsManager::tags() const
{
    return m_tags;
}

void TagsManager::tagDevice(const QString &deviceId, const QString &tagId, const QString &value)
{
    QVariantMap params;
    QVariantMap tag;
    tag.insert("deviceId", deviceId);
    tag.insert("appId", "nymea:app");
    tag.insert("tagId", tagId);
    tag.insert("value", value);
    params.insert("tag", tag);
    m_jsonClient->sendCommand("Tags.AddTag", params, this, "addTagReply");
}

void TagsManager::untagDevice(const QString &deviceId, const QString &tagId)
{
    QVariantMap params;
    QVariantMap tag;
    tag.insert("deviceId", deviceId);
    tag.insert("appId", "nymea:app");
    tag.insert("tagId", tagId);
    params.insert("tag", tag);
    m_jsonClient->sendCommand("Tags.RemoveTag", params, this, "removeTagReply");
}

void TagsManager::tagRule(const QString &ruleId, const QString &tagId, const QString &value)
{
    QVariantMap params;
    QVariantMap tag;
    tag.insert("ruleId", ruleId);
    tag.insert("appId", "nymea:app");
    tag.insert("tagId", tagId);
    tag.insert("value", value);
    params.insert("tag", tag);
    m_jsonClient->sendCommand("Tags.AddTag", params, this, "addTagReply");
}

void TagsManager::untagRule(const QString &ruleId, const QString &tagId)
{
    QVariantMap params;
    QVariantMap tag;
    tag.insert("ruleId", ruleId);
    tag.insert("appId", "nymea:app");
    tag.insert("tagId", tagId);
    params.insert("tag", tag);
    m_jsonClient->sendCommand("Tags.RemoveTag", params, this, "removeTagReply");
}

void TagsManager::handleTagsNotification(const QVariantMap &params)
{
    qDebug() << "Have tags notification" << params;

    QVariantMap tagMap = params.value("params").toMap().value("tag").toMap();
    if (tagMap.value("appId").toString() != "nymea:app") {
        return; // not for us
    }

    QString notification = params.value("notification").toString();
    if (notification == "Tags.TagAdded") {
        Tag *tag = unpackTag(tagMap);
        if (tag) {
            m_tags->addTag(tag);
        }

    } else if (notification == "Tags.TagRemoved") {
        for (int i = 0; i < m_tags->rowCount(); i++) {
            Tag* tag = m_tags->get(i);
            if (tagMap.value("deviceId").toUuid() == tag->deviceId() &&
                    tagMap.value("ruleId").toUuid() == tag->ruleId() &&
                    tagMap.value("tagId").toString() == tag->tagId()) {
                m_tags->removeTag(tag);
                return;
            }
        }
    } else if (notification == "Tags.TagValueChanged") {
        qDebug() << "tag value changed";
        for (int i = 0; i < m_tags->rowCount(); i++) {
            Tag* tag = m_tags->get(i);
            if (tagMap.value("deviceId").toUuid() == tag->deviceId() &&
                    tagMap.value("ruleId").toUuid() == tag->ruleId() &&
                    tagMap.value("tagId").toString() == tag->tagId()) {
                qDebug() << "Found tag";
                tag->setValue(tagMap.value("value").toString());
            }
        }
    }
}

void TagsManager::getTagsReply(const QVariantMap &params)
{
    QList<Tag*> tags;
    foreach (const QVariant &tagVariant, params.value("params").toMap().value("tags").toList()) {
        qDebug() << "aDDING TAG";
        Tag *tag = unpackTag(tagVariant.toMap());
        if (tag) {
            tags.append(tag);
        }
    }
    m_tags->addTags(tags);

    m_busy = false;
    emit busyChanged();
}

void TagsManager::addTagReply(const QVariantMap &params)
{
    qDebug() << "AddTag reply" << params;
}

void TagsManager::removeTagReply(const QVariantMap &params)
{
    qDebug() << "RemoveTag reply" << params;
}

Tag* TagsManager::unpackTag(const QVariantMap &tagMap)
{
    QString deviceId = tagMap.value("deviceId").toString();
    QString ruleId = tagMap.value("ruleId").toString();
    QString tagId = tagMap.value("tagId").toString();
    QString value = tagMap.value("value").toString();
    Tag *tag = nullptr;
    if (!deviceId.isEmpty()) {
        tag = new Tag(tagId, value);
        tag->setDeviceId(deviceId);
    } else if (!ruleId.isEmpty()) {
        tag = new Tag(tagId, value);
        tag->setRuleId(ruleId);
    } else {
        qWarning() << "Invalid tag. Neither deviceId nor ruleId are set. Skipping...";
        tag->deleteLater();
        return nullptr;
    }
//    qDebug() << "adding tag" << tag->tagId() << tag->value();
    return tag;
}
