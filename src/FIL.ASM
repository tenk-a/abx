;	module FIL
;		c_name	"","",1
;	
;	//	import	Pri
;	
;	//@define IsKanji(c)  ((c >= 0x81) && (c <= 0x9f) || (c >= 0xE0 && c <= 0xFC))
;	@define IsKanji(c)  ( (c) >= 0x81 && ((c) <= 0x9f || ((c) >= 0xE0 && (c) <= 0xFC) ) )
	.186
DGROUP	group	_DATA,_BSS

_TEXT	segment byte public 'CODE'
	assume	cs:_TEXT
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
_TEXT	ends

_DATA	segment word public 'DATA'
_DATA	ends

_BSS	segment word public 'BSS'
;	
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	var	FIL_zenFlg:w = 1
_BSS	ends

_DATA	segment word public 'DATA'
_FIL_zenFlg	label	word
	dw	01h
;	
;	*proc	FIL_SetZenMode(fff:w);cdecl;far
;	enter
;		save	ax,ds
;		ds = ax = %var
_DATA	ends

_TEXT	segment byte public 'CODE'
	public	_FIL_SetZenMode
_FIL_SetZenMode	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	ax
	push	ds
	push	bp
	mov	bp,sp
	mov	ax,DGROUP
	mov	ds,ax
;		FIL_zenFlg = ax = fff
	mov	ax,word ptr [bp+10]
	mov	word ptr [_FIL_zenFlg],ax
;		return
	db	0C9h	;leave
	pop	ds
	pop	ax
	db	0CBh	;retf
;	endproc

;	
;	*proc	FIL_GetZenMode();cdecl;far
;	begin
;		save	ds
;		ds = ax = %var
	public	_FIL_GetZenMode
_FIL_GetZenMode	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
;		ax = FIL_zenFlg
	mov	ax,word ptr [_FIL_zenFlg]
;		return
	pop	ds
	db	0CBh	;retf
;	endproc

;	
;	@define	FIL_GetZenModeMACRO()		\
;				push	ds;				\
;					ds = ax = %var;		\
;					ax = FIL_zenFlg;	\
;				pop		ds;				\
;				;
;	
;	
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	proc STR_End(s0:d)	//;cdecl;far
;	enter
;		save es,di,cx
;		rep.inc
_STR_End	label	near
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	cx
	push	di
	push	es
	push	bp
	mov	bp,sp
	cld
;		es.di = s0
	les	di,dword ptr [bp+10]
;		repn.scan di,0,0xffff
	mov	cx,0FFFFh
	xor	al,al
	repne	scasb
;		ax = di
	mov	ax,di
;		--ax
	dec	ax
;		dx = es
	mov	dx,es
;		return
	db	0C9h	;leave
	pop	es
	pop	di
	pop	cx
	ret	04h
;	endproc

;	
;	proc STR_Cpy(dst:d,src:d)
;	enter
;		save si,di,es,ds
;		ds.si = src
_STR_Cpy	label	near
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	si
	push	di
	push	ds
	push	es
	push	bp
	mov	bp,sp
	lds	si,dword ptr [bp+16]
;		es.di = dst
	les	di,dword ptr [bp+12]
;		loop
L$1:
;			rep.load al,si
	lodsb
;			rep.set di,al
	stosb
;		endloop (al != 0)
	or	al,al
	jne	L$1
;		dx.ax = dst
	mov	ax,word ptr [bp+12]
	mov	dx,word ptr [bp+14]
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	ret	08h
;	endproc

;	
;	proc STR_CpyUpr(dst:d,src:d)
;	enter
;		save si,di,es,ds
;		ds.si = src
_STR_CpyUpr	label	near
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	si
	push	di
	push	ds
	push	es
	push	bp
	mov	bp,sp
	lds	si,dword ptr [bp+16]
;		es.di = dst
	les	di,dword ptr [bp+12]
;		loop
L$4:
;			rep.load al,si
	lodsb
;			if (al >= 'a' && al <= 'z')
	cmp	al,061h
	jb	L$7
	cmp	al,07Ah
	ja	L$7
;				al -= 0x20
	sub	al,020h
;			fi
L$7:
;			rep.set di,al
	stosb
;		endloop (al != 0)
	or	al,al
	jne	L$4
;		dx.ax = dst
	mov	ax,word ptr [bp+12]
	mov	dx,word ptr [bp+14]
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	ret	08h
;	endproc

