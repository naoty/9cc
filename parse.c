#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

// トークンの種類
enum TokenKind {
  TK_RESERVED,  // 記号
  TK_NUM,       // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
};

// トークン型
struct Token {
  TokenKind kind;  // トークンの型
  Token *next;     // 次の入力トークン
  int val;         // kindがTK_NUMの場合、その数値
  char *str;       // トークン文字列
  int len;         // トークンの長さ
};

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strncmp(token->str, op, strlen(op)) != 0)
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
  if (token->kind != TK_RESERVED || strncmp(token->str, op, strlen(op)) != 0)
    error_at(token->str, "'%s'ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM) error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() { return token->kind == TK_EOF; }

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = strlen(str);
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0 ||
        strncmp(p, ">=", 2) == 0 || strncmp(p, "<=", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p);
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '>' || *p == '<') {
      // p++はpを返してからインクリメントしている
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);

      // 読んだ文字の次の文字に進める
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *term();
Node *unary();

Node *expr() {
  Node *node = equality();
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_E, node, relational());
      continue;
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      break;
    }
  }

  return node;
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<=")) {
      node = new_node(ND_LE, node, add());
      continue;
    } else if (consume("<")) {
      node = new_node(ND_L, node, add());
      continue;
    } else if (consume(">=")) {
      node = new_node(ND_GE, add(), node);
      continue;
    } else if (consume(">")) {
      node = new_node(ND_G, add(), node);
      continue;
    } else {
      break;
    }
  }

  return node;
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
      continue;
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      break;
    }
  }

  return node;
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      break;
    }
  }

  return node;
}

Node *unary() {
  if (consume("+")) {
    return term();
  } else if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), term());
  } else {
    return term();
  }
}

Node *term() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  return new_node_num(expect_number());
}
