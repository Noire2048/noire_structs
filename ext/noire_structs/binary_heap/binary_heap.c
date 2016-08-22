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
  unsigned long size;
  unsigned long array_size;
  double realloc_factor;
}bheap;

typedef bheap * t_bheap;

VALUE rb_cBinaryHeap;

/*----Function Declaration----------------------------------------------------*/

void
m_free(t_bheap data);

void
m_mark(t_bheap data);

void
m_alloc(VALUE self);

VALUE
m_initialize(int argc, VALUE * argv, VALUE self);

void
swap(VALUE * array, unsigned long index1, unsigned long index2);

VALUE
m_insert(VALUE self, VALUE key);

VALUE
m_size(VALUE self);

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
  unsigned long i = 0;
  for (i; i < data->size; i++){
    rb_gc_mark(data->heap[i]);
  }
}

void
m_alloc(VALUE self){
  t_bheap data = malloc(sizeof(bheap));
  return Data_Wrap_Struct(self, m_mark, m_free, data);
}

VALUE
m_initialize(int argc, VALUE * argv, VALUE self){
  t_bheap data;
  VALUE init_size, realloc_factor;
  unsigned long init;
  double rf;

  rb_scan_args(argc, argv, "02", &init_size, &realloc_factor);

  if (NIL_P(init_size)){
    init = 20;
  }else{
    init = NUM2ULONG(init_size);
  }

  if (NIL_P(realloc_factor)){
    rf = 1.75
  }else{
    rf = DBL2NUM(realloc_factor);
  }

  if ( rf <= 1.0 )
    rb_raise(rb_eRuntimeError, "The realloc factor must be a number greater
     than 1.0.");

  if ( init <= 0 )
    rb_raise(rb_eRuntimeError, "The initial size must be a positive integer.");

	Data_Get_Struct(self, bheap, data);

  data->heap = malloc(sizeof(VALUE)*init);
  data->array_size = init;
  data->size = 0;
  data->realloc_factor = rf;
	return self;
}

void
swap(VALUE * array, unsigned long index1, unsigned long index2){
  VALUE aux;
  aux = array[index1];
  array[index1] = array[index2];
  array[index2] = aux;
  return;
}

VALUE
m_insert(VALUE self, VALUE key){
  t_bheap * data;
  unsigned long index, aux;
  boolean flag = true;

  Data_Get_Struct(self, b_heap, data);

  if(data->size == data->array_size){
    data->array_size = data->array_size * data->realloc_factor;
    data->heap = realloc(data->heap, data->array_size * sizeof(VALUE));
  }

  index = data->size;
  data->heap[index] = key;

  while( index != 0 || flag ){
    aux = PARENT(index);
    if ( rb_funcall(data->heap[index], rb_intern("<"), 1, data->heap[aux]) ){
      swap(data->heap,index,aux);
      index = aux;
    }else{
      flag = false;
    }
  }
  data->size++;
}

VALUE
m_size(VALUE self){
  t_bheap data;
  Data_Get_Struct(self, b_heap, data);

  return ULONG2NUM(data->size);
}

void
Init_binary_heap(void){
  VALUE rb_cBinaryHeap = rb_define_class("BinaryHeap", rb_cObject);
  rb_define_alloc_func(rb_cBinaryHeap, m_alloc);
  rb_define_method(rb_cBinaryHeap, "initialize", m_initialize, -1);
  rb_define_method(rb_cBinaryHeap, "insert", m_insert, 1);
  rb_define_method(rb_cBinaryHeap, "size", m_size, 0);
}