;	
;	
;	
;	
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	
;	/* ----------------------------------------------------------------------------
;	 ◎ FIL_SetWccMode,FIL_GetWccMode -- FIL_WcCmpの特殊文字の制御
;		void FIL_SetWccMode(int flags);	// 設定
;		int  FIL_GetWccMode(void);		// 取得
;		フラグ
;			bit 0:  ? の機能を 0:有効  1:抑止
;			bit 1:  * の機能を 0:有効  1:抑止
;			bit 2:  [ [^ の機能を 0:有効  1:抑止
;			bit 3:  ^ の機能を 0:有効  1:抑止
;			(bit 4:  MS全角     0:対応  1:非対応)
;	
;	
;	 ◎ FIL_WcCmp  -- シフトJIS 対応ワイルドカード機能付文字列比較
;		int  FIL_WcCmp(B F *key, B F *st);
;		proc wccmp(key:d, st:d,flg:w);cdecl
;		入力
;			key		ワイルドカード文字を含む文字列
;			st		文字列
;		出力
;			ax 	結果
;	
;		文字列 key と st を比較し、マッチすれば０をしなければ０以外を返す。
;		返される値の意味は、
;			0		key と st はマッチした
;			-1		key の指定がおかしい（ワイルドカード文字の指定ミスなど)
;			-2		st は key よりも文字列長が短い
;			正数n	st は 先頭 n-1 バイトで key とマッチした（st は key より長い）
;	
;		st は極端に長すぎてはいけない。一応、63Kバイトくらいに対応しているはず。
;		（正確には64K - 18バイトくらい？）
;		（ポインタの正規化はおこなわないので、呼び出し側で注意すること）
;	
;		文字列の終端は'\0'. 文字は全角(2ﾊﾞｲﾄ)文字も１文字として扱われる.
;	
;		ワイルド・カード文字の意味（c は 半角か全角文字の 1文字のこと）
;			*	  0文字以上の任意の文字列にマッチ.
;			?	  任意の1文字にマッチ.
;			^c	  c 以外の任意の１文字にマッチ.
;			[..]  [  ] でくくられた文字列（１文字以上）中のどれか１文字にマッチ.
;			[^..] [^ ] でくくられた文字列（１文字以上）中にない１文字にマッチ.
;	
;	　　括弧（[..],[^..]）のなかではワイルドカード文字の機能は抑制される。
;	　　括弧のなかに']' を含めたい場合は、[]abc] や [^]ed] のように、'[' や '[^'
;		の直後に']'を置く。
;		括弧内では、A-Z のように,'-'を用いて文字コードで A から Z の範囲を指定で
;		きる。ただし、'-' の両側に文字がないといけない。[-abc] や [abc-] のよう
;		に,'[' や '[^' の直後や ']' の直前に '-' があるばあい,'-'は 1文字として
;		扱われる。[0-9-A] というようなのは不定扱い（実際にはこの版では[0-99-A]と
;		指定されたばあいと同じ動作をして、エラーにしていない）
;		あと'-' の右側の文字は左側の文字よりも大きいものを指定しなければならない。
;		Z-A ではマッチしない。
;	   --------------------------------------------------------------------------*/
;	//	1992-1995	writen by M.Kitamura
;	
;	var FIL_wcFlg:w = 0x08
_TEXT	ends

_DATA	segment word public 'DATA'
_FIL_wcFlg	label	word
	dw	08h
;	
;	*proc FIL_SetWccMode(flg:w);cdecl;far
;	enter
;		save ax,ds
;		ds = ax = %var
_DATA	ends

_TEXT	segment byte public 'CODE'
	public	_FIL_SetWccMode
_FIL_SetWccMode	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	ax
	push	ds
	push	bp
	mov	bp,sp
	mov	ax,DGROUP
	mov	ds,ax
;		FIL_wcFlg = ax = flg
	mov	ax,word ptr [bp+10]
	mov	word ptr [_FIL_wcFlg],ax
;		return
	db	0C9h	;leave
	pop	ds
	pop	ax
	db	0CBh	;retf
;	endproc

;	
;	*proc FIL_GetWccMode();cdecl;far
;	begin
;		save ds
;		ds = ax = %var
	public	_FIL_GetWccMode
_FIL_GetWccMode	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
;		ax = FIL_wcFlg
	mov	ax,word ptr [_FIL_wcFlg]
;		return
	pop	ds
	db	0CBh	;retf
;	endproc

;	
;	proc FIL_WcCmp_(ds.si/*key*/, es.di/*fstr*/, dx/*flg*/)
;		//out ax
;	enter
;		save bx,cx,dx,si,di,es,ds
;		const _N_ = -2, _E_ = -1, _Y_ = 0
;		local  sptr:word
;		assume ds:nothing, es:nothing
_FIL_WcCmp_	label	near
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	cx
	push	dx
	push	bx
	push	si
	push	di
	push	ds
	push	es
	enter	02h,00h
	assume	ds:NOTHING,es:NOTHING
;	
;		//ポインタの正規化はおこなわない.
;		//ds.si = key
;		//es.di = fstr
;		sptr = di
	mov	word ptr [bp-2],di
;	
;		loop.w
L$8:
;			al = b[si]
	mov	al,byte ptr [si]
;			++si
	inc	si
;			if ((dl & 0x10) == 0 && IsKanji(al))
	test	dl,010h
	jne	L$11
	cmp	al,081h
	jb	L$11
	cmp	al,09Fh
	jbe	L$12
	cmp	al,0E0h
	jb	L$11
	cmp	al,0FCh
	ja	L$11
L$12:
;				ah = b[si]
	mov	ah,byte ptr [si]
;				go.w (ah == '\0') RTS_ERR
	or	ah,ah
	jne	L$14
	jmp	L$13
L$14:
;				ah <=> al
	xchg	ah,al
;				++si
	inc	si
;				gosub	CX_GET	//(di)
	call	L$15
;				go.w (ax != cx) RTS_NO
	cmp	ax,cx
	je	L$17
	jmp	L$16
L$17:
;				next.w
	jmp	L$9
;			fi
L$11:
;			ah = 0
	xor	ah,ah
;			if (al == '*' && (dl & 0x02) == 0)
	cmp	al,02Ah
	jne	L$18
	test	dl,02h
	jne	L$18
;				loop
L$19:
;					FIL_WcCmp_ ds.si, es.di, dx
	call	_FIL_WcCmp_
;					go.w (ax == _Y_) RTS_YES
	or	ax,ax
	jne	L$23
	jmp	L$22
L$23:
;					go.w (ax == _E_) RTS_ERR
	cmp	ax,(-1)
	jne	L$24
	jmp	L$13
L$24:
;					gosub	CX_GET	//(di)
	call	L$15
;				endloop (cx)
	or	cx,cx
	jne	L$19
;				go RTS_N
	jmp short	L$25
;			fi
L$18:
;			if (al == '?' && (dl & 0x01) == 0)
	cmp	al,03Fh
	jne	L$26
	test	dl,01h
	jne	L$26
;				gosub	CX_GET	//(di)
	call	L$15
;				//go (cx == '\0') RTS_N
;				go (cx == '\0' || cx == '.') RTS_N
	or	cx,cx
	je	L$25
	cmp	cx,02Eh
	je	L$25
;	
;			elsif (al == '^' && (dl & 0x08) == 0)
	jmp	L$27
