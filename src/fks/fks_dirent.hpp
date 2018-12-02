

#ifdef __cplusplus
template<class CB, class MF=void, class MD=void>
fks_isize_t fks_foreachDirEntries(Fks_DirEntries* dirEntries, CB& cb, Fks_DirEnt_Matchs* mt = NULL)
{
 #ifdef FKS_ARG_PTR_ASSERT
    FKS_ARG_PTR_ASSERT(1, dirEntries);
 #endif
    if (!dirEntries)
        return -2;
	Fks_DirEnt_Matchs	mtBuf = {0};
    fks_isize_t         cnt     = 0;
    fks_isize_t         n       = dirEntries->size;
    Fks_DirEnt const*   entries = dirEntries->entries;
    char const*         dirPath = dirEntries->path;
	if (mt == NULL)
		mt = &mtBuf;
	unsigned int flags = mt->flags;
    for (fks_isize_t i = 0; i < n; ++i) {
        Fks_DirEnt const* d = &entries[i];
        if (!(flags & FKS_DE_DotOrDotDot)) {
	        if (d->name[0] == '.' && (d->name[1] == 0 || (d->name[1] == '.' && d->name[2] == 0)))
	            continue;
		}
        if (FKS_DE_IsDirOnly(flags) && !FKS_S_ISDIR(d->stat->st_mode))
            continue;
        if (FKS_DE_IsFileOnly(flags) && FKS_S_ISDIR(d->stat->st_mode))
            continue;
        if (mt->isMatch && mt->isMatch(mt->isMatchData, d) == 0)
            goto NEXT;
        if (cb(d, dirPath) == 0)
            return -1;  // foreach break
        ++cnt;
     NEXT:
        if (d->sub) {
	        if (d->name[0] == '.' && (d->name[1] == 0 || (d->name[1] == '.' && d->name[2] == 0)))
	            continue;
	        if (mt->isDirMatch && mt->isDirMatch(mt->isDirMatchData, d) == 0)
	            continue;
            fks_isize_t cnt2 = fks_foreachDirEntries(d->sub, cb, mt);
            if (cnt2 < 0)
                return cnt2;    // foreach break
            cnt += cnt2;
        }
    }
    return cnt; // foreach continue
}
#endif
