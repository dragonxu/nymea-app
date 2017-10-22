/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2017 Simon Stuerz <simon.stuerz@guh.io>                  *
 *                                                                         *
 *  This file is part of guh-control.                                      *
 *                                                                         *
 *  guh-control is free software: you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 3 of the License.                *
 *                                                                         *
 *  guh-control is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with guh-control. If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "websocketinterface.h"
#include "engine.h"

#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSettings>

WebsocketInterface::WebsocketInterface(QObject *parent) :
    GuhInterface(parent)
{
    m_socket = new QWebSocket(QGuiApplication::applicationName(), QWebSocketProtocol::Version13, this);

    QObject::connect(m_socket, &QWebSocket::connected, this, &WebsocketInterface::connected);
    QObject::connect(m_socket, &QWebSocket::disconnected, this, &WebsocketInterface::disconnected);
    typedef void (QWebSocket:: *errorSignal)(QAbstractSocket::SocketError);
    QObject::connect(m_socket, static_cast<errorSignal>(&QWebSocket::error), this, &WebsocketInterface::error);
    QObject::connect(m_socket, &QWebSocket::textMessageReceived, this, &WebsocketInterface::onTextMessageReceived);

    typedef void (QWebSocket:: *sslErrorsSignal)(const QList<QSslError> &);
    QObject::connect(m_socket, static_cast<sslErrorsSignal>(&QWebSocket::sslErrors),this, &WebsocketInterface::sslErrors);
}

QStringList WebsocketInterface::supportedSchemes() const
{
    return {"ws", "wss"};
}

void WebsocketInterface::connect(const QUrl &url)
{
    m_socket->open(QUrl(url));
}

bool WebsocketInterface::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void WebsocketInterface::disconnect()
{
    m_socket->close();
}

void WebsocketInterface::sendData(const QByteArray &data)
{
    m_socket->sendTextMessage(QString::fromUtf8(data));
}

void WebsocketInterface::ignoreSslErrors(const QList<QSslError> &errors)
{
    m_socket->ignoreSslErrors(errors);
}

void WebsocketInterface::onTextMessageReceived(const QString &data)
{
    emit dataReady(data.toUtf8());
}