L$26:
	cmp	al,05Eh
	jne	L$28
	test	dl,08h
	jne	L$28
;				gosub	AX_GET	//(si)
	call	L$29
;				go.w (ax == 0) RTS_ERR
	or	ax,ax
	jne	L$30
	jmp	L$13
L$30:
;				gosub	CX_GET	//(di)
	call	L$15
;				go (ax == cx) RTS_N
	cmp	ax,cx
	je	L$25
;	
;			elsif (al == '[' && (dl & 0x04) == 0)
	jmp	L$27
L$28:
	cmp	al,05Bh
	jne	L$31
	test	dl,04h
	jne	L$31
;				gosub	CX_GET	//(di)
	call	L$15
;				go (cx == 0) RTS_N
	jcxz	L$25
;				gosub	AX_GET	//(si)
	call	L$29
;				dh = 0
	xor	dh,dh
;				if (ax == '^')
	cmp	ax,05Eh
	jne	L$32
;					dh = 1
	mov	dh,01h
;					gosub	AX_GET	//(si)
	call	L$29
;				fi
L$32:
;				bx = 0xffff
	mov	bx,0FFFFh
;				go J0
	jmp short	L$33
;	  RTS_N:
L$25:
;				go RTS_NO
	jmp short	L$16
;	
;				loop
L$34:
;					if (ax == '-')
	cmp	ax,02Dh
	jne	L$37
;						gosub	AX_GET	//(si)
	call	L$29
;						go (ax == '\0') RTS_ERR
	or	ax,ax
	je	L$13
;						if (ax == ']')
	cmp	ax,05Dh
	jne	L$38
;							go (bx == cx) J1
	cmp	bx,cx
	je	L$39
;							go J2
	jmp short	L$40
;						fi
L$38:
;						go (cx >= bx && cx <= ax) J1
	cmp	cx,bx
	jb	L$41
	cmp	cx,ax
	jbe	L$39
L$41:
;					else
	jmp	L$42
L$37:
;	 J0:
L$33:
;						if (ax == cx)
	cmp	ax,cx
	jne	L$43
;	 J1:
L$39:
;							go (dh) RTS_NO
	or	dh,dh
	jne	L$16
;							loop
L$44:
;								gosub	AX_GET	//(si)
	call	L$29
;								go (ax == 0) RTS_ERR
	or	ax,ax
	je	L$13
;							endloop (ax != ']')
	cmp	ax,05Dh
	jne	L$44
;							exit
	jmp short	L$36
;						fi
L$43:
;					fi
L$42:
;					bx = ax
	mov	bx,ax
;					gosub	AX_GET	//(si)
	call	L$29
;					go (ax == '\0') RTS_ERR
	or	ax,ax
	je	L$13
;					if (ax == ']')
	cmp	ax,05Dh
	jne	L$47
;	 J2:
L$40:
;						exit (dh)
	or	dh,dh
	jne	L$36
;						go RTS_NO
	jmp short	L$16
;					fi
L$47:
;				endloop
	jmp short	L$34
L$36:
;	
;			else
	jmp	L$27
L$31:
;				gosub CX_GET	//(di)
	call	L$15
;	
;				if (ax != cx)
	cmp	ax,cx
	je	L$48
;					go (ax) RTS_NO
	or	ax,ax
	jne	L$16
;					go ((di -= sptr) == 0) RTS_NO
	sub	di,word ptr [bp-2]
	je	L$16
;					ax = di
	mov	ax,di
;					go RTS
	jmp short	L$49
;				fi
L$48:
;				go (cx == 0) RTS_YES
	jcxz	L$22
;			fi
L$27:
;		endloop// (ax)
L$9:
	jmp	L$8
;	 RTS_YES:
L$22:
;		ax = _Y_
	xor	ax,ax
;	 RTS:
L$49:
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	bx
	pop	dx
	pop	cx
	ret
;	
;	 RTS_ERR:
L$13:
;		ax = _E_
	mov	ax,(-1)
;		go RTS_NO2
	jmp short	L$50
;	
;	 RTS_NO:
L$16:
;		ax = _N_
	mov	ax,(-2)
;	 RTS_NO2:
L$50:
;		go RTS
	jmp short	L$49
;	
;	
;	CX_GET:	//(es.di)
L$15:
;	//	in	  es
;	//	out   cx,di
;	//	break fx
;	
;		ch = 0
	xor	ch,ch
;		cl = b[es:di]
	mov	cl,byte ptr es:[di]
;		++di
	inc	di
;		if ((dl & 0x10) == 0 && IsKanji(cl))
	test	dl,010h
	jne	L$51
	cmp	cl,081h
	jb	L$51
	cmp	cl,09Fh
	jbe	L$52
	cmp	cl,0E0h
	jb	L$51
	cmp	cl,0FCh
	ja	L$51
L$52:
;			ch = b[es:di]
	mov	ch,byte ptr es:[di]
;			++di
	inc	di
;			go (ch == 0) CX_GET_J1
	or	ch,ch
	je	L$53
;			ch <=> cl
	xchg	ch,cl
;		fi
L$51:
;		ret
	ret
;	  CX_GET_J1:
L$53:
;		cl = 0
	xor	cl,cl
;		ret
	ret
;	
;	AX_GET:	//(ds.si)
L$29:
;	//	in	  ds
;	//	out   ax,si,cf
;	//	break fx
;		ah = 0
	xor	ah,ah
;		al = b[si]
	mov	al,byte ptr [si]
;		++si
	inc	si
;		if ((dl & 0x10) == 0 && IsKanji(al))
	test	dl,010h
	jne	L$54
	cmp	al,081h
	jb	L$54
	cmp	al,09Fh
	jbe	L$55
	cmp	al,0E0h
	jb	L$54
	cmp	al,0FCh
	ja	L$54
L$55:
;			ah = b[si]
	mov	ah,byte ptr [si]
