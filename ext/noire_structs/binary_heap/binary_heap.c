/*
 * Binary Heap implementation.
 *  Author: Noire.
 */

#include "binary_heap.h"

 /*----Constants and Macros---------------------------------------------------*/

#define LEFT_CHILD(index) (2*index+1)
#define RIGHT_CHILD(index) (2*index+2)
#define PARENT(index) (index/2)

/*----Structures and Variables------------------------------------------------*/

typedef struct bheap{
  VALUE * heap;
  uint64_t size;
  uint64_t array_size;
  double realloc_factor;
}bheap;

typedef bheap * t_bheap;

VALUE rb_cBinaryHeap;

/*----Function Declaration----------------------------------------------------*/

void
m_free(t_bheap data);

void
m_mark(t_bheap data);

VALUE
m_alloc(VALUE self);

VALUE
m_initialize(int argc, VALUE * argv, VALUE self);

void
swap(VALUE * array, uint64_t index1, uint64_t index2);

VALUE
m_offer(VALUE self, VALUE key);

VALUE
m_size(VALUE self);

VALUE
m_peek(VALUE self);

VALUE
m_is_empty(VALUE self);

VALUE
m_poll(VALUE self);

VALUE
m_contains(VALUE self, VALUE key);

boolean
aux_contains(t_bheap data, uint64_t index, VALUE key);

VALUE
m_remove(VALUE self, VALUE key);

boolean
aux_remove(t_bheap data, uint64_t index, VALUE key);

void
down_heap(t_bheap data, uint64_t index);

void
Init_binary_heap(void);

/*----Function Implementation-------------------------------------------------*/

void
m_free(t_bheap data){
  free(data->heap);
  free(data);
}

void
m_mark(t_bheap data){
  uint64_t i;
  for (i = 0; i < data->size; i++){
    rb_gc_mark(data->heap[i]);
  }
}

VALUE
m_alloc(VALUE self){
  t_bheap data = malloc(sizeof(bheap));
  if (data == NULL){
    rb_raise(rb_eRuntimeError, "The computer ran out of memory.");
  }
  return Data_Wrap_Struct(self, m_mark, m_free, data);
}

VALUE
m_initialize(int argc, VALUE * argv, VALUE self){
  t_bheap data;
  VALUE init_size, realloc_factor;
  uint64_t init;
  double rf;

  rb_scan_args(argc, argv, "02", &init_size, &realloc_factor);

  if (NIL_P(init_size)){
    init = 20;
  }else{
    init = NUM2ULONG(init_size);
  }

  if (NIL_P(realloc_factor)){
    rf = 1.75;
  }else{
    rf = DBL2NUM(realloc_factor);
  }

  if ( rf <= 1.0 )
    rb_raise(rb_eRuntimeError, "The realloc factor must be a number greater than 1.0.");

  if ( init <= 0 )
    rb_raise(rb_eRuntimeError, "The initial size must be a positive integer.");

	Data_Get_Struct(self, bheap, data);

  data->heap = malloc(sizeof(VALUE)*init);
  if (data->heap == NULL ){
    rb_raise(rb_eRuntimeError, "The computer ran out of memory.");
  }
  data->array_size = init;
  data->size = 0;
  data->realloc_factor = rf;
	return self;
}

void
swap(VALUE * array, uint64_t index1, uint64_t index2){
  VALUE aux;
  aux = array[index1];
  array[index1] = array[index2];
  array[index2] = aux;
  return;
}

VALUE
m_offer(VALUE self, VALUE key){
  t_bheap data;
  VALUE * aux_data;
  uint64_t index, aux;
  boolean flag = true;

  Data_Get_Struct(self, bheap, data);

  if(data->size == data->array_size){
    data->array_size = data->array_size * data->realloc_factor;
    aux_data = realloc(data->heap, data->array_size * sizeof(VALUE));
    if (aux_data == NULL){
      rb_raise(rb_eRuntimeError, "The computer ran out of memory.");
    }else{
      data->heap = aux_data;
    }
  }

  index = data->size;
  data->heap[index] = key;

  while( index > 0 && flag ){
    aux = PARENT(index);
    if ( rb_funcall(data->heap[index], rb_intern("<"), 1, data->heap[aux]) ){
      swap(data->heap,index,aux);
      index = aux;
    }else{
      flag = false;
    }
  }
  data->size++;
  return Qnil;
}

