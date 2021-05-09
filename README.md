インターンシップ応募にあたり一時的に公開許可をもらい公開中  
主に装甲板認識部を担当(armor_detector,yolo)
チーム内wikiをもとに記述

# Overview
[RoboMaster](https://www.robomaster.jp/) での自動照準用の装甲版認識プログラム   
intel NUC,Ubuntu18.04などを用いて開発
詳細は[ArmorDetectorROS](https://github.com/rikupo/ArmorDetectorROS)のREDMEにて

# DetectArmor
装甲板検出プログラム ROI実装版 11/04版 OpenCV4.4対応版  
OpenCV3系とOpenCV4系でホットワードが違うため互換性なし．  
例  
CV3 : frames = int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_COUNT))  
CV4 : frames = int(cap.get(7))  

# 概要
装甲版認識プログラム
画像をモノクロにしてしきい値処理して残った明るい部分=装甲としてそれを矩形近似する感じ

前フレームで装甲を認識した場合,次のフレームでは捜索範囲を前回の装甲の3倍(大体の機体サイズ+余裕分)の領域に絞る．  
認識できなかった場合，以降は画像領域を徐々に拡大する．ここはパラメータ要調整．  

毎ループで閾値変更のキー入力待機を1ms入れている．
  
# 実行結果と効果  
約30%の高速化  

# 動作環境
ubuntu 18.04  
OpenCV 4.4  
i5-4258U @ 2.4 GHz RAM DDR3 8GB
ターミナルで実行　　

# Build
videoディレクトリにgoogleドライブに上がっている動画をコピーしてくる
```
cd detectArmor
mkdir build
cd build
cmake ..
make
```
# 動作
 * nキーで画像をすすめる　これは無効化 順次次のフレームを処理
 * qキーでプログラムを終了
 * uキーでグレースケール画像から2値画像へ変更するときのしきい値をインクリメント
 * dキーでグレースケール画像から2値画像へ変更するときのしきい値をディクリメント
 * rキーでカラー画像（厳密には違う）から2値画像へ変更するときのしきい値をインクリメント
 * fキーでカラー画像（厳密には違う）から2値画像へ変更するときのしきい値をディクリメント