;			++si
	inc	si
;			go (ah == 0) AX_GET_J1
	or	ah,ah
	je	L$56
;			ah <=> al
	xchg	ah,al
;		fi
L$54:
;		ret
	ret
;	  AX_GET_J1:
L$56:
;		al = 0
	xor	al,al
;		ret
	ret
;	
;	endproc

;	
;	*proc FIL_WcCmp(key:dword, fstr:dword);cdecl;far
;		//out ax
;	enter
;		save cx,bx,dx,si,di,es,ds
;		ds = ax = %var
	public	_FIL_WcCmp
_FIL_WcCmp	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	cx
	push	dx
	push	bx
	push	si
	push	di
	push	ds
	push	es
	push	bp
	mov	bp,sp
	mov	ax,DGROUP
	mov	ds,ax
;		dx = FIL_wcFlg
	mov	dx,word ptr [_FIL_wcFlg]
;		dl &= 0x0f
	and	dl,0Fh
;		if (FIL_zenFlg == 0)
	cmp	word ptr [_FIL_zenFlg],00h
	jne	L$57
;			dl |= 0x10
	or	dl,010h
;		fi
L$57:
;		ds.si = key
	lds	si,dword ptr [bp+20]
;		es.di = fstr
	les	di,dword ptr [bp+24]
;		FIL_WcCmp_ ds.si, es.di, dx
	call	_FIL_WcCmp_
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	bx
	pop	dx
	pop	cx
	db	0CBh	;retf
;	endproc

;	
;	
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	
;	*proc FIL_BaseName(fnam:d);cdecl;far
;	enter
;		save si,ds
;		rep.inc
	public	_FIL_BaseName
_FIL_BaseName	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	si
	push	ds
	push	bp
	mov	bp,sp
	cld
;		ds.si = fnam
	lds	si,dword ptr [bp+10]
;		dx = si
	mov	dx,si
;		go J1
	jmp short	L$58
;		loop
L$59:
;			if (al == '/' || al == '\\' || al == ':')
	cmp	al,02Fh
	je	L$63
	cmp	al,05Ch
	je	L$63
	cmp	al,03Ah
	jne	L$62
L$63:
;				dx = si
	mov	dx,si
;				//++dx
;			fi
L$62:
;			if (IsKanji(al) && b[si])
	cmp	al,081h
	jb	L$64
	cmp	al,09Fh
	jbe	L$65
	cmp	al,0E0h
	jb	L$64
	cmp	al,0FCh
	ja	L$64
L$65:
	cmp	byte ptr [si],00h
	je	L$64
;				FIL_GetZenModeMACRO
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	mov	ax,word ptr [_FIL_zenFlg]
	pop	ds
;				if (ax)
	or	ax,ax
	je	L$66
;					++si
	inc	si
;				fi
L$66:
;			fi
L$64:
;	  J1:
L$58:
;			rep.load al,si
	lodsb
;		endloop (al != 0)
	or	al,al
	jne	L$59
;		ax = dx
	mov	ax,dx
;		dx = ds
	mov	dx,ds
;		return
	db	0C9h	;leave
	pop	ds
	pop	si
	db	0CBh	;retf
;	endproc

;	
;	
;	*proc FIL_ChgExt(filename:d, ext:d);cdecl;far
;	enter
;		save bx,cx,si,di,es,ds
;	
;		rep.inc
	public	_FIL_ChgExt
_FIL_ChgExt	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	cx
	push	bx
	push	si
	push	di
	push	ds
	push	es
	push	bp
	mov	bp,sp
	cld
;		dx.si = ext
	mov	si,word ptr [bp+22]
	mov	dx,word ptr [bp+24]
;		go (dx == 0 && si == 0) RET
	or	dx,dx
	jne	L$68
	or	si,si
	je	L$67
L$68:
;	  @if 0
;		ds = dx
;		go (b[si] == 0) RET
;	  @fi
;		FIL_BaseName filename
	push	word ptr [bp+20]
	push	word ptr [bp+18]
	call far ptr	_FIL_BaseName
	add	sp,04h
;		ds.si = dx.ax
	mov	si,ax
	mov	ds,dx
;		loop
L$69:
;			rep.load al,si
	lodsb
;			go (al == '.') PRIO
	cmp	al,02Eh
	je	L$72
;		endloop (al != 0)
	or	al,al
	jne	L$69
;		b[si-1] = '.'
	mov	byte ptr [si-01h],02Eh
;	  PRIO:
L$72:
;		STR_Cpy dx.si, ext
	push	word ptr [bp+24]
	push	word ptr [bp+22]
	push	dx
	push	si
	call	_STR_Cpy
;	  RET:
L$67:
;		dx.ax = filename
	mov	ax,word ptr [bp+18]
	mov	dx,word ptr [bp+20]
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	bx
	pop	cx
	db	0CBh	;retf
;	endproc

;	
;	*proc FIL_AddExt(filename:d, ext:d);cdecl;far
;	enter
;		save bx,cx,si,di,es,ds
;	
;		rep.inc
	public	_FIL_AddExt
_FIL_AddExt	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	cx
	push	bx
	push	si
	push	di
	push	ds
	push	es
	push	bp
	mov	bp,sp
	cld
;		dx.si = ext
	mov	si,word ptr [bp+22]
	mov	dx,word ptr [bp+24]
;		go (dx == 0 && si == 0) RET
	or	dx,dx
	jne	L$74
	or	si,si
	je	L$73
L$74:
;	  @if 0
;		ds = dx
;		go (b[si] == 0) RET
;	  @fi
;		FIL_BaseName filename
	push	word ptr [bp+20]
	push	word ptr [bp+18]
	call far ptr	_FIL_BaseName
	add	sp,04h
;		ds.si = dx.ax
	mov	si,ax
	mov	ds,dx
;		loop
L$75:
;			rep.load al,si
	lodsb
