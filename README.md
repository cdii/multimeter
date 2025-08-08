# multimeter

Что осталось доделать:

- класс Channel

```cpp
class Channel {
public:
  void StartMeasure();
  void SetRange(int range);
  void StopMeasure();
  std::string GetStatus();
  float GetResult(float& result);

private:
  friend class ChannelState;
  void ChangeState(ChannelState*);
...
```

- классы ChannelState

```cpp
class ChannelState {
public:
  virtual ~ChannelState() = default;
  virtual void StartMeasure(Channel* channel) = 0;
  virtual void SetRange(Channel* channel, int range) = 0;
  virtual void StopMeasure(Channel* channel) = 0;
  virtual std::string GetName() = 0;
  virtual float GetResult(Channel* channel) = 0;
};

class ErrorState : public ChannelState {
class IdleState : public ChannelState {
class MeasureState : public ChannelState {
class BusyState : public ChannelState {
```

- классы Command

```cpp
class Command {
public:
  virtual ~Command() = default;
  virtual std::string Execute(const auto& args, auto& channels) = 0;
  ...

class StartMeasureCommand : public Command {
class SetRangeCommand : public Command {
class StopMeasureCommand : public Command {
class GetStatusCommand : public Command {
class GetResultCommand : public Command {

// например
std::string GetResultCommand::Execute(
  ...

  try {
    ...

    float result = channels[channelId].getResult();

    return "ok, " + result
  } catch (const std::exception& e) {
    return "fail";
  }
}
```

### Предлагаемые улучшения реализации
1. Упрощение обработки запросов
   
   На текущем этапе для упрощения реализации предлагается отправлять ответ от канала непосредственно в функции Server::HandleClient сразу после получения запроса.

1. Доработка клиентской части

   Необходимо завершить разработку класса Client и доработать main.qml, добавив элементы управления для взаимодействия с сервером.

### Недостатки текущей реализации
1. Архитектурные ограничения

   Идеальным решением была бы реализация неблокирующего сервера с использованием механизмов select/epoll для эффективной обработки множества соединений.

1. Оптимизация чтения данных

   Метод Socket::ReadLine можно оптимизировать, реализовав буферизированное чтение с возможностью частичного считывания строки.

### Возможный внешний вид клиента:
![alt text](https://github.com/cdii/multimeter/blob/dev/prototype.png "Client")
