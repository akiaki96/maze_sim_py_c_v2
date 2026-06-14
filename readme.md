# Maze Simulator Python-C V2

マイクロマウスの迷路シミュレーター  
Pythonで実装されたシミュレータとC言語で実装されたソルバーアルゴリズムを組み合わせたプロジェクトです。

https://github.com/user-attachments/assets/7bbd9405-3f24-4bff-a597-ca004da9034e

## 概要

このプロジェクトは迷路シミュレータを提供し、異なるアルゴリズムによる迷路解法を実装・テストできます。
- Pythonでの迷路管理と描画
- C言語で実機に導入できるソルバー
- 迷路画像のパースと処理

## 要件

- Python 3.9+
- Virtual environment (myenv)

## インストール
0. ローカルにダウンロード：
   ```
   git clone https://github.com/akiaki96/maze_sim_py_c_v2.git
   ```

   または
   https://github.com/akiaki96/maze_sim_py_c_v2
   からダウンロード

1. 仮想環境を作成：
   ```
   cd [project dir]
   python3 -m venv myenv
   ```

1. 仮想環境を有効化：
   ```
   # Windows
   myenv\Scripts\activate
   
   # macOS / Linux
   source myenv/bin/activate
   ```

2. 依存パッケージをインストール：
   ```
   pip install -r requirements.txt
   ```

## 依存パッケージ

- pygame: グラフィックス描画
- numpy: 数値計算
- opencv-python: 画像処理

## 基本的な操作

### メインプログラムの実行
```
python main.py
```

### 迷路ロジックの実行
```
python maze_logic.py
```

### 迷路のパース
迷路画像を処理する場合：
```
python parse_maze_image.py
```

### ソルバーAPIの使用
```
python solver_api.py
```

## 機能

<!-- 以下に機能説明を追加してください -->

- **迷路シミュレーション**: Pythonベースの迷路管理と描画
- **複数のソルバーアルゴリズム**: C言語実装による高速ソルバー
- **画像解析**: 迷路画像のパースと自動認識

## プロジェクト構造

```
.
├── main.py                  # メインエントリーポイント
├── maze.py                  # 迷路クラス定義
├── maze_logic.py            # 迷路シミュレーション ロジック
├── parse_maze_image.py      # 迷路画像解析
├── render.py                # 描画処理
├── solver_api.py            # ソルバーAPI
├── action_enum.py           # アクション列挙型
├── action_loader.py         # アクション読み込み
├── build_solver.py          # ソルバービルドスクリプト
├── action.json              # アクション設定
├── solver_config.json       # ソルバー設定
├── requirements.txt         # Python依存パッケージ
├── maze_image/              # 迷路画像ディレクトリ
├── solver/                  # C言語ソルバー実装
│   ├── solver.c             # メインソルバー
│   ├── solver_left_wall.c   # 左手法
│   ├── solver_adachi.c      # 足立法
│   ├── maze.c               # 迷路データ構造
│   ├── global.c             # グローバル関数
│   ├── my_lib.c             # ライブラリ関数
│   ├── action.h             # アクションヘッダ
│   ├── global.h             # グローバルヘッダ
│   └── maze.c               # 迷路ヘッダ
└── myenv/                   # Python仮想環境

```

## トラブルシューティング

<!-- 以下に問題解決方法を追加してください -->

### ImportError が発生する場合
- 仮想環境が有効化されているか確認
- `pip install -r requirements.txt` を実行

### C言語ソルバーのビルドエラー
- `python build_solver.py` を実行してソルバーを再ビルド

### 画像ファイルが見つからない場合
- `maze_image/` ディレクトリに迷路画像を配置

## 設定

### solver_config.json
ソルバーの動作設定をカスタマイズできます。

### action.json
アクション（動作）の定義をカスタマイズできます。

## 開発ガイド

<!-- 以下に開発情報を追加してください -->

### ソルバーアルゴリズムの追加
1. `solver/` ディレクトリに新しい C ファイルを作成
2. `build_solver.py` を更新してビルド設定に追加
3. `solver_api.py` でアルゴリズムを登録

### カスタムアクションの定義
`action.json` と `action_enum.py` を更新して新しいアクションを定義

### 迷路画像の処理
`parse_maze_image.py` を参考に、カスタム画像処理を実装

## ライセンス

MIT License  
Copyright (c) 2026 akiaki96

## 参考資料

KERI's Lab <br>
マイクロマウスのターン一覧 <br>
https://www.kerislab.jp/posts/2017-09-03-pattern-of-turn/

## 変更履歴

2.0.0 readmeを追加 
