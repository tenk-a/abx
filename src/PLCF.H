void unlink_e __((byte *nam));
void rename_e __((byte *old_name,byte *new_name));
FILE *fopen_e __((byte *name,byte *mode));
void fputs_e  __((byte *buf,FILE *fp));
byte *chg_ext __((byte *new_name,byte *old_name,byte *ext));
