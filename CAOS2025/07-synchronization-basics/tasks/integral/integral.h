#pragma once

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wait.h"

typedef double field_t;

typedef field_t func_t(field_t);

typedef struct pthread_info pthread_info_t;

typedef struct par_integrator {
  field_t left_bound;
  field_t right_bound;
  func_t* function;
  _Atomic uint32_t threads_num;
  _Atomic uint32_t last_notified_thread;
  pthread_t* threads;
  pthread_info_t* thread_info_storage;
} par_integrator_t;

#define STATE_READY 0
#define STATE_RUNNING 1
#define STATE_FINISHED 2
const field_t cPrec = 1e-7;

typedef struct pthread_info {
  _Atomic uint32_t state;
  _Atomic uint32_t thread_index;
  _Atomic field_t result;
  par_integrator_t* current_integrator;
} pthread_info_t;

void* thread_func(void* arg) {
  pthread_info_t* info = (pthread_info_t*)arg;
  par_integrator_t* integrator = info->current_integrator;
  while (true) {
    atomic_wait(&info->state, STATE_READY);
    if (info->state == STATE_RUNNING) {
      field_t left_bound = integrator->left_bound;
      field_t right_bound = integrator->right_bound;
      func_t* func = integrator->function;
      size_t threads_num = integrator->threads_num;
      size_t thread_index = info->thread_index;

      field_t step = (right_bound - left_bound) / threads_num;
      field_t smol_step = step / (field_t)(7000);
      field_t a = left_bound + step * thread_index;
      field_t b = a + smol_step;
      field_t rbound = a + step;
      field_t local_result = 0;
      bool flag = true;
      for (; flag && b < rbound;) {
        if (b + smol_step > rbound) {
          b = rbound;
          flag = false;
        }
        field_t mid = (a + b) / 2;
        field_t fa = func(a);
        field_t fm = func(mid);
        field_t fb = func(b);
        local_result += (fa + 4.0 * fm + fb) * (b - a) / 6.0;
        a += smol_step;
        b += smol_step;
      }
      info->result = local_result;
      info->current_integrator->last_notified_thread += 1;
      info->state = STATE_READY;
      atomic_notify_one(&integrator->last_notified_thread);
    } else {
      break;
    }
  }

  return NULL;
}

int par_integrator_init(par_integrator_t* self, size_t threads_num) {
  // что мы тут делаем - инициализируем себя, и открываем все потоки, ставя в
  // статус что те готовы к запуску, кроме того инициализируем thread_info для
  // них, функция thread_create, в которую закидываем thread_routine
  if (self == NULL) {
    return -1;
  }
  if (threads_num > 4) {
    threads_num = 4;
  }
  self->last_notified_thread = self->threads_num = threads_num;
  self->threads = (pthread_t*)malloc(sizeof(pthread_t) * threads_num);
  if (self->threads == NULL) {
    return -1;
  }
  self->thread_info_storage =
      (pthread_info_t*)malloc(sizeof(pthread_info_t) * threads_num);
  if (self->thread_info_storage == NULL) {
    return -1;
  }
  for (uint32_t i = 0; i < threads_num; ++i) {
    self->thread_info_storage[i].state = STATE_READY;
    self->thread_info_storage[i].thread_index = i;
    self->thread_info_storage[i].result = 0.0;
    self->thread_info_storage[i].current_integrator = self;
    int ret = pthread_create(&self->threads[i], NULL, thread_func,
                             &self->thread_info_storage[i]);
    if (ret != 0) {
      for (uint32_t j = 0; j < i; ++j) {
        pthread_cancel(self->threads[j]);
        pthread_join(self->threads[j], NULL);
      }
      free(self->threads);
      free(self->thread_info_storage);
      return -1;
    }
  }
  return 0;
}

int par_integrator_start_calc(par_integrator_t* self, func_t* func,
                              field_t left_bound, field_t right_bound) {
  // проходимся по всем запущенным потокам, ставим им статус - running,
  // atomic_notify_one
  if (self->threads_num != self->last_notified_thread || self == NULL ||
      func == NULL) {
    return -1;
  }
  self->function = func;
  self->left_bound = left_bound;
  self->right_bound = right_bound;
  self->last_notified_thread = 0;
  for (uint32_t i = 0; i < self->threads_num; ++i) {
    self->thread_info_storage[i].state = STATE_RUNNING;
    atomic_notify_one(&self->thread_info_storage[i].state);
  }
  return 0;
}

int par_integrator_get_result(par_integrator_t* self, field_t* result) {
  if (result == NULL || self == NULL) {
    return -1;
  }
  *result = 0.0;
  for (uint32_t i; (i = self->last_notified_thread) < self->threads_num;
       atomic_wait(&self->last_notified_thread, i)) {
  }
  for (uint32_t i = 0; i < self->threads_num; ++i) {
    *result += self->thread_info_storage[i].result;
  }
  return 0;
}

int par_integrator_destroy(par_integrator_t* self) {
  if (self == NULL) return -1;
  for (uint32_t j = 0; j < self->threads_num; ++j) {
    self->thread_info_storage[j].state = STATE_FINISHED;
    atomic_notify_one(&self->thread_info_storage[j].state);
    pthread_join(self->threads[j], NULL);
  }
  free(self->threads);
  free(self->thread_info_storage);
  return 0;
}