;			go (al == '.') RET
	cmp	al,02Eh
	je	L$73
;		endloop (al != 0)
	or	al,al
	jne	L$75
;		b[si-1] = '.'
	mov	byte ptr [si-01h],02Eh
;		b[si] = 0
	mov	byte ptr [si],00h
;		STR_Cpy dx.si, ext
	push	word ptr [bp+24]
	push	word ptr [bp+22]
	push	dx
	push	si
	call	_STR_Cpy
;	  RET:
L$73:
;		dx.ax = filename
	mov	ax,word ptr [bp+18]
	mov	dx,word ptr [bp+20]
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	bx
	pop	cx
	db	0CBh	;retf
;	endproc

;	
;	
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	
;	*proc FIL_FullPath(src:d, dst:d);cdecl;far
;		// ret dx.ax = dst
;	enter
;		save bx,cx,si,di,es,ds
;		local buf:b(520)	//はたして520バイトでいいのか^^;...
;	
;		rep.inc
	public	_FIL_FullPath
_FIL_FullPath	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	cx
	push	bx
	push	si
	push	di
	push	ds
	push	es
	enter	0208h,00h
	cld
;		ds.si = src
	lds	si,dword ptr [bp+18]
;		es.di = dst
	les	di,dword ptr [bp+22]
;	
;		/*
;		 * 絶対パスを得るためまずカレント・ドライブ＆ディレクトリ名を得る
;		 */
;		ax = w[si]
	mov	ax,word ptr [si]
;		if (al && ah == ':')	//ドライブ名があるときはそれを用いる
	or	al,al
	je	L$78
	cmp	ah,03Ah
	jne	L$78
;			if (al >= 'a' && al <= 'z')
	cmp	al,061h
	jb	L$79
	cmp	al,07Ah
	ja	L$79
;				al -= 0x20
	sub	al,020h
;			fi
L$79:
;			si += 2
	inc	si
	inc	si
;		else					//ドライブ名がなければDOSにカレントを聞く
	jmp	L$80
L$78:
;			ah = 0x19
	mov	ah,019h
;			intr 0x21
	int	021h
;			al += 'A'
	add	al,041h
;		fi
L$80:
;		dl = al		
	mov	dl,al
;		dl -= 'A'-1	//dl=ドライブ番号
	sub	dl,040h
;		ah = ':'
	mov	ah,03Ah
;		rep.set.w di,ax
	stosw
;	
;		//ルートが指定されているか
;		cx = &buf
	lea	cx,BYTE ptr [bp-520]
;		al = b[si]
	mov	al,byte ptr [si]
;		if (al == '\\' || al == '/')  //ルートなら名前をそのままbufにコピー
	cmp	al,05Ch
	je	L$82
	cmp	al,02Fh
	jne	L$81
L$82:
;			STR_Cpy ss.cx, ds.si
	push	ds
	push	si
	push	ss
	push	cx
	call	_STR_Cpy
;		else					//ﾙｰﾄでないならｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘをDOSに聞いて残りをｺﾋﾟｰ
	jmp	L$83
L$81:
;			dh = 0
	xor	dh,dh
;			//++dx
;			//fil.getcdir dx, ss.cx
;			push ds.si
	push	ds
	push	si
;				ds.si = ss.cx
	mov	si,cx
	push	ss
	pop	ds
;				b[si] = '/'; ++si
	mov	byte ptr [si],02Fh
	inc	si
;				ah = 0x47
	mov	ah,047h
;				intr 0x21
	int	021h
;				if (cf == 0)
	jc	L$84
;					ax = 0
	xor	ax,ax
;				fi
L$84:
;			pop  ds.si
	pop	si
	pop	ds
;	 //Pri.Fmt "cd %Fs\N", ss.cx
;			STR_End ss.cx
	push	ss
	push	cx
	call	_STR_End
;			push es.di
	push	es
	push	di
;				es.di = dx.ax
	mov	di,ax
	mov	es,dx
;				al = b[es:di-1]
	mov	al,byte ptr es:[di-01h]
;				if (al != '\\' && al != '/')
	cmp	al,05Ch
	je	L$85
	cmp	al,02Fh
	je	L$85
;					rep.set di,'/'
	mov	al,02Fh
	stosb
;				fi
L$85:
;				STR_Cpy es.di, ds.si
	push	ds
	push	si
	push	es
	push	di
	call	_STR_Cpy
;			pop  es.di 
	pop	di
	pop	es
;		fi
L$83:
;	
;	
;		/*
;		 * ｾﾊﾟﾚｰﾀ'\\' '/' を'/'にし、".." "."で不要ﾃﾞｨﾚｸﾄﾘ名文字列を削除
;		 * （なぜ'/'にするかといえば全角の２バイト目にならないから）
;		 */
;		ds = ss
	push	ss
	pop	ds
;		si = &buf
	lea	si,BYTE ptr [bp-520]
;	  //Pri.Fmt "%Fs\N", ds.si
;		//si += 2
;		dx = di		//dx:ルート位置(戻るときに戻り過ぎないように)
	mov	dx,di
;		bx = di		//bx:前回のセパレータ'\\''/'の位置を保存
	mov	bx,di
;		loop.w
L$86:
;		  LOOP:
L$89:
;			rep.load al,si
	lodsb
;			exit.w (al == 0)
	or	al,al
	jne	L$90
	jmp	L$88
L$90:
;			if (al == '\\' || al == '/')
	cmp	al,05Ch
	je	L$92
	cmp	al,02Fh
	jne	L$91
L$92:
;				bx = di
	mov	bx,di
;				rep.set di,'/'
	mov	al,02Fh
	stosb
;				//b[es:di] = 0
;				go LOOP
	jmp short	L$89
;			fi
L$91:
;			if (al == '.')
	cmp	al,02Eh
	jne	L$93
;				rep.load.w ax,si
	lodsw
