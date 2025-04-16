# Integral

## Задание

Требуется параллельно вычислить интеграл от функции $f:\ \mathbb{R}\rightarrow\mathbb{R}$ на отрезке с некоторой точностью $\varepsilon=10^{-7}$.

## Реализация

Вычисление интегралов будет производиться посредством взаимодействия с объектом типа `par_integrator_t` с использованием следующего API:

- Инициализация объекта, фиксирование числа потоков `threads_num`, которые будут использованы для вычисления:
```c++
int par_integrator_init(par_integrator_t* self, size_t threads_num)
```

- Запуск процесса вычисления интеграла $\displaystyle\int_{\mathrm{left\_bound}}^{\mathrm{right\_bound}}\mathrm{func}(x) dx$. Данная операция НЕ должна блокировать поток исполнения в ожидании окончания вычислений. 
```c++
int par_integrator_start_calc(par_integrator_t* self, func_t* func,
                              field_t left_bound, field_t right_bound)
```

- Получение готового результата вычисления интеграла, запущеннго функцией выше, или его ожидание
```c++
int par_integrator_get_result(par_integrator_t* self, field_t* result)
```

- Декструктор объекта
```c++
int par_integrator_destroy(par_integrator_t* self)
```

## Требования к реализации:
* Запрещается создавать и использовать число потоков, большее значения `threads_num`, указанного в конструкторе
* Любое ожидание в потоках не должно нагружать CPU
* Функции `par_integrator_start_calc` и `par_integrator_get_result` могут вызываться повторно (с сохранением взаимного порядка) для вычисления других интегралов на выделенных мощностях данного объекта `par_integrator_t`
* Может быть создано несколько объектов `par_integrator_t`, которые могут выполнять вычисления как последовательно, так и параллельно.
* Из средств синхронизации разрешено использовать только атомики, любые операции с ними, а также операции [`wait`](https://en.cppreference.com/w/cpp/atomic/atomic/wait), [`notify_one`](https://en.cppreference.com/w/cpp/atomic/atomic/notify_one) и [`notify_all`](https://en.cppreference.com/w/cpp/atomic/atomic/notify_all), реализованные в `wait.h`.

Все функции должны возвращать 0 в случае успеха и число, отличное от нуля иначе.
  
Разместите ваше решение в `integral.h`.
