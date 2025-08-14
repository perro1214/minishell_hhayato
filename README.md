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

## ファイル構成

- `lexer_parser.h`: ヘッダーファイル
- `lexer.c`: Lexer実装
- `parser.c`: Parser実装
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

# リダイレクション
cat < input.txt > output.txt

# 複合
ls -la | grep test > output.txt

# クォート
echo "hello world" 'test'

# ヒアドキュメント
cat << EOF
```

## 注意事項

- この実装はminishellの基本的なLexer/Parser機能のみを提供
- 実際のコマンド実行は別途実装が必要
- エラーハンドリングは最小限（本格実装時は拡張が必要）