#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "hash.h"

#define HASH_TABLE_RESIZE_PROPORTION 0.7

struct hash_node * hash_node_new(char * word, void * elem) {
  struct hash_node * node = malloc(sizeof(struct hash_node));

  node->word = word;
  node->next = NULL;
  node->data = elem;

  return node;
}

struct hash_table * hash_table_new(int size) {
  struct hash_table * table = malloc(sizeof(struct hash_table));
  assert(table != NULL);

  if(!table) {
    fprintf(stderr, "no memory while allocating hash_table\n");
    return NULL;
  }

  table->size = size;
  table->population = 0;
  table->growth_proportion = 0.7;
  table->shrink_proportion = 0.25;
  table->storage = calloc(table->size, sizeof(struct hash_node *));

  if(table->storage == NULL) {
    fprintf(stderr, "no memory while allocating table->store\n");
    free(table);
    return NULL;
  }

  return table;
}

void hash_node_free(struct hash_node * node) {
  if (node) {
    free(node);
  }
}

void hash_nodes_free(struct hash_node * node) {
  if (node) {
    hash_nodes_free(node->next);
    hash_node_free(node);
  }
}

void hash_table_destroy(struct hash_table *table) {
  int i = 0;
  for(; i < table->size; i++) {
    if(table->storage[i] != NULL) {
      hash_nodes_free(table->storage[i]);
    }
  }
  free(table->storage);
  free(table);
}

void * hash_table_store(struct hash_table * table, char * word, void * elem) {
  return hash_table_store_with_resize(table, word, elem, 1);
}

void * hash_table_store_with_resize(struct hash_table * table, char * word, void * elem, int consider_resize) {
  struct hash_node * dummy = table->storage[lua_hash(word)%table->size];

  if (dummy) {
    /* if the word is in the linked list, then replace the contents */
    struct hash_node * head = dummy;
    while (head) {
      if (strcmp(head->word, word) == 0) {
        dummy = head->data;
        head->data = elem;
        return dummy;
      }
      head = head->next;
    }

    /* otherwise, this is just a collison. resolve. */
    head = dummy;
    struct hash_node * new_head = hash_node_new(word, elem);
    new_head->next = head;
    table->storage[lua_hash(word)%table->size] = new_head;
    table->population ++;
  } else {
    /* nothing is here. fill up the space */
    struct hash_node * n = hash_node_new(word, elem);
    table->storage[lua_hash(word)%table->size] = n;
    table->population ++;
  }

  if (consider_resize) {
    hash_table_resize(table);
  }

  return 0;
}

void * hash_table_delete(struct hash_table * table, char * word) {
  struct hash_node * head = table->storage[lua_hash(word) % table->size], *prev = NULL;
  void * old_data = NULL;

  while (head) {
    if (strcmp(head->word, word) == 0) {
      if (prev) {
        prev->next = head->next;
      } else {
        table->storage[lua_hash(word) % table->size] = head->next;
      }

      old_data = head->data;
      hash_node_free(head);
      table->population--;
      hash_table_resize(table);
      return old_data;
    }

    prev = head;
    head = head->next;
  }

  return NULL;
}

void hash_table_resize(struct hash_table * table) {
  int i = 0, keys_in_old_hash = table->size;
  struct hash_node ** new_storage;
  float proportion = (float)table->population/(float)table->size;

  if (proportion > table->growth_proportion) {
    new_storage = calloc(table->size * 2, sizeof(struct hash_node *));
    table->size = table->size * 2;
  } else if (proportion < table->shrink_proportion && table->size > 1) {
    new_storage = calloc(table->size / 2, sizeof(struct hash_node *));
    table->size = table->size / 2;
  } else {
    return;
  }

  if (new_storage == NULL) {
    return;
  }

  struct hash_node ** old_storage = table->storage;
  table->population = 0;
  table->storage = new_storage;

  /* rehash all elements */
  for (; i < keys_in_old_hash; i++) {
    if (old_storage[i] != NULL) {
      struct hash_node * node = old_storage[i];
      while(node) {
        hash_table_store_with_resize(table, node->word, node->data, 0);
        node = node->next;
      }
    }
  }

  free(old_storage);
}

struct hash_node * hash_table_get_hash_node(struct hash_table* table, char * word) {
  int hash = lua_hash(word);
  struct hash_node * head;

  if(table->storage[hash%table->size] == NULL) {
    return NULL;
  } else {
    head = table->storage[hash%table->size];

    if(head->next == NULL) {
      return head;
    }

    while(head != NULL) {
      if (strcmp(head->word, word) == 0) {
        return head;
      }

      head = head->next;
    }
  }
  return NULL;
}

void * hash_table_get(struct hash_table * table, char * word) {
  struct hash_node * node = hash_table_get_hash_node(table, word);
  if (node) {
    return node->data;
  } else {
    return NULL;
  }
}

char ** hash_table_get_all_keys(struct hash_table *table) {
  char ** arr = malloc(table->population * sizeof(char *));
  struct hash_node * head = NULL;

  int i = 0, k = 0;
  for(; i < table->size; i++) {
    if(table->storage[i]) {
      head = table->storage[i];

      while (head != NULL) {
        arr[k] = head->word;
        k++;
        head = head->next;
      }
    }
  }

  return arr;
}

int lua_hash(char *str) {
  int l = (int)strlen(str);
  int i, step = ((l >> 5) + 1);
  int h = l + (l >= 4?*(int*)str:0);
  for( i=l; i >= step; i -= step) {
    h = h^(( h<<5 ) + (h >> 2) + ((unsigned char *)str)[i-1]);
  }

  return abs(h);
}