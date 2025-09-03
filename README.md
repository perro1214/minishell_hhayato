# hhayato

## 変更点

### パイプライン実行機能の実装

- **修正箇所**:
  - **command_execution.c :** 
    - 追加した関数
      
      process_heredoc(const char *delimiter)
      
    - 修正した関数
      
      apply_redirections()
      
  - **lexer.c :**
    - 追加した関数  
      has_unclosed_quote(const char *str)
      
      handle_multiline_input(const char *initial_input)

## 機能

### Lexer（字句解析）
入力された文字列を以下のトークンに分解します：
- `EXPANDABLE`: 通常の文字列（環境変数展開対象）
- `EXPANDABLE_QUOTED`: ダブルクォート内文字列（環境変数展開対象）
- `NON_EXPANDABLE`: シングルクォート内文字列（環境変数展開なし）
- `PIPE`: パイプ（|）
- `REDIRECT_IN`: 入力リダイレクション（<）
- `REDIRECT_OUT`: 出力リダイレクション（>）
- `REDIRECT_APPEND`: 追記リダイレクション（>>）
- `REDIRECT_HEREDOC`: ヒアドキュメント（<<）
- `EOF_TOKEN`: 終端トークン

### Parser（構文解析）
トークンから抽象構文木（AST）を生成します。以下の構文をサポート：
- 基本コマンド
- パイプライン
- リダイレクション
- クォート処理

### Command Execution（コマンド実行）
ASTをコマンド実行構造体に変換し、実際のコマンド実行に必要な情報を管理します：
- **Redirection管理**: 入力・出力・追記・ヒアドキュメントのリダイレクション処理
- **引数配列生成**: コマンドと引数の配列作成
- **パイプライン対応**: パイプで連結されたコマンドの実行構造体作成
- **メモリ管理**: 動的に割り当てられたメモリの適切な解放

## ファイル構成

- `lexer_parser.h`: ヘッダーファイル
- `lexer.c`: Lexer実装
- `parser.c`: Parser実装
- `command_execution.c`: コマンド実行構造体の変換・管理
- `main.c`: テスト用メイン関数
- `Makefile`: ビルド設定

## ビルド方法

```bash
make
```

## 使用方法

```bash
./lexer_parser
```

プログラム実行後、コマンドを入力するとトークン化とAST生成の結果が表示されます。

## テスト例

```bash
# 基本コマンド
echo hello world

# パイプライン
ls -la | grep test

# 連続パイプ
ls -la | wc -l | wc -l

# リダイレクション
cat < input.txt > output.txt

# 複合
ls -la | grep test > output.txt

# 環境変数展開
echo $HOME
echo "$USER lives in $HOME"

# クォート
echo "hello world" 'test'

# ヒアドキュメント
cat << EOF
```

## 使用例

```bash
 % ./lexer_parser 
Lexer & Parser Test Program
Enter a command (or press Ctrl+D to exit):
echo "hello world" 'test'

--- Input: echo "hello world" 'test' ---

=== TOKENS ===
Type: EXPANDABLE        Value: echo
Type: EXPANDABLE_QUOTED Value: hello world
Type: NON_EXPANDABLE    Value: test
Type: EOF_TOKEN         Value: (null)

=== AST ===
EXPANDABLE: echo
└─ right:
  EXPANDABLE_QUOTED: hello world
  └─ right:
    NON_EXPANDABLE: test

Enter a command (or press Ctrl+D to exit):

Goodbye!
```

```bash
 % make test
Testing lexer and parser...
Lexer & Parser Test Program
Enter a command (or press Ctrl+D to exit):

--- Input: echo hello world ---

=== TOKENS ===
Type: EXPANDABLE        Value: echo
Type: EXPANDABLE        Value: hello
Type: EXPANDABLE        Value: world
Type: EOF_TOKEN         Value: (null)

=== AST ===
EXPANDABLE: echo
└─ right:
  EXPANDABLE: hello
  └─ right:
    EXPANDABLE: world

Enter a command (or press Ctrl+D to exit):

Goodbye!
Lexer & Parser Test Program
Enter a command (or press Ctrl+D to exit):

--- Input: ls -la | grep test > output.txt ---

=== TOKENS ===
Type: EXPANDABLE        Value: ls
Type: EXPANDABLE        Value: -la
Type: PIPE              Value: |
Type: EXPANDABLE        Value: grep
Type: EXPANDABLE        Value: test
Type: REDIRECT_OUT      Value: >
Type: EXPANDABLE        Value: output.txt
Type: EOF_TOKEN         Value: (null)

=== AST ===
PIPE: |
├─ left:
  EXPANDABLE: ls
  └─ right:
    EXPANDABLE: -la
└─ right:
  EXPANDABLE: grep
  └─ right:
    EXPANDABLE: test
    └─ right:
      REDIRECT_OUT: >
      └─ right:
        EXPANDABLE: output.txt

Enter a command (or press Ctrl+D to exit):

Goodbye!
Lexer & Parser Test Program
Enter a command (or press Ctrl+D to exit):

--- Input: cat < input.txt | wc -l >> count.txt ---

=== TOKENS ===
Type: EXPANDABLE        Value: cat
Type: REDIRECT_IN       Value: <
Type: EXPANDABLE        Value: input.txt
Type: PIPE              Value: |
Type: EXPANDABLE        Value: wc
Type: EXPANDABLE        Value: -l
Type: REDIRECT_APPEND   Value: >>
Type: EXPANDABLE        Value: count.txt
Type: EOF_TOKEN         Value: (null)

=== AST ===
PIPE: |
├─ left:
  EXPANDABLE: cat
  └─ right:
    REDIRECT_IN: <
    └─ right:
      EXPANDABLE: input.txt
└─ right:
  EXPANDABLE: wc
  └─ right:
    EXPANDABLE: -l
    └─ right:
      REDIRECT_APPEND: >>
      └─ right:
        EXPANDABLE: count.txt

Enter a command (or press Ctrl+D to exit):

Goodbye!
Lexer & Parser Test Program
Enter a command (or press Ctrl+D to exit):

--- Input: cat << EOF ---

=== TOKENS ===
Type: EXPANDABLE        Value: cat
Type: REDIRECT_HEREDOC  Value: <<
Type: EXPANDABLE        Value: EOF
Type: EOF_TOKEN         Value: (null)

=== AST ===
EXPANDABLE: cat
└─ right:
  REDIRECT_HEREDOC: <<
  └─ right:
    EXPANDABLE: EOF

Enter a command (or press Ctrl+D to exit):

Goodbye!
```
