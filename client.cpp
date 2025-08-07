#include "client.h"

Client::Client(QObject* parent) : QObject(parent), socket_(this) {
  connect(&socket_, &QLocalSocket::connected, this, [this]() {
    emit connectionChanged(true);
  });

  connect(&socket_, &QLocalSocket::disconnected, this, [this]() {
    emit connectionChanged(false);
  });

  connect(&socket_, &QLocalSocket::readyRead, this, &Client::onReadyRead);
  connect(
    &socket_, &QLocalSocket::errorOccurred, this, &Client::onErrorOccurred);
}

bool Client::isConnected() const {
  return socket_.state() == QLocalSocket::ConnectedState;
}

void Client::connectToServer(const QString& name) {
  socket_.connectToServer(name);
}

void Client::disconnectFromServer() { socket_.disconnectFromServer(); }

void Client::onReadyRead() {
  while (socket_.canReadLine()) {
    QString response = QString::fromUtf8(socket_.readLine()).trimmed();
    emit responseReceived(response);
  }
}

void Client::sendMessage(const QString& message) {
  QByteArray data = message.toUtf8() + "\n";
  socket_.write(data);
}

void Client::onErrorOccurred(QLocalSocket::LocalSocketError socketError) {
  Q_UNUSED(socketError)
  emit errorOccurred(socket_.errorString());
}
