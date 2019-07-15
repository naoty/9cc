typedef enum TokenKind TokenKind;
typedef struct Token Token;
typedef enum NodeKind NodeKind;
typedef struct Node Node;

// 抽象構文木のノードの種類
enum NodeKind {
  ND_E,    // ==
  ND_NE,   // !=
  ND_L,    // <
  ND_LE,   // <=
  ND_G,    // >
  ND_GE,   // >=
  ND_ADD,  // +
  ND_SUB,  // -
  ND_MUL,  // *
  ND_DIV,  // /
  ND_NUM,  // 整数
};

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;  // ノードの型
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
};

// 入力プログラム
extern char *user_input;

// 現在着目しているトークン
extern Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...);

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...);

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p);

Node *expr();

void gen(Node *node);
