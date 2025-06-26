#include "rbtree.h"

#include <stdlib.h>

typedef enum { LEFT, RIGHT, ROOT } mode;

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
  mode parent_is;

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
            // if (grandparent == t->root) { 
            //   t->root = parent;
            // }
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
            // if (grandparent == t->root) {
            //   t->root = parent;
            // }
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

void rotate(node_t *grandparent, rbtree *t, mode mode) {

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
  } else { // ggparent가 nil이라면 -> grandparent가 root였다는 의미 -> root 수정해줘야함
    t->root = parent;
    t->root->parent = t->nil;
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

int rbtree_erase(rbtree *t, node_t *p) { // 아무래도 이번에는 이 안에 직접 bst_erase를 구현하는게 나을거같음
  // 지금 필요한 게 삭제 대상 노드 위치(주소), 실제로 삭제된 노드의 색깔
  // 그러니까 그 판단을 여기서 해야함 -> 실제로 삭제될 노드가 삭제 대상 노드인지, 아니면 predecessor인지
  rbtree *left_subtree = t; // 임시로 t로 초기화

  color_t erased_color = RBTREE_RED;

  node_t *target = NULL;
  node_t **target_node = &target; // 이중 포인터를 안전하게 초기화하는 방법
  *target_node = rbtree_find(t, p->key);


  // 위 코드를 잘못 쓴 사례: 정의되지 않은 동작을 일으킴
  // node_t **target_node;
  // *target_node = rbtree_find(t, p->key);
  // 이렇게 써버리면 어딘지도 모르는 곳에 rbtree_find의 반환값을 쓰겠다는 의미
  node_t *succeeding_node = *target_node; // 얘는 그냥 단순 초기화
  node_t *erased_node = *target_node;
  mode target_is;

  // 판단 1 -> 타겟을 찾았는가? 찾지 못했는가?
  if ((*target_node) == NULL) { // 타겟을 찾지 못함 -> 함수 종료
    return 0;
  }

  // 판단 2 -> 타겟이 부모의 왼쪽 자식인가? 오른쪽 자식인가? 부모가 없는 root인가? 
  if ((*target_node) == (*target_node)->parent->left) {
    target_is = LEFT;
  } else if ((*target_node) == (*target_node)->parent->right) {
    target_is = RIGHT;
  } else {
    target_is = ROOT;
  }

  // 판단 3 -> 타겟의 자식이 둘인가? 하나인가? 없는가?
  if ((*target_node)->left == t->nil && (*target_node)->right == t->nil) { // 타겟의 자식이 없다
    switch (target_is) {
      case LEFT:
        (*target_node)->parent->left = t->nil;
        break;
      case RIGHT:
        (*target_node)->parent->right = t->nil;
        break;
      case ROOT:
        t->root = t->nil;
        free(erased_node);
        return 0;
    }
    erased_color = (*target_node)->color; // 삭제되는 색깔 = 삭제되는 노드의 색깔
    t->nil->parent = (*target_node)->parent;

  }

  else if ((*target_node)->left == t->nil || (*target_node)->right == t->nil) { // 타겟의 자식이 하나만 있다
    erased_color = (*target_node)->color; // 삭제되는 색깔 = 삭제되는 노드의 색깔
    if ((*target_node)->right == t->nil) { // 자식이 왼쪽에 있었을 경우
      (*target_node)->left->parent = (*target_node)->parent;
      switch (target_is)
      {
      case LEFT:
        (*target_node)->parent->left = (*target_node)->left;
        break;
      case RIGHT:
        (*target_node)->parent->right = (*target_node)->left;
        break;
      case ROOT:
        t->root = (*target_node)->left;
        break;
      }
      succeeding_node = (*target_node)->left; // 삭제 노드의 대체 노드 -> 왼쪽 자식
    }
    else { // 자식이 오른쪽에 있었을 경우
      (*target_node)->right->parent = (*target_node)->parent;
      switch (target_is)
      {
      case LEFT:
        (*target_node)->parent->left = (*target_node)->right;
        break;
      case RIGHT:
        (*target_node)->parent->right = (*target_node)->right;
        break;
      case ROOT:
        t->root = (*target_node)->right;
        break;
      }
      succeeding_node = (*target_node)->right; // 삭제 노드의 대체 노드 -> 오른쪽 자식
    }

    if ((*target_node) == t->root) { // 루트 변경이 필요할 경우 해줘야함
      t->root = succeeding_node;
    }
  
  }
  
  else { // 타겟이 둘다 자식이 있다
    left_subtree = new_rbtree(); // predecessor 정보만 알아내는 용도
    left_subtree->root = (*target_node)->left;
    left_subtree->nil = t->nil; // 얘를 안해주면 rbtree_max 함수 while 조건 못빠져나와서 무한루프 빠짐
    node_t *predecessor = rbtree_max(left_subtree);

    //predecessor = rbtree_find(t, predecessor->key); // 이제 진짜 predecessor 주소로 교체..할 필요는 없었음
    
    erased_color = predecessor->color; // 삭제되는 색을 predecessor 색으로 세팅
    (*target_node)->key = predecessor->key;

    // predecessor의 특징 -> 오른쪽 자식이 nil이며, 무조건 parent의 오른쪽 자식이거나, parent가 없다(=왼쪽 subtree의 ROOT이다)
 
    erased_node = predecessor;// 그 여부와 상관없이..
    

    if (predecessor->left == t->nil) { // predecessor의 왼쪽 자식이 없는 경우 nil이 predecessor를 대체해야 하는데, 그러면 곤란해지므로
      t->nil->parent = predecessor; // predecessor 자체롤 pseudo-nil로 변형시키..면 안됨
      succeeding_node = predecessor; // 자기 자신을 대체했다고 처리
    }
    else { // predecessor가 왼쪽 자식이 있는 경우, 왼쪽 자식이 predecessor를 대체
      if (predecessor->parent == (*target_node)) { // predecessor가 왼쪽 subtree의 ROOT이면, predecessor의 parent는 target 노드
        predecessor->parent->left = predecessor->left; // predecessor는 target 노드의 왼쪽 자식이므로, target 노드의 왼쪽 자식에 predecessor의 왼쪽 자식을 연결
      }
      else { // predecessor가 왼쪽 subtree의 root가 아닐 경우, 무조건 parent의 오른쪽 자식
        predecessor->parent->right = predecessor->left; // 따라서 parent 노드의 오른쪽 자식에 predecessor의 왼쪽 자식을 연결
      }
      succeeding_node = predecessor->left; // 왼쪽 자식이 predecessor를 대체했다고 처리
    }

  }

  if (erased_color == RBTREE_BLACK) {
    rb_delete_fixup(t, succeeding_node);
  }
  
  free(erased_node);
  t->nil->parent = t->nil;
  
  return 0;
}

void rb_delete_fixup(rbtree *t, node_t *extra_black_node) { // double_black_node로 바꾸는게 낫지 않나..?

  mode extra_black_node_is;
  node_t *brother;
  node_t *nephew_left;
  node_t *nephew_right;
  node_t *parent = extra_black_node->parent;

  // #2 위반을 제일 먼저 체크.. 라기보다는 그냥 루트가 더블블랙이면 무조건
  if (extra_black_node == t->root) {
    t->root->color = RBTREE_BLACK;
    return;
  }

  // 이제 extra_black_node에 extra black을 부여했다고 가정
  if (extra_black_node->color == RBTREE_RED) { //extra black을 부여한 노드가 red라면
    extra_black_node->color = RBTREE_BLACK; // 그 노드를 black으로 바꿔주고 함수 종료
    return;
  }

  if (extra_black_node == extra_black_node->parent->left) {
    extra_black_node_is = LEFT;
    brother = extra_black_node->parent->right;
  } else if (extra_black_node == extra_black_node->parent->right) {
    extra_black_node_is = RIGHT;
    brother = extra_black_node->parent->left;
  } else {
    extra_black_node_is = ROOT;
  }

  nephew_left = brother->left;
  nephew_right = brother->right;

  switch (extra_black_node_is)
  {
    case LEFT:
      if (brother->color == RBTREE_RED) { // Case 1
        brother->color = RBTREE_BLACK;
        parent->color = RBTREE_RED;
        rotate(parent, t, LEFT);
        rb_delete_fixup(t, extra_black_node);
      }
      else if (nephew_left->color == RBTREE_BLACK && nephew_right->color == RBTREE_BLACK) { // Case 2
        brother->color = RBTREE_RED;
        rb_delete_fixup(t, parent);
      }

      else if (nephew_right->color == RBTREE_BLACK) { // Case 3
        nephew_left->color = RBTREE_BLACK;
        brother->color = RBTREE_RED;
        rotate(brother, t, RIGHT);
        rb_delete_fixup(t, extra_black_node);
      }

      else { // Case 4
        brother->color = parent->color;
        nephew_right->color = RBTREE_BLACK;
        parent->color = RBTREE_BLACK;
        rotate(parent, t, LEFT);
      }

      break;
    case RIGHT:
      if (brother->color == RBTREE_RED) { // Case 1
        brother->color = RBTREE_BLACK;
        parent->color = RBTREE_RED;
        rotate(parent, t, RIGHT);
        rb_delete_fixup(t, extra_black_node);
      }
      else if (nephew_left->color == RBTREE_BLACK && nephew_right->color == RBTREE_BLACK) { // Case 2
        brother->color = RBTREE_RED;
        rb_delete_fixup(t, parent);
      }

      else if (nephew_left->color == RBTREE_BLACK) { // Case 3
        nephew_right->color = RBTREE_BLACK;
        brother->color = RBTREE_RED;
        rotate(brother, t, LEFT);
        rb_delete_fixup(t, extra_black_node);
      }

      else { // Case 4
        brother->color = parent->color;
        nephew_left->color = RBTREE_BLACK;
        parent->color = RBTREE_BLACK;
        rotate(parent, t, RIGHT);
      }    
      break;
  }

}

// void pseudo_nilify(node_t *node, rbtree *t) {
//   node->color = RBTREE_BLACK;
//   node->key = t->nil->key;
//   node->left = t->nil;
//   node->right = t->nil;
// }

// node_t *bst_erase(rbtree *t, node_t *p) { // 대체한 노드의 주소 반환?
//   key_t key = p->key;
//   rbtree *left_subtree = t; // 일단 t로 초기화
//   mode target_is;
//   color_t target_color = RBTREE_RED; // 일단 red로 초기화

//   node_t **target_node;
//   *target_node = rbtree_find(t, key);

//   if ((*target_node) == NULL) { // 해당하는 node를 찾지 못했을 경우
//     return NULL; // 대체된 노드가 없으므로 NULL을 리턴
//   } else {
//     target_color = (*target_node)->color; //일단 타겟 노드 복사
//   }

//   if ((*target_node) == (*target_node)->parent->left) {
//     target_is = LEFT;
//   } else if ((*target_node) == (*target_node)->parent->right) {
//     target_is = RIGHT;
//   } else {
//     target_is = ROOT;
//   }


//   if ((*target_node)->left == t->nil && (*target_node)->right == t->nil)
//   { // 삭제되는 노드의 자식이 없는 경우
//     (*target_node)->color = RBTREE_BLACK;
//     (*target_node)->key = 0;
//     (*target_node)->parent = t->nil;
//     (*target_node)->left = t->nil;
//     (*target_node)->right = t->nil;
//     }
//     // free((*target_node));
//     return target_node;
//   }
//   else if ((*target_node)->left == t->nil || (*target_node)->right == t->nil)
//   { // 삭제되는 노드의 자식이 하나만 있는 경우
//     if ((*target_node)->right == t->nil) { // 자식이 왼쪽에 있었을 경우
//       (*target_node)->left->parent = (*target_node)->parent;
//       switch (target_is)
//       {
//       case LEFT:
//         (*target_node)->parent->left = (*target_node)->left;
//         break;
//       case RIGHT:
//         (*target_node)->parent->right = (*target_node)->left;
//         break;
//       case ROOT:
//         t->root = (*target_node)->left;
//         break;
//       }
//       free((*target_node));
//       return target_color;
//     }
//     else { // 자식이 오른쪽에 있었을 경우
//       (*target_node)->right->parent = (*target_node)->parent;
//       switch (target_is)
//       {
//       case LEFT:
//         (*target_node)->parent->left = (*target_node)->right;
//         break;
//       case RIGHT:
//         (*target_node)->parent->right = (*target_node)->right;
//         break;
//       case ROOT:
//         t->root = (*target_node)->right;
//         break;
//       }
//       free((*target_node));
//       return target_color;
//     }
//   }
//   else 
//   { // 자식이 둘 다 있었을 경우
//     left_subtree = new_rbtree();
//     left_subtree->root = (*target_node)->left;
//     node_t **predecessor;
//     *predecessor = rbtree_max(left_subtree);
//     // predecessor의 특징 -> 오른쪽 자식이 nil이며, 무조건 parent의 오른쪽 자식이거나 ROOT이다
//     (*target_node)->key = (*predecessor)->key; // target 자리에 predecessor의 값 복사 (색깔은 target 색 유지)
//     (*predecessor)->parent->left = (*predecessor)->left;
    
//     if ((*predecessor)->parent != (*target_node)) {
//       (*predecessor)->parent->right = t->nil;
//     }

//     if ((*predecessor)->left != t->nil) {
//       (*predecessor)->left->parent = (*predecessor)->parent;
//     }
//     target_color = (*predecessor)->color; // 삭제되는 색이 target색이 아니라 predecessor의 색이 됨
//     free((*predecessor));
//     return target_color;
//   }

// }

// 삭제된 노드 색깔 반환 버전
// color_t bst_erase(rbtree *t, node_t *p) {
//   key_t key = p->key;
//   rbtree *left_subtree = t; // 일단 t로 초기화
//   mode target_is;
//   color_t target_color = RBTREE_RED; // 일단 red로 초기화

//   node_t **target_node;
//   *target_node = rbtree_find(t, key);

//   if ((*target_node) == NULL) { // 해당하는 node를 찾지 못했을 경우
//     return target_color; // 아무 일도 없어야 하므로 red를 리턴
//   } else {
//     target_color = (*target_node)->color;
//   }

//   if ((*target_node) == (*target_node)->parent->left) {
//     target_is = LEFT;
//   } else if ((*target_node) == (*target_node)->parent->right) {
//     target_is = RIGHT;
//   } else {
//     target_is = ROOT;
//   }


//   if ((*target_node)->left == t->nil && (*target_node)->right == t->nil)
//   { // 삭제되는 노드의 자식이 없는 경우
//     switch (target_is) {
//       case LEFT:
//         (*target_node)->parent->left = t->nil;
//         break;
//       case RIGHT:
//         (*target_node)->parent->right = t->nil;
//         break;
//       case ROOT:
//         t->root = t->nil;
//         break;
//     }
//     free((*target_node));
//     return target_color;
//   }
//   else if ((*target_node)->left == t->nil || (*target_node)->right == t->nil)
//   { // 삭제되는 노드의 자식이 하나만 있는 경우
//     if ((*target_node)->right == t->nil) { // 자식이 왼쪽에 있었을 경우
//       (*target_node)->left->parent = (*target_node)->parent;
//       switch (target_is)
//       {
//       case LEFT:
//         (*target_node)->parent->left = (*target_node)->left;
//         break;
//       case RIGHT:
//         (*target_node)->parent->right = (*target_node)->left;
//         break;
//       case ROOT:
//         t->root = (*target_node)->left;
//         break;
//       }
//       free((*target_node));
//       return target_color;
//     }
//     else { // 자식이 오른쪽에 있었을 경우
//       (*target_node)->right->parent = (*target_node)->parent;
//       switch (target_is)
//       {
//       case LEFT:
//         (*target_node)->parent->left = (*target_node)->right;
//         break;
//       case RIGHT:
//         (*target_node)->parent->right = (*target_node)->right;
//         break;
//       case ROOT:
//         t->root = (*target_node)->right;
//         break;
//       }
//       free((*target_node));
//       return target_color;
//     }
//   }
//   else 
//   { // 자식이 둘 다 있었을 경우
//     left_subtree = new_rbtree();
//     left_subtree->root = (*target_node)->left;
//     node_t **predecessor;
//     *predecessor = rbtree_max(left_subtree);
//     // predecessor의 특징 -> 오른쪽 자식이 nil이며, 무조건 parent의 오른쪽 자식이거나 ROOT이다
//     (*target_node)->key = (*predecessor)->key; // target 자리에 predecessor의 값 복사 (색깔은 target 색 유지)
//     (*predecessor)->parent->left = (*predecessor)->left;
    
//     if ((*predecessor)->parent != (*target_node)) {
//       (*predecessor)->parent->right = t->nil;
//     }

//     if ((*predecessor)->left != t->nil) {
//       (*predecessor)->left->parent = (*predecessor)->parent;
//     }
//     target_color = (*predecessor)->color; // 삭제되는 색이 target색이 아니라 predecessor의 색이 됨
//     free((*predecessor));
//     return target_color;
//   }

// }






int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