;				if (al == '.' && (ah == '\\'||ah == '/'||ah == '\0'))
	cmp	al,02Eh
	jne	L$94
	cmp	ah,05Ch
	je	L$95
	cmp	ah,02Fh
	je	L$95
	or	ah,ah
	jne	L$94
L$95:
;					// '..'なら一つﾃﾞｨﾚｸﾄﾘ名を削る
;					if (ah == 0)
	or	ah,ah
	jne	L$96
;						--si
	dec	si
;					fi
L$96:
;					di = bx
	mov	di,bx
;					loop
L$97:
;						if (di <= dx)
	cmp	di,dx
	ja	L$100
;							di = bx
	mov	di,bx
;							go PP_1
	jmp short	L$101
;						fi
L$100:
;						--di
	dec	di
;						al = b[es:di]
	mov	al,byte ptr es:[di]
;					endloop (al != '/' /*&& al != '\\'*/)
	cmp	al,02Fh
	jne	L$97
;					bx = di
	mov	bx,di
;				  PP_1:
L$101:
;					++di
	inc	di
;					go LOOP
	jmp short	L$89
;	
;				elsif (al == '\\' || al == '/' || al == '\0')
	jmp	L$102
L$94:
	cmp	al,05Ch
	je	L$104
	cmp	al,02Fh
	je	L$104
	or	al,al
	jne	L$103
L$104:
;					// '.'だけなら'.'を省く
;					--si
	dec	si
;					if (al == 0)
	or	al,al
	jne	L$105
;						--si
	dec	si
;					fi
L$105:
;					di = bx
	mov	di,bx
;					++di
	inc	di
;					go LOOP
	jmp short	L$89
;				fi
L$103:
L$102:
;				//'.'で始まるファイル名だ^^;
;				si -= 2
	dec	si
	dec	si
;			fi
L$93:
;			--si
	dec	si
;			loop
L$106:
;				rep.load al,si
	lodsb
;				--si
	dec	si
;				exit (al == '\0' || al == '\\' || al == '/')
	or	al,al
	je	L$108
	cmp	al,05Ch
	je	L$108
	cmp	al,02Fh
	je	L$108
;				FIL_GetZenModeMACRO	//全角有効モードなら全角チェック
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	mov	ax,word ptr [_FIL_zenFlg]
	pop	ds
;				if (ax)
	or	ax,ax
	je	L$109
;					if (b[si] && IsKanji(al))
	cmp	byte ptr [si],00h
	je	L$110
	cmp	al,081h
	jb	L$110
	cmp	al,09Fh
	jbe	L$111
	cmp	al,0E0h
	jb	L$110
	cmp	al,0FCh
	ja	L$110
L$111:
;						rep.cpy di,si
	movsb
;					fi
L$110:
;				fi
L$109:
;				rep.cpy di,si
	movsb
;			endloop
	jmp short	L$106
L$108:
;		endloop
	jmp	L$86
L$88:
;		rep.set di,0
	xor	al,al
	stosb
;	
;		/*
;		 * '/' を '\\' に置き換え、英小文字を大文字化
;		 */
;		es.di = dst
	les	di,dword ptr [bp+22]
;		loop
L$112:
;		  L2:
L$115:
;			al = b[es:di]
	mov	al,byte ptr es:[di]
;			exit (al == 0)
	or	al,al
	je	L$114
;			if (al == '/')
	cmp	al,02Fh
	jne	L$116
;				rep.set di,'\\'
	mov	al,05Ch
	stosb
;				go L2
	jmp short	L$115
;			elsif (al >= 'a' && al <= 'z')
L$116:
	cmp	al,061h
	jb	L$118
	cmp	al,07Ah
	ja	L$118
;				al -= 'a' - 'A'
	sub	al,020h
;				rep.set di,al
	stosb
;				go L2
	jmp short	L$115
;			else
L$118:
;				++di
	inc	di
;				FIL_GetZenModeMACRO	//全角有効モードなら全角チェック
	push	ds
	mov	ax,DGROUP
	mov	ds,ax
	mov	ax,word ptr [_FIL_zenFlg]
	pop	ds
;				go (ax == 0) L2
	or	ax,ax
	je	L$115
;				if (b[es:di] && IsKanji(al))
	cmp	byte ptr es:[di],00h
	je	L$119
	cmp	al,081h
	jb	L$119
	cmp	al,09Fh
	jbe	L$120
	cmp	al,0E0h
	jb	L$119
	cmp	al,0FCh
	ja	L$119
L$120:
;					++di
	inc	di
;				fi
L$119:
;			fi
L$117:
;		endloop
	jmp short	L$112
L$114:
;	
;		dx.ax = dst
	mov	ax,word ptr [bp+22]
	mov	dx,word ptr [bp+24]
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	bx
	pop	cx
	db	0CBh	;retf
;	endproc

;	
;	
;	
;	
;	
;	
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	proc DOS_GetDTA()
;	//	DTA のアドレスを得る
;	//	out   es.bx
;	//	break ah
;	macro
;		ah = 0x2f
;		intr 0x21
;	endproc
;	
;	proc DOS_SetDTA(ds.dx)
;	//	DTA のアドレスを変更する
;	//	in	ds
;	//	break ah
;	macro
;		ah = 0x1a
;		intr 0x21
;	endproc
;	
;	proc DOS_FindFirst(ds.dx, cx)
;	//	out dx,cx,ax,cf
;	macro
;		ah = 0x4e
;		intr 0x21
;	endproc
;	
;	proc DOS_FindNext()
;	//	out ax,cf
;	macro
;		ah = 0x4f
;		intr 0x21
;	endproc
;	
;	struct FSLOT
;		drv:b
;		name:b(8)
;		ext:b(3)
;		attr:b
;		count:w
;		cluster:w
;		rsv:b(4)
;		fattr:b
;		ftime:w
;		fdate:w
;		fsize:d
;		fname:b(13)
;		/**/
;		dmy:b
;		srchname:b(14)
;	endstruct
;	
;	*proc FIL_FindFirst(fnam:d, atr:w, fslot:d);cdecl;far
;	enter
;		save	bx,cx,dx,si,di,es,ds
;		//local	buf:b(280)	/* // */
;		local	savDta:d
;		cvar	srchall:b() = "*.*"
L$121	label	byte
	db	42,46,42
	db	1 dup(0)
