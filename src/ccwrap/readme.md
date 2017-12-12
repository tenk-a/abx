# ccwrap


c11/c++11以前の古いC/C++コンパイラ向けに、標準ライブラリのincludeに
ワンクッション噛ませて、いくらか定義を補完する。


## 対応は大まかに２つ

1. コンパイラ独自名(__alignof等)を新規標準名(alignof等)へ#defineしたり
   stdint.h 等の足りてない標準cヘッダの追加
   対象コンパイラ:
   - MS Visual C/C++ (主に 8～14 / 2005～2015)       (vc)
   - borland c/c++ v5.5.1 (bc55) Embarcadero C++ 7.2 (bcc32c) 
   - open watcom c/c++ v1.9                          (ow)
   - digital mars c/c++                              (dmc)
   - mingw (c++11 対応以降の gcc/g++)
2. boost を用いて新規標準ヘッダや追加機能を代用
   - namespace std に using ::boost::????; するだけ
   - 実質 vc のみ.
   - (現状 include してみただけでほとんど使っていないので実用性不明)


## 暗黙include・includeパス

コンパイラオプションで 
- 暗黙のincludeファイル指定  
  (vc: -FI  bc55:無し  ow:-fi=  dmc:-HI  mingw(gcc):-include)
- ccwrap/??/ ディレクトリを、他より先に検索されるように include パス設定
  することで標準ライブラリをフックして定義を追加している。  
  (vc: -I  bc55:-I  ow:-i=  dmc:-I  mingw(gcc):-I )



## 辻褄合わせ内容

### ccwrap_header.h

基本的に、コンパイラオプションで暗黙の include で使用するのを想定。

ただ 各コンパイラの頻用ヘッダ(stddef.h等)をフックしてそこで ccwrap_header.h
をincludeするようにしているので必ず標準ヘッダをicnludeすればその限りでない。  
(暗黙includeの出来ないbc55はこれで対処)

可能であれば、コンパイラ独自の相当機能、通常文法範囲での実装、空定義等で
#define して代用品を用意している

- c++用:  
    alignas, alignof, char16_t, char32_t, final, __func__,  
    noexcept, nullptr, override, _Pragma, static_assert, thread_local
- c 用:  
    _Alignas, _Alignof, _Bool, inline, _Noreturn, restrict,  
    _Static_assert, _Thread_local  
    (※vcでは__restrictをrestrictに#defineすると問題有で未定義)
- ※ その他の(c89/c++03より)新しい語は未定義
