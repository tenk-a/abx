/*
	AVL–Ø
	1991-1993	M.Kitamura
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

#if 0
  #define MSGF(x)	(printf x)
#else
  #define MSGF(x)
#endif

/*---------------------------------------------------------------------------*/

static TREE_NODE *curNode;
static void *elem;

static TREE_CMP cmpElement;
static TREE_DEL delElement;
static TREE_NEW newElement;
static TREE_MALLOC funcMalloc;
static int	newElementFlg;

TREE *TREE_Make(TREE_NEW newElement,TREE_DEL delElement,TREE_CMP cmpElement, void *(*funcMalloc)(unsigned))
	/* “ñ•ª–Ø‚ðì¬‚µ‚Ü‚·Bˆø”‚ÍA—v‘f‚Ìì¬,íœ,”äŠr‚Ì‚½‚ß‚ÌŠÖ”‚Ö‚ÌÎß²ÝÀ*/
{
	TREE *p;

	if (funcMalloc == NULL)
		return NULL;
	p = funcMalloc(sizeof(TREE));
	if (p) {
		p->root = NULL;
		p->node = NULL;
		p->flag = 0;
		p->newElement  = newElement;
		p->delElement  = delElement;
		p->cmpElement  = cmpElement;
		p->malloc      = funcMalloc;
	}
	return p;
}


static TREE_NODE *NewNode(void)
{
	TREE_NODE *sp;

	sp = funcMalloc(sizeof(TREE_NODE));
	if (sp) {
		memset(sp,0x00,sizeof(TREE_NODE));
	  /*
		sp->link[0] = sp->link[1] = NULL;
		sp->avltFlg = 0;
		sp->element = NULL;
	  */
	}
	return sp;
}

static int InsertNode(TREE_NODE *pp, int  lrFlg)
{
	int l,r,nl,nr;
	int  a;
	TREE_NODE *p,*lp,*lrp,*lrlp,*lrrp;

 MSGF(("pp=%p  ",pp));
	if (pp == NULL)
		return 0;
	p = pp->link[lrFlg];
 MSGF(("p=%p  ",p));
	if (p == NULL) {
		curNode = p = pp->link[lrFlg] = NewNode();
		newElementFlg = 1;
		p->element = newElement(elem);
 MSGF(("elem=%p\n",p->element));
		return 3;
	}
	a = cmpElement(elem,p->element);
 MSGF(("a=%d\n",a));
	if (a == 0) {
		curNode = p;
		return 0;
	}
	if (a < 0) {
		l = 0;	r = 1;	nl = 1; nr = 2;
	} else {
		l = 1;	r = 0;	nl = 2; nr = 1;
	}

	a = InsertNode(p,l);
	if (a == 0) {
		return 0;
	}
	if (p->avltFlg == nr) {
		p->avltFlg = 0;
		return 0;
	} else if (p->avltFlg == 0) {
		p->avltFlg = nl;
		return p->avltFlg;
	}
	if (a == nl) {
		lp = p->link[l];
		lrp = lp->link[r];
		p->link[l] = lrp;
		p->avltFlg = 0;
		lp->link[r] = p;
		lp->avltFlg = 0;
		pp->link[lrFlg] = lp;
	} else if (a == nr) {
		lp = p->link[l];
		lrp = lp->link[r];
		lrlp = lrp->link[l];
		lrrp = lrp->link[r];
		pp->link[lrFlg] = lrp;
		p->link[l]	 = lrrp;
		lp->link[r]  = lrlp;
		lrp->link[l] = lp;
		lrp->link[r] = p;
		lp->avltFlg = p->avltFlg = 0;
		if (lrp->avltFlg == nl)
			p->avltFlg = nr;
		else if (lrp->avltFlg == nr)
			lp->avltFlg = nl;
		lrp->avltFlg = 0;
	} else {
		printf("PRGERR:InsertNode avltFlg = 3\n");
		exit(1);
	}
	return 0;
}


void *TREE_Insert(TREE *tree, void *e)
	/* —v‘f‚ð–Ø‚É‘}“ü */
{
	static TREE_NODE tmp;

	funcMalloc  = tree->malloc;
	cmpElement	= tree->cmpElement;
	newElement	= tree->newElement;
	tmp.link[0] = tree->root;
	newElementFlg = 0;
	curNode = NULL;
	elem = e;
	InsertNode(&tmp, 0);
	tree->root = tmp.link[0];
	tree->node = curNode;
	tree->flag = newElementFlg;
	if (curNode)
		return curNode->element;
	return NULL;
}


/*----------------------------------*/
static TREE_NODE *SearchElement(TREE_NODE *np, void *p)
{
	int n;

	if (np == NULL)
		return NULL;
	if ((n = cmpElement(p,np->element)) == 0)
		return np;
	if (n < 0 && np->link[0])
		return SearchElement(np->link[0],p);
	else if (np->link[1])
		return SearchElement(np->link[1],p);
	return NULL;
}