VALUE
m_size(VALUE self){
  t_bheap data;
  Data_Get_Struct(self, bheap, data);

  return ULONG2NUM(data->size);
}

VALUE
m_peek(VALUE self){
  t_bheap data;
  Data_Get_Struct(self, bheap, data);

  if (data->size == 0){
    rb_raise(rb_eRuntimeError, "The BinaryHeap is empty.");
  }

  return data->heap[0];
}

VALUE
m_is_empty(VALUE self){
  t_bheap data;
  Data_Get_Struct(self, bheap, data);

  if (data->size == 0){
    return Qtrue;
  }else{
    return Qfalse;
  }
}

VALUE
m_poll(VALUE self){
  t_bheap data;
  VALUE ret;
  Data_Get_Struct(self, bheap, data);

  if (data->size == 0){
    rb_raise(rb_eRuntimeError, "The Binary Heap is empty.");
  }

  ret = data->heap[0];
  down_heap(data,0);

  return ret;
}

VALUE
m_contains(VALUE self, VALUE key){
  t_bheap data;
  boolean ret;

  Data_Get_Struct(self, bheap, data);

  ret = aux_contains(data,0,key);
  if (ret){
    return Qtrue;
  }else{
    return Qfalse;
  }

}

boolean
aux_contains(t_bheap data, uint64_t index, VALUE key){
  if( index >= data->size ||
    rb_funcall(key, rb_intern("<"), 1, data->heap[index])){
    return false;
  }else if ( rb_funcall(key, rb_intern("=="), 1, data->heap[index]) ){
    return true;
  }else{
    return aux_contains(data,LEFT_CHILD(index),key) ||
      aux_contains(data,RIGHT_CHILD(index),key);
  }
}

VALUE
m_remove(VALUE self, VALUE key){
  t_bheap data;
  boolean ret;

  Data_Get_Struct(self, bheap, data);

  ret = aux_remove(data,0,key);

  if (ret){
    return Qtrue;
  }else{
    return Qfalse;
  }

}

boolean
aux_remove(t_bheap data, uint64_t index, VALUE key){
  if( index >= data->size ||
    rb_funcall(key, rb_intern("<"), 1, data->heap[index])){
    return false;
  }else if ( rb_funcall(key, rb_intern("=="), 1, data->heap[index]) ){
    down_heap(data,index);
    return true;
  }else{
    return aux_remove(data,LEFT_CHILD(index),key) ||
      aux_remove(data,RIGHT_CHILD(index),key);
  }
}

void
down_heap(t_bheap data, uint64_t index){
  boolean flag = true;
  uint64_t left_index, right_index, max;

  (data->size)--;
  data->heap[index] = data->heap[data->size];

  while ( flag ){
    left_index = LEFT_CHILD(index);
    right_index = RIGHT_CHILD(index);
    max = index;

    if ( left_index < data->size &&
      rb_funcall(data->heap[max], rb_intern(">"), 1, data->heap[left_index])){
        max = left_index;
    }

    if( right_index < data->size &&
      rb_funcall(data->heap[max], rb_intern(">"), 1,data->heap[right_index])){
        max = right_index;
    }

    if( max == index ){
      flag = false;
    }else{
      swap(data->heap,index,max);
      index = max;
    }
  }
}

void
Init_binary_heap(void){
  VALUE rb_cBinaryHeap = rb_define_class("BinaryHeap", rb_cObject);
  rb_define_alloc_func(rb_cBinaryHeap, m_alloc);
  rb_define_method(rb_cBinaryHeap, "initialize", m_initialize, -1);

  rb_define_method(rb_cBinaryHeap, "offer", m_offer, 1);
  rb_define_method(rb_cBinaryHeap, "add", m_offer, 1);

  rb_define_method(rb_cBinaryHeap, "size", m_size, 0);

  rb_define_method(rb_cBinaryHeap, "peek", m_peek, 0);
  rb_define_method(rb_cBinaryHeap, "is_empty?", m_is_empty, 0);
  rb_define_method(rb_cBinaryHeap, "poll", m_poll, 0);

  rb_define_method(rb_cBinaryHeap, "contains?", m_contains, 1);
  rb_define_method(rb_cBinaryHeap, "remove", m_remove, 1);
}
