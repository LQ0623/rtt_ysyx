#include <am.h>
#include <klib.h>
#include <rtthread.h>

#define STACK_SIZE  1024*16

static Context* ev_handler(Event e, Context *c) {
  switch (e.event) {
    case EVENT_YIELD:
      // rt_thread_t self  = rt_thread_self();
      // rt_ubase_t *args  = (void*)self->user_data;
      // Context** from    = (Context**)args[0];
      // Context** to      = (Context**)args[1];
      // if (from!=NULL)
      //   *from = c;
      // c = *to;
      rt_thread_t self  = rt_thread_self();
      rt_ubase_t *para  = (rt_ubase_t *)self->user_data;
      rt_ubase_t from   = para[0];
      rt_ubase_t to     = para[1];
      if(from){
        *((Context **)from) = c;
        c = *((Context **)to);
      }
      else{
        c = *((Context **)to);
      }
      break;
    case EVENT_IRQ_TIMER:
      // printf("event irq_timer\n");
      break;
    default: printf("Unhandled event ID = %d\n", e.event); assert(0);
  }
  return c;
}

void __am_cte_init() {
  cte_init(ev_handler);
}

void rt_hw_context_switch_to(rt_ubase_t to) {
  // assert(0);
  rt_ubase_t temp_user_data;
  rt_ubase_t user_data[2] = {0, to};

  rt_thread_t current     = rt_thread_self();
  temp_user_data          = current->user_data;

  current->user_data      = (rt_ubase_t)user_data;

  yield();

  current->user_data      = temp_user_data;
}

void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to) {
  // assert(0);
  rt_ubase_t temp_user_data;
  rt_ubase_t user_data[2] = {from, to};
  rt_thread_t current     = rt_thread_self();

  temp_user_data          = current->user_data;
  current->user_data      = (rt_ubase_t)user_data;

  yield();

  current->user_data      = temp_user_data;
}

void rt_hw_context_switch_interrupt(void *context, rt_ubase_t from, rt_ubase_t to, struct rt_thread *to_thread) {
  assert(0);
}

static void tentry_entry(void *para) {
  uintptr_t *args = para;
  void (*tentry)() = (void*)args[1];
  tentry(args[0]);
  void (*texit)() = (void*)args[2];
  texit(); 
}

rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit) {
  // assert(0);
  stack_addr = (rt_uint8_t *)(((uintptr_t)stack_addr + sizeof(uintptr_t) - 1) & ~(sizeof(uintptr_t) - 1));
  uintptr_t *args = (void*)(stack_addr-512);
  stack_addr      = stack_addr - 512;
  args[0]         = (uintptr_t)parameter;
  args[1]         =(uintptr_t)tentry;
  args[2]         =(uintptr_t)texit;
  Context* cp     = kcontext((Area){(void*)(stack_addr - STACK_SIZE), (void*)stack_addr}, tentry_entry, args);
  return (rt_uint8_t*)cp;
}
