abx は ファイル名うめ込みテキスト（バッチ）生成＆実行をする
コマンドラインツールです。

例えば

例> abx -r a:\filename.ext
        A:のディレクトリのどっかにある filename.extを探す.

例> abx *.txt 'nkf -Sw -O $x.utf8 $c' >a.bat
    (nkfによる一括 sjis->utf8 テキスト変換)
    カレントディレクトリにある*.txtファイルを検索してファイルごとに
          nkf -O hoge.utf8 hoge.txt
    のような文字列を生成、a.bat に出力

例> abx -sm -ci1 -y -x *.jpg 'move $c img_$+3i.jpg'
    (連番リネーム)
    カレントディレクトリにある*.jpgファイルを検索、数字列を数値として
    比較する名前順にソートし、ファイルごとに
          move "hoge hoge 2.jpg' img_001.jpg
          move "hoge hoge 10.jpg' img_002.jpg
    のように生成、最後にバッチ実行。


ライセンスは Boost Software License Version 1.0 です。