;	
;		rep.inc
	public	_FIL_FindFirst
_FIL_FindFirst	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	cx
	push	dx
	push	bx
	push	si
	push	di
	push	ds
	push	es
	enter	04h,00h
	cld
;	
;		// DTA アドレスを待避
;		DOS_GetDTA
	mov	ah,02Fh
	int	021h
;		savDta = es.bx
	mov	word ptr [bp-4],bx
	mov	word ptr [bp-2],es
;	
;		// 一時的に DTA を fslot に変更
;		ax.dx = fslot
	mov	dx,word ptr [bp+26]
	mov	ax,word ptr [bp+28]
;		go.w (ax == 0 && dx == 0) ERR_ARG
	or	ax,ax
	jne	L$123
	or	dx,dx
	jne	L$123
	jmp	L$122
L$123:
;		DOS_SetDTA ax.dx
	mov	ds,ax
	mov	ah,01Ah
	int	021h
;	
;		// fnam を 検索文字列 fslot.srchname と ディレクトリ buf に分け、
;		// bufの後ろを"*.*" にする。
;		dx.ax = fnam
	mov	ax,word ptr [bp+20]
	mov	dx,word ptr [bp+22]
;		go.w (ax == 0 && dx == 0) ERR_ARG
	or	ax,ax
	jne	L$124
	or	dx,dx
	jne	L$124
	jmp	L$122
L$124:
;		 //D Pri.Fmt "1 %08lx %Fs\N", dx.ax, dx.ax
;		sp -= 280; bx = sp 		/* // bx = &buf */
	sub	sp,0118h
	mov	bx,sp
;		STR_CpyUpr ss.bx, dx.ax
	push	dx
	push	ax
	push	ss
	push	bx
	call	_STR_CpyUpr
;		 //D Pri.Fmt "2 %08lx %Fs\N", ss.bx, ss.bx
;		FIL_BaseName ss.bx
	push	ss
	push	bx
	call far ptr	_FIL_BaseName
	add	sp,04h
;		es.di = fslot
	les	di,dword ptr [bp+26]
;		di += %FSLOT.srchname
	add	di,02Ch
;		// 検索ファイル名の最後が'*.*'ならば'*'に変換
;		// 		'.'がふくまれていなければ '.'を付加
;		push dx.ax
	push	dx
	push	ax
;			ds.si = dx.ax
	mov	si,ax
	mov	ds,dx
;			cx = 0
	xor	cx,cx
;			dh = 1
	mov	dh,01h
;			loop
L$125:
;				rep.load al,si
	lodsb
;				rep.set  di,al
	stosb
;				++cx
	inc	cx
;				if (al == '.')
	cmp	al,02Eh
	jne	L$128
;					dh = 0
	xor	dh,dh
;				fi
L$128:
;			endloop (al != 0)
	or	al,al
	jne	L$125
;		  @if 0
;			if (cx > 3 && w[es:di-4] == '*' + '.' * 0x100  &&  b[es:di-2] == '*')
;				w[es:di-4] = '*' + '\0' * 0x100
;		  	fi
;		  @endif
;		  @if 0
;			if (cx > 1 && b[es:di-2] == '.')
;				b[es:di-2] = '\0'
;			fi
;		  @else
;			if (cx > 0 && dh && b[es:di-2] != '*')
	or	cx,cx
	jbe	L$129
	or	dh,dh
	je	L$129
	cmp	byte ptr es:[di-02h],02Ah
	je	L$129
;				w[es:di-1] = '.' + '\0' * 0x100
	mov	word ptr es:[di-01h],02Eh
;			fi
L$129:
;		  @fi
;		pop  dx.ax
	pop	ax
	pop	dx
;		STR_Cpy dx.ax, ww(cs, &srchall)
	push	cs
	push	offset _TEXT:L$121
	push	dx
	push	ax
	call	_STR_Cpy
;		 //D Pri.Fmt "4 %08lx %Fs\N", dx.ax, dx.ax
;	
;		// ファイルさーち
;		 //D Pri.Fmt "5 %08lx %Fs\N", ss.bx, ss.bx
;		DOS_FindFirst ss.bx, atr
	mov	cx,word ptr [bp+24]
	mov	dx,bx
	push	ss
	pop	ds
	mov	ah,04Eh
	int	021h
;		go.w (cf) ERR_FIND
	jnc	L$131
	jmp	L$130
L$131:
;		sp += 280				/* // */
	add	sp,0118h
;	
;		// ワイルドカード文字対応でファイル名比較
;		ds.cx = fslot
	lds	cx,dword ptr [bp+26]
;		di = cx
	mov	di,cx
;		di += %FSLOT.srchname
	add	di,02Ch
;		cx += %FSLOT.fname
	add	cx,01Eh
;		dx = ds
	mov	dx,ds
;		 //D Pri.Fmt "6 %08lx %Fs\N", dx.cx, dx.cx
;		 //D Pri.Fmt "7 %08lx %Fs\N", dx.di, dx.di
;		go J1
	jmp short	L$132
;		loop
L$133:
;			DOS_FindNext
	mov	ah,04Fh
	int	021h
;			go (cf) RET
	jc	L$136
;	  J1:
L$132:
;			 //D Pri.Fmt "8 %Fs(%08lx) vs %Fs(%08lx)\N", ds.di, ds.di, ds.cx, ds.cx
;			// 拡張子無しファイルなら、とりあえず '.'を付加
;			bx = 0
	xor	bx,bx
