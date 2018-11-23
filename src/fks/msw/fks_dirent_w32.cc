/*
 *  @file   fks_dirent_w32.cc
 *  @brief  
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */

#include <fks/fks_dirent.h>
#include <fks/fks_malloc.h>
#include <fks/fks_assert_ex.h>
#include <fks/fks_path.h>
#include <string.h>
#include <windows.h>
#include <fks/msw/fks_io_priv_w32.h>

FKS_INLINE_DECL (void)
fks_getDirEntFromWin32(Fks_DirEnt* d, WIN32_FIND_DATA const* s)
{
	FKS_ARG_PTR_ASSERT(1, d);
	FKS_ARG_PTR_ASSERT(2, s);
	d->st.st_size 	= ((uint64_t)s->nFileSizeHigh << 32) | s->nFileSizeLow;
	d->st.st_ctime 	= FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &s->ftCreationTime ) );
	d->st.st_atime 	= FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &s->ftLastAccessTime ) );
	d->st.st_mtime 	= FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &s->ftLastWriteTime  ) );
	d->st.st_mode  	= FKS_W32FATTER_TO_STATMODE( s->dwFileAttributes );
	d->st.st_native_attr = s->dwFileAttributes;
	d->name         = s->cFileName;
	d->sub			= NULL;
}


FKS_LIB_DECL (Fks_DirEntries*)
fks_getDirEntries1(Fks_DirEntries* dirEntries, char const* dirName, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	struct LinkData {
		LinkData*		link;
		WIN32_FIND_DATA	data;
	};
	HANDLE				hdl;
	size_t				strsz;
	size_t				entSz;
	size_t				n;
	size_t				l;
	char*				buf;
	char*				p;
	Fks_DirEnt*			d;
	LinkData*			t;
	LinkData			root = { 0 };
    WIN32_FIND_DATA		findData = { 0 };
    char				srchPath[ FKS_PATH_MAX_URL ];

	FKS_ARG_PTR_ASSERT(1, dirEntries);
	FKS_ARG_PTR_ASSERT(2, dirName);
	if (!dirEntries)
		return NULL;
	memset(dirEntries,0, sizeof *dirEntries);
	if (!dirName)
		return NULL;
	fks_pathJoin(srchPath, sizeof srchPath, dirName, "*.*");
    hdl = FindFirstFile(srchPath, &findData);
    if (hdl == INVALID_HANDLE_VALUE)
    	return NULL;
    strsz = strlen(dirName) + 1;

	// pool WIN32_FIND_DATA data
	t    = &root;
	n    = 0;
    do {
		if (!(flags & FKS_DE_DotAndDotDot) && (!strcmp(findData.cFileName, ".") || !strcmp(findData.cFileName, "..")))
			continue;
		if ((flags & FKS_DE_DirOnly) && !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;
		if ((flags & FKS_DE_FileOnly) && (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;
		if (isMatch) {
			Fks_DirEnt dent;
			fks_getDirEntFromWin32(&dent, &findData);
			if (isMatch(&dent) == 0)
				continue;
		}
		t->link = (LinkData*)fks_calloc(1, sizeof(LinkData));
		if (t == NULL) {
			dirEntries = NULL;
			goto ERR;
		}
		t       = t->link;
		t->data = findData;
		strsz += strlen(findData.cFileName) + 1;
		++n;
    } while (FindNextFile(hdl, &findData) != 0);
    FindClose(hdl);

	//
    entSz = sizeof(Fks_DirEnt) * n;
	buf   = (char*)fks_calloc(1, entSz + strsz);
	if (buf == NULL) {
		dirEntries = NULL;
		goto ERR;
	}
	d		 			= (Fks_DirEnt*)buf;
	dirEntries->entries = d;
	p     				= buf + entSz;
	l					= strlen(dirName) + 1;
	memcpy(p, dirName, l);
	dirEntries->name    = p;
	p += l;
	for (t = root.link; t->link; t = t->link) {
		WIN32_FIND_DATA* s = &t->data;
		fks_getDirEntFromWin32(d, s);
		l	= strlen(s->cFileName) + 1;
		memcpy(p, dirName, l);
		d->name = p;
		p  += l;
		++d;
	}
	dirEntries->size    = n;

 ERR:
	// remove work table
	t = root.link;
	do {
		LinkData* u = t->link;
		fks_free(t);
		t = u;
	} while (t);
    return dirEntries;
}