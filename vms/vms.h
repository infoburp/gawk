/*
 * vms.h - miscellaneous definitions for use with VMS system services.
 *							Pat Rankin, Nov'89
 */

#if 0
#include <iodef.h>
#else
#define IO$_WRITEVBLK	48	/* write virtual block */
#define IO$V_CANCTRLO	6	/* cancel <ctrl/O> (ie, resume tty output) */
#define IO$M_CANCTRLO	(1 << IO$V_CANCTRLO)
#endif

#if 0
#include <clidef.h>
#include <cliverbdef.h>
#include <fscndef.h>
#else
#define CLI$K_GETCMD	1
#define CLI$K_VERB_MCR	33
#define CLI$K_VERB_RUN	36
#define FSCN$_FILESPEC	1
#endif

#if 0
#include <climsgdef.h>
#else
#define CLI$_RUNUSED	0x00030000	/* value returned by $CLI for "RUN" */
#define CLI$_SYNTAX	0x000310FC	/* error signalled by CLI$DCL_PARSE */
#define CLI$_INSFPRM	0x00038048	/* insufficient parameters */
#define CLI$_VALREQ	0x00038150	/* missing required value  */
#define CLI$_NEGATED	0x000381F8	/* explicitly negated */
#define CLI$_CONFLICT	0x00038258	/* conflicting qualifiers  */
#define CLI$_NOOPTPRS	0x00038840	/* no option present	   */
#endif

#if 0
#include <psldef.h>
#else
#define PSL$C_USER	3	/* user mode */
#endif

/* note: `ulong' and `u_long' end up conflicting with various header files */
typedef unsigned long	U_Long;
typedef unsigned short	U_Short;

typedef struct _dsc { int len; char *adr; } Dsc; /* limited string descriptor */
 /* standard VMS itemlist-3 structure */
typedef struct _itm { U_Short len, code; void *buffer; U_Short *retlen; } Itm;

#define vmswork(sts) ((sts)&1)
#define vmsfail(sts) (!vmswork(sts))
#define CondVal(sts) ((sts)&0x0FFFFFF8)     /* strip severity & msg inhibit */
#define Descrip(strdsc,strbuf) Dsc strdsc = {sizeof strbuf - 1, (char *)strbuf}

extern int    shell$is_shell(void);
extern U_Long LIB$FIND_FILE(const Dsc *, Dsc *, void *, ...);
extern U_Long LIB$FIND_FILE_END(void *);
#ifndef NO_TTY_FWRITE
extern U_Long LIB$GET_EF(long *);
extern U_Long SYS$ASSIGN(const Dsc *, short *, long, const Dsc *);
extern U_Long SYS$DASSGN(short);
extern U_Long SYS$QIO(U_Long, U_Long, U_Long, void *,
			 void (*)(U_Long), U_Long,
			 const char *, int, int, U_Long, int, int);
extern U_Long SYS$SYNCH(long, void *);
#endif	/*!NO_TTY_FWRITE*/
extern U_Long LIB$SPAWN(const Dsc *,const Dsc *,const Dsc *,
			   const U_Long *,const Dsc *,U_Long *,U_Long *,...);
 /* system services for logical name manipulation */
extern U_Long SYS$TRNLNM(const U_Long *,const Dsc *,const Dsc *,
			    const unsigned char *,Itm *);
extern U_Long SYS$CRELNM(const U_Long *,const Dsc *,const Dsc *,
			    const unsigned char *,const Itm *);
extern U_Long SYS$CRELOG(int,const Dsc *,const Dsc *,unsigned char);
extern U_Long SYS$DELLNM(const Dsc *,const Dsc *,const unsigned char *);

extern void   v_add_arg(int, const char *);
extern void   vms_exit(int);
extern char  *vms_strerror(int);
extern char  *vms_strdup(const char *);
extern int    vms_devopen(const char *,int);
extern int    vms_execute(const char *, const char *, const char *);
extern int    vms_gawk(void);
extern U_Long Cli_Present(const char *);
extern U_Long Cli_Get_Value(const char *, char *, int);
extern U_Long Cli_Parse_Command(const void *, const char *);