;		  @if 1
;			si = cx
	mov	si,cx
;			loop
L$137:
;				rep.load al,si
	lodsb
;				if (al == 0)
	or	al,al
	jne	L$140
;					bx = si
	mov	bx,si
;					--bx
	dec	bx
;					w[bx] = '.' + '\0' * 0x100
	mov	word ptr [bx],02Eh
;					exit
	jmp short	L$139
;				fi
L$140:
;			endloop (al != '.')
	cmp	al,02Eh
	jne	L$137
L$139:
;		  @endif
;			FIL_WcCmp ds.di, ds.cx
	push	ds
	push	cx
	push	ds
	push	di
	call far ptr	_FIL_WcCmp
	add	sp,08h
;		endloop (ax)
	or	ax,ax
	jne	L$133
;	
;		// 拡張子無しファイル名につけた'.'は取り外す
;		if (bx)
	or	bx,bx
	je	L$141
;			b[bx] = 0
	mov	byte ptr [bx],00h
;		fi
L$141:
;	
;		//ax = 0		//ここを通るときは、すでに必ず ax = 0だ
;	  RET:
L$136:
;		push ax
	push	ax
;			DOS_SetDTA savDta
	lds	dx,dword ptr [bp-4]
	mov	ah,01Ah
	int	021h
;		pop  ax
	pop	ax
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	bx
	pop	dx
	pop	cx
	db	0CBh	;retf
;	
;	 ERR_ARG:
L$122:
;		ax = -1
	mov	ax,(-1)
;		go RET
	jmp short	L$136
;	
;	 ERR_FIND:
L$130:
;		sp += 280				/* // */
	add	sp,0118h
;		go RET
	jmp short	L$136
;	
;	endproc

;	
;	
;	*proc FIL_FindNext(fslot:d);cdecl;far
;	enter
;		save bx,cx,si,di,es,ds
;		local savDta:d
;	
;		rep.inc
	public	_FIL_FindNext
_FIL_FindNext	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	push	cx
	push	bx
	push	si
	push	di
	push	ds
	push	es
	enter	04h,00h
	cld
;		// DTA アドレスを待避
;		DOS_GetDTA
	mov	ah,02Fh
	int	021h
;		savDta = es.bx
	mov	word ptr [bp-4],bx
	mov	word ptr [bp-2],es
;	
;		// 一時的に DTA を fslot に変更
;		ax.dx = fslot
	mov	dx,word ptr [bp+18]
	mov	ax,word ptr [bp+20]
;		go (ax == 0 && dx == 0) ERR_ARG
	or	ax,ax
	jne	L$143
	or	dx,dx
	je	L$142
L$143:
;		DOS_SetDTA ax.dx
	mov	ds,ax
	mov	ah,01Ah
	int	021h
;	
;		// ワイルドカード文字対応でファイル名比較
;		ds.cx = fslot
	lds	cx,dword ptr [bp+18]
;		di = cx
	mov	di,cx
;		cx += %FSLOT.fname
	add	cx,01Eh
;		di += %FSLOT.srchname
	add	di,02Ch
;		dx = ds
	mov	dx,ds
;		 // Pri.Fmt "9 %08lx %Fs\N", ds.cx, ds.cx
;		 // Pri.Fmt "a %08lx %Fs\N", ds.di, ds.di
;		loop
L$144:
;			DOS_FindNext
	mov	ah,04Fh
	int	021h
;			go (cf) RET
	jc	L$147
;			 // Pri.Fmt "b %Fs(%08lx) vs %Fs(%08lx)\N", ds.di, ds.di, ds.cx, ds.cx
;			// 拡張子無しファイルなら、とりあえず '.'を付加
;			bx = 0
	xor	bx,bx
;		  @if 1
;			si = cx
	mov	si,cx
;			loop
L$148:
;				rep.load al,si
	lodsb
;				if (al == 0)
	or	al,al
	jne	L$151
;					bx = si
	mov	bx,si
;					--bx
	dec	bx
;					w[bx] = '.' + '\0' * 0x100
	mov	word ptr [bx],02Eh
;					exit
	jmp short	L$150
;				fi
L$151:
;			endloop (al != '.')
	cmp	al,02Eh
	jne	L$148
L$150:
;		  @endif
;			FIL_WcCmp ds.di, ds.cx
	push	ds
	push	cx
	push	ds
	push	di
	call far ptr	_FIL_WcCmp
	add	sp,08h
;		endloop (ax)
	or	ax,ax
	jne	L$144
;	
;		// 拡張子無しファイル名につけた'.'は取り外す
;		if (bx)
	or	bx,bx
	je	L$152
;			b[bx] = 0
	mov	byte ptr [bx],00h
;		fi
L$152:
;	
;		//ax = 0		//ここを通るときは、すでに必ず ax = 0だ
;	  RET:
L$147:
;		push ax
	push	ax
;			DOS_SetDTA savDta
	lds	dx,dword ptr [bp-4]
	mov	ah,01Ah
	int	021h
;		pop  ax
	pop	ax
;		return
	db	0C9h	;leave
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	bx
	pop	cx
	db	0CBh	;retf
;	
;	 ERR_ARG:
L$142:
;		ax = -1
	mov	ax,(-1)
;		go RET
	jmp short	L$147
;	
;	endproc

;	
;	
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	/*---------------------------------------------------------------------------*/
;	
;	*proc DOS_KbdIn();cdecl;far
;	begin
;		ah = 0x08
	public	_DOS_KbdIn
_DOS_KbdIn	label	far
	assume	ds:DGROUP,es:NOTHING,ss:DGROUP
	mov	ah,08h
;		intr 0x21
	int	021h
;		ah = 0
	xor	ah,ah
;		return
	db	0CBh	;retf
;	endproc

;	
;	endmodule
_TEXT	ends
	end