void *TREE_Search(TREE *tree, void *p)
	/* –Ø‚©‚ç—v‘f‚ð’T‚· */
{
	TREE_NODE *np;

	cmpElement	= tree->cmpElement;
	tree->node = np = SearchElement(tree->root, p);
	if (np == NULL)
		return NULL;
	return np->element;
}


/*----------------------------------*/
static void DelAllNode(TREE_NODE *np)
{
	if (np == NULL)
		return;
	if (np->link[0])
		DelAllNode(np->link[0]);
	if (np->link[1])
		DelAllNode(np->link[1]);
	if (delElement)
		delElement(np->element);
	free(np);
	return;
}

void TREE_Clear(TREE *tree)
	/* –Ø‚ðÁ‹Ž‚·‚é */
{
	delElement	= tree->delElement;
	DelAllNode(tree->root);
	free(tree);
	return;
}

/*---------------------------------------------------------------------------*/
#if 1

static void DoElement(TREE_NODE *np, void (*DoElem)(void *))
{
	if (np == NULL)
		return;
	if (np->link[0])
		DoElement(np->link[0],DoElem);
	DoElem(np->element);
	if (np->link[1])
		DoElement(np->link[1],DoElem);
	return;
}

void TREE_DoAll(TREE *tree, void (*func)(void *))
	/* –Ø‚Ì‚·‚×‚Ä‚Ì—v‘f‚É‚Â‚¢‚Ä func(void *) ‚ðŽÀs. 
		func‚É‚Í—v‘f‚Ö‚Ìƒ|ƒCƒ“ƒ^‚ª“n‚³‚ê‚é */
{
	DoElement(tree->root,func);
}

#else
static void (*DoElem)(void *);

static void DoElement(TREE_NODE *np)
{
	if (np == NULL)
		return;
	if (np->link[0])
		DoElement(np->link[0]);
	DoElem(np->element);
	if (np->link[1])
		DoElement(np->link[1]);
	return;
}

void TREE_DoAll(TREE *tree, void (*func)(void *))
	/* –Ø‚Ì‚·‚×‚Ä‚Ì—v‘f‚É‚Â‚¢‚Ä func(void *) ‚ðŽÀs. 
		func‚É‚Í—v‘f‚Ö‚Ìƒ|ƒCƒ“ƒ^‚ª“n‚³‚ê‚é */
{
	DoElem = func;
	DoElement(tree->root);
}
#endif



#if 0
/*---------------------------------------------------------------------------*/
static TREE_NODE *listCur;
static TREE_NODE *listTop;

static TREE_NODE *Tree2dlist_sub(TREE_NODE *dp)
{
	if (dp == NULL)
		return NULL;
	if (dp->link[0] == NULL && listTop == NULL) {
		listCur = listTop = dp;
	} else {
		if (dp->link[0])
			Tree2dlist_sub(dp->link[0]);
		if (listTop == NULL) {
			printf("PRGERR:tree2list ‚ª‚¨‚©‚µ‚¢‚¼!");
			exit(1);
		}
		listCur->link[1] = dp;
		dp->link[0] = listCur;
		listCur = dp;
	}
	if (dp->link[1])
		Tree2dlist_sub(dp->link[1]);
	return dp;
}

void TREE_ToDList(TREE *tp)
	/* “ñ•ª–Ø‚ð‘o•ûŒüƒŠƒXƒg‚É•ÏŠ· */
{
	listTop = NULL;
	listCur = NULL;
	Tree2dlist_sub(tp->root);
	tp->node = tp->root = listTop;
}

void *TREE_DListFirst(TREE *tp)
	/* ‘o•ûŒüƒŠƒXƒg‚Ìæ“ª‚ÉƒJ[ƒ\ƒ‹‚ðˆÚ‚· */
{
	tp->node = tp->root;
	if (tp->node)
		return tp->node->element;
	return NULL;
}

void *TREE_DListNext(TREE *tp)
	/* ŽŸ‚Ì—v‘f‚ÖˆÚ“® */
{
	if (tp->node) {
		tp->node = tp->node->link[1];
		if (tp->node)
			return tp->node->element;
	}
	return NULL;
}

void TREE_DListClear(TREE *tp)
	/* ‘o•ûŒüƒŠƒXƒg(‚à‚Æ‚à‚Æ‚Í–Øj‚ðÁ‹Ž */
{
	TREE_NODE *p;
	TREE_NODE *q;

	p = tp->root;
	while (p) {
		q = p->link[1];
		if (delElement)
			delElement(p->element);
		free(p);
		p = q;
	}
	free(tp);
}


/*---------------------------------------------------------------------------*/
#endif
