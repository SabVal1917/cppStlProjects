# Lockfree Stack

## Задание
Требуется реализовать структуру данных стек, обладающую свойством [lockfree](https://en.wikipedia.org/wiki/Non-blocking_algorithm#Lock-freedom). Взаимодействие с ним будет осуществляться в режиме MPSC (multi-producer-single-consumer): конкурентно могут выполняться произвольное число операций push и не более одной операции pop.

_Факультативно можно поддержать MPMC (multi-producer-multi-consumer), используя [differential reference counting](https://www.1024cores.net/home/lock-free-algorithms/object-life-time-management/differential-reference-counting) для управления памятью / предотвращения ABA-problem._

## Реализация

Тип элементов стека - `uintptr_t`, что следует понимать как возможность хранить в нём как целые числа, так и указатели на произвольные объекты.

Требуется реализовать функции:

- конструктор стека, вызывается для инициализации пустого объекта `lfstack_t`:
```c++
int lfstack_init(lfstack_t *stack)
```
- операция вставки `value` в стек:
```c++
int lfstack_push(lfstack_t *stack, uintptr_t value)
```
- операция извлечения элемента из стека, полученное значение сохраняется в `*value`
    - извлечение элемента из пустого стека должно корректно обрабатываться (с кодом `0`) и возвращать `value = 0`
```c++
int lfstack_pop(lfstack_t *stack, uintptr_t *value)
```
- деструктор пустого стека
```c++
int lfstack_destroy(lfstack_t *stack)
```

Все функции должны возвращать 0 в случае успеха и число, отличное от нуля иначе.
  
Разместите ваше решение в `lockfree-stack.h`.
