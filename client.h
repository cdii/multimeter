#pragma once

#include <QLocalSocket>
#include <QObject>

class Client : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool connected READ isConnected NOTIFY connectionChanged)

public:
  explicit Client(QObject* parent = nullptr);

  bool isConnected() const;

  Q_INVOKABLE void connectToServer(const QString& name);
  Q_INVOKABLE void disconnectFromServer();
  Q_INVOKABLE void sendMessage(const QString& message);

signals:
  void connectionChanged(bool connected);
  void errorOccurred(const QString& error);
  void responseReceived(const QString& response);

private slots:
  void onErrorOccurred(QLocalSocket::LocalSocketError socketError);
  void onReadyRead();

private:
  QLocalSocket socket_;
};
