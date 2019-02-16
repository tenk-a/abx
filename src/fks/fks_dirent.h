/*
 *  @file   fks_dirent.h
 *  @brief
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_DIRENTRY_H_INCLUDED
#define FKS_DIRENTRY_H_INCLUDED

#include <fks_io.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    FKS_DE_ReadOnly     = 0x01,         // Read Only
    FKS_DE_Hidden       = 0x04,         // Add Hidden file
    FKS_DE_DotOrDotDot  = 0x08,         // Add "." ".."
    FKS_DE_File         = 0x10,         // File
    FKS_DE_Dir          = 0x20,         // Directory
    FKS_DE_Recursive    = 0x80,         // Recursive directory
    //FKS_DE_ErrCont    = 0x8000,       // Continue even if an error occurs. Do not delete the created part.
    FKS_DE_Tiny         = 0x1000000,    // For linux (readdir(), not use stat())
    FKS_DE_NameStat     = 0x2000000,    // fks_createDirEntPaths: char** -> Fks_DE_NameStat*    (fks_countDirEntries)
};
#define FKS_DE_IsDirOnly(f)     (((f) & (FKS_DE_Dir|FKS_DE_File)) == FKS_DE_Dir)
#define FKS_DE_IsFileOnly(f)    (((f) & (FKS_DE_Dir|FKS_DE_File)) == FKS_DE_File)
#define FKS_DE_IsDotOrDotDot(d) ((d)[0] == '.' && ((d)[1] == 0 || ((d)[1] == '.' && (d)[2] == 0)))

typedef struct Fks_DirEnt {
    char const*             name;
    fks_stat_t*             stat;
    struct Fks_DirEntries*  sub;
} Fks_DirEnt;

typedef struct Fks_DirEntries {
    Fks_DirEnt const*       entries;
    fks_isize_t             size;       // >=0:entries count  <0: has error.
    char const*             path;
} Fks_DirEntries;

typedef struct Fks_DirEntPathStat {
    char const*     path;
    fks_stat_t*     stat;
} Fks_DirEntPathStat;

typedef int (*Fks_DirEnt_IsMatchCB)(void*, Fks_DirEnt const*);
typedef int (*Fks_ForeachDirEntCB)(void* data, Fks_DirEnt const* dirEnt, char const* dirPath);

typedef struct Fks_DirEnt_Matchs {
    char const*             fname;
    unsigned int            flags;
    Fks_DirEnt_IsMatchCB    isMatch;
    void*                   isMatchData;
    Fks_DirEnt_IsMatchCB    isDirMatch;
    void*                   isDirMatchData;
} Fks_DirEnt_Matchs;

FKS_LIB_DECL (Fks_DirEntries*)  fks_createDirEntries(Fks_DirEntries* dirEntries
                                    , char const*        dirPath
                                    , Fks_DirEnt_Matchs* mt FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (void)             fks_releaseDirEntries(Fks_DirEntries* dirEntries) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_isize_t)      fks_foreachDirEntries(Fks_DirEntries* dirEntries
                                    , int (*cb)(void* data, Fks_DirEnt const* dirEnt, char const* dirPath)
                                    , void*                 cbData          FKS_ARG_INI(NULL)
                                    , Fks_DirEnt_Matchs*    mt              FKS_ARG_INI(NULL) ) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_isize_t)      fks_countDirEntries(size_t* strBytes
                                    , Fks_DirEntries*    dirEntries
                                    , Fks_DirEnt_Matchs* mt FKS_ARG_INI(NULL)) FKS_NOEXCEPT;


FKS_LIB_DECL (fks_isize_t)      fks_convDirEntPathStats(Fks_DirEntPathStat** ppPathStats
                                    , Fks_DirEntries*    dirEntries
                                    , Fks_DirEnt_Matchs* mt FKS_ARG_INI(NULL)) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_isize_t)      fks_createDirEntPathStats(Fks_DirEntPathStat** ppPathStats
                                    , char const*        dirPath
                                    , Fks_DirEnt_Matchs* mt FKS_ARG_INI(NULL)) FKS_NOEXCEPT;

FKS_LIB_DECL (void)             fks_releaseDirEntPathStats(Fks_DirEntPathStat* pathStats) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_isize_t)      fks_convDirEntPaths(char*** ppPaths
                                    , Fks_DirEntries*    dirEntries
                                    , Fks_DirEnt_Matchs* mt FKS_ARG_INI(NULL)) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_isize_t)      fks_createDirEntPaths(char*** ppPaths
                                    , char const*        dirPath
                                    , Fks_DirEnt_Matchs* mt FKS_ARG_INI(NULL)) FKS_NOEXCEPT;

FKS_LIB_DECL (void)             fks_releaseDirEntPaths(char** paths) FKS_NOEXCEPT;

int     fks_dirEnt_isMatchStartWithNonDot(void* dmy, Fks_DirEnt const* ent);

#ifdef __cplusplus
}
#endif

#endif  // FKS_DIRENTRY_H_INCLUDED
