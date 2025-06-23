#include "rbtree.h"

#include <stdlib.h>

typedef enum { LEFT, RIGHT } mode_n;

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
  p->nil->parent = p->nil;
  p->root = p->nil;
  return p;
}

void delete_rbtree(rbtree *t) {
  delete_node_t(t->root, t);
  free(t->nil);
  free(t);
}

void delete_node_t(node_t *node, rbtree *t) { // void 함수는 return을 안써도 되나?
  if (node == t->nil) {
    return;
  }

  delete_node_t(node->left, t->nil);
  delete_node_t(node->right, t->nil);
  free(node);

};


node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *newnode = (node_t *)calloc(1, sizeof(node_t));
  newnode->color = RBTREE_RED;
  newnode->key = key;
  newnode->left = t->nil;
  newnode->right = t->nil;
  newnode->parent = t->nil;

  bst_insert(&(t->root), newnode, t);


  rb_insert_fixup(newnode, t);

  return newnode;
}

void bst_insert(node_t **treenode, node_t *newnode, rbtree *t) {
  if ((*treenode) == t->nil) { // root가 비어있었을 경우
    (*treenode) = newnode;
    newnode->color = RBTREE_BLACK;
    return;
  }
  if (newnode->key < (*treenode)->key){
    if ((*treenode)->left == t->nil){
      (*treenode)->left = newnode;
      newnode->parent = (*treenode);
      return;
    } else {
      bst_insert(&((*treenode)->left), newnode, t);
    }
  } else {
    if ((*treenode)->right == t->nil) {
      (*treenode)->right = newnode;
      newnode->parent = (*treenode);
      return;
    } else {
      bst_insert(&((*treenode)->right), newnode, t);
    }
  }
  return;
}

void rb_insert_fixup(node_t *newnode, rbtree *t) {
  node_t *parent = newnode->parent; // 미리 parent 주소 복사 -> rotate 후에도 안바뀜
  node_t *grandparent = newnode->parent->parent; // 미리 grandparent 주소 복사 -> rotate 후에도 안바뀜
  node_t *uncle = t->nil; // 일단 uncle은 nil로 초기화
  int parent_is;

  if (newnode == t->root) {
    if (newnode->color == RBTREE_RED){
      newnode->color = RBTREE_BLACK;
    }
    return;
  }

  if (grandparent->left == newnode->parent) {
    uncle = grandparent->right;
    parent_is = LEFT;
  } else if (grandparent->right == newnode->parent) {
    uncle = grandparent->left;
    parent_is = RIGHT;
  } else { // 이 경우는 grandparent가 nil인 경우 뿐 -> root 바로 밑이므로 즉시 종료
      return;
  }
  
  // 여기부터 수정 시작
  if (parent->color == RBTREE_RED) {
    if (uncle->color == RBTREE_RED) { // case 1
      parent->color = RBTREE_BLACK;
      uncle->color = RBTREE_BLACK;
      grandparent->color = RBTREE_RED;
      rb_insert_fixup(grandparent, t);
      return;
    } 
    else { // case 2 or 3
      switch (parent_is)
      {
        case LEFT: // parent가 grandparent의 왼쪽 자식
          if (parent->left == newnode) // case 3
          {
            rotate(grandparent, t, RIGHT);
            if (grandparent == t->root) { 
              t->root = parent;
            }
            grandparent->color = RBTREE_RED;
            parent->color = RBTREE_BLACK;
          }
          else if (parent->right == newnode) // case 2
          {
            rotate(parent, t, LEFT);
            rb_insert_fixup(parent, t);
          }
          break;
        case RIGHT: // parent가 grandparent의 오른쪽 자식
          if (parent->right == newnode) // case 3
          { 
            rotate(grandparent, t, LEFT);
            if (grandparent == t->root) {
              t->root = parent;
            }
            grandparent->color = RBTREE_RED;
            parent->color = RBTREE_BLACK;
          }
          else if (parent->left == newnode) // case 2
          {
            rotate(parent, t, RIGHT);
            rb_insert_fixup(parent, t);
          }
          break;
      }
    }
  }
}

void rotate(node_t *grandparent, rbtree *t, int mode) {

  node_t *parent;
  node_t *brother;

  if (mode == LEFT) {
    parent = grandparent->right; // 여기 모드 바뀌면 left<->right
    brother = parent->left; // 여기 모드 바뀌면 right<->left
    grandparent->right = parent->left; // 모드 바뀌면 left<->right
    parent->left = grandparent; // 모드 바뀌면 left<->right  
  }
  else if (mode == RIGHT) {
    parent = grandparent->left; // 여기 모드 바뀌면 left<->right
    brother = parent->right; // 여기 모드 바뀌면 right<->left
    grandparent->left = parent->right; // 모드 바뀌면 left<->right
    parent->right = grandparent; // 모드 바뀌면 left<->right      
  }
  
  node_t *ggparent = grandparent->parent;

  // 위에 정의한 애들 자체 값을 바꾸는 건 아무 의미가 없음 유의! (lvalue로 못들어간다)

  parent->parent = grandparent->parent; // 이 두 줄은
  grandparent->parent = parent; // 순서가 바뀌면 안됨

  if (brother != t->nil) { // brother가 nil이 아니면 brother의 parent 값도 바꿔야함
    brother->parent = grandparent;
  }

  if (ggparent != t->nil) { // ggparent가 nil이 아니면 ggparent의 left/right도 바꿔야함
    if (ggparent->left == grandparent) {
      ggparent->left = parent;
    } else if (ggparent->right == grandparent) {
      ggparent->right = parent;
    }
  }

}



node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *finding_node = t->root;

  while (finding_node != t->nil) {
    if (finding_node->key == key) {
      return finding_node;
    } else if (finding_node->key > key) {
      finding_node = finding_node->left;
    } else {
      finding_node = finding_node->right;
    }
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *finding_node = t->root;

  while (finding_node->left != t->nil) {
    finding_node = finding_node->left;
  }

  return finding_node;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *finding_node = t->root;

  while (finding_node->right != t->nil) {
    finding_node = finding_node->right;
  }

  return finding_node;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
