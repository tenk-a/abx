typedef struct TREE_NODE {
	struct TREE_NODE *link[2];
	int   avltFlg;
	void *element;
} TREE_NODE;

typedef void *(*TREE_NEW)(void *);
typedef void (*TREE_DEL)(void *);
typedef int  (*TREE_CMP)(void *,void *);
typedef void *(*TREE_MALLOC)(unsigned);

typedef struct TREE {
	TREE_NODE *root;
	TREE_NODE *node;
	int       flag;
	TREE_NEW  newElement;
	TREE_DEL  delElement;
	TREE_CMP  cmpElement;
	TREE_MALLOC malloc;
} TREE;

TREE *TREE_Make(TREE_NEW newElement,TREE_DEL delElement,TREE_CMP cmpElement, TREE_MALLOC funcMalloc);
void *TREE_Insert(TREE *tree, void *e);
void *TREE_Search(TREE *tree, void *p);
void TREE_Clear(TREE *tree);
void TREE_DoAll(TREE *tree, void (*func)(void *));

void TREE_ToDList(TREE *tp);
void *TREE_DListFirst(TREE *tp);
void *TREE_DListNext(TREE *tp);
void TREE_DListClear(TREE *tp);
