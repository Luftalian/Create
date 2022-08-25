# SDIOLogWrapper.cpp 使い方
文字列をキューへ投げ込むと勝手に処理してくれるライブラリを作成しました。文字列は`sprintf()`とかで適当に生成してください。namespace SDIOLogWrapperで囲ってあります。

# 初期設定
## キューの作成
```
int makeQueue(int uxQueueLength, int uxQueueSize);
```
- `int makeQueue(int uxQueueLength, int uxQueueSize);` 長さ`uxQueueSize`byteのデータが`uxQueueLength`個入るキューを作成する。返り値が1でキューの作成に失敗。

## セーブファイル名の設定
```
void setSaveFileName(const char *);
```
ファイル名を変更できます。sdカード直下の`hoge.csv`に保存したい場合、`/hoge.csv`を引数に与えてください。絶対パスです。

## セーブ頻度の設定
```
void setSaveInterval(int);
```
セーブ頻度の設定です。キューに対し、与えた数に1度、SDカードへの確実な保存を行います。


# キューからSDへ書き込むタスクの起動
```
int initSD();
void openFile();
void writeTaskCreate(int TaskExecuteCore);
```

- `int initSD();` SDカードを接続する。帰り値が0で正常、返り値が1で失敗。
- `void openFile();` ファイルを開く。開かれるファイルはデフォルトでは`logging.csv`固定。上書きでなく常に追記される。現状変更には`SDIOLogWrapper.cpp`を編集する必要がある。
- `void writeTaskCreate(int TaskExecuteCore);` キューを処理するタスクを展開するコアを指定する。`TaskExecuteCore`は`APP_CPU_NUM`を推奨。（`PRO_CPU_NUM`にデータ取得のタスクのみを展開することでロギングを安定させるため）


# キューへのデータの投げ込み
```
int appendQueue(char xData[]);
```

- `appendQueue(char xData[])` キューへデータを追加する。長さは`uxQueueSize`であること。

# 終了処理
```
void writeTaskDelete();
void closeFile();
void deinitSD();
```
タスクキルしてファイル閉じてSD取り外しできるようにします。



