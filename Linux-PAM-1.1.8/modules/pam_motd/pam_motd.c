/* pam_motd module */

/*
 * Modified for pam_motd by Ben Collins <bcollins@debian.org>
 *
 * Based off of:
 * $Id$
 *
 * Written by Michael K. Johnson <johnsonm@redhat.com> 1996/10/24
 *
 */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <syslog.h>

#include <security/_pam_macros.h>
#include <security/pam_ext.h>
/*
 * here, we make a definition for the externally accessible function
 * in this file (this definition is required for static a module
 * but strongly encouraged generally) it is used to instruct the
 * modules include file to define the function prototypes.
 */

#define PAM_SM_SESSION
#define DEFAULT_MOTD	"/etc/motd"

#include <security/pam_modules.h>
#include <security/pam_modutil.h>

/* --- session management functions (only) --- */

PAM_EXTERN int
pam_sm_close_session (pam_handle_t *pamh UNUSED, int flags UNUSED,
		      int argc UNUSED, const char **argv UNUSED)
{
     return PAM_IGNORE;
}

static char default_motd[] = DEFAULT_MOTD;

PAM_EXTERN
int pam_sm_open_session(pam_handle_t *pamh, int flags,
			int argc, const char **argv)
{
    int retval = PAM_IGNORE;
    int do_update = 1;
    int fd;
    const char *motd_path = NULL;
    char *mtmp = NULL;
    struct stat st_motd;

    if (flags & PAM_SILENT) {
	return retval;
    }

    for (; argc-- > 0; ++argv) {
        if (!strncmp(*argv,"motd=",5)) {

            motd_path = 5 + *argv;
            if (*motd_path != '\0') {
                D(("set motd path: %s", motd_path));
	    } else {
		motd_path = NULL;
		pam_syslog(pamh, LOG_ERR,
			   "motd= specification missing argument - ignored");
	    }
	}
        else if (!strcmp(*argv,"noupdate")) {
            do_update = 0;
        }
	else
	    pam_syslog(pamh, LOG_ERR, "unknown option: %s", *argv);
    }

    if (motd_path == NULL)
	motd_path = default_motd;

    /* Run the update-motd dynamic motd scripts, outputting to /run/motd.dynamic.
       This will be displayed only when calling pam_motd with
       motd=/run/motd.dynamic; current /etc/pam.d/login and /etc/pam.d/sshd
       display both this file and /etc/motd. */
    if (do_update && (stat("/etc/update-motd.d", &st_motd) == 0)
        && S_ISDIR(st_motd.st_mode))
    {
       mode_t old_mask = umask(0022);
       if (!system("/usr/bin/env -i PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin run-parts /etc/update-motd.d > /run/motd.dynamic.new"))
           rename("/run/motd.dynamic.new", "/run/motd.dynamic");
       umask(old_mask);
    }

    while ((fd = open(motd_path, O_RDONLY, 0)) >= 0) {
	struct stat st;

	/* fill in message buffer with contents of motd */
	if ((fstat(fd, &st) < 0) || !st.st_size || st.st_size > 0x10000)
	    break;

	if (!(mtmp = malloc(st.st_size+1)))
	    break;

	if (pam_modutil_read(fd, mtmp, st.st_size) != st.st_size)
	    break;

	if (mtmp[st.st_size-1] == '\n')
	    mtmp[st.st_size-1] = '\0';
	else
	    mtmp[st.st_size] = '\0';

	pam_info (pamh, "%s", mtmp);
	break;
    }

    _pam_drop (mtmp);

    if (fd >= 0)
	close(fd);

     return retval;
}


#ifdef PAM_STATIC

/* static module data */

struct pam_module _pam_motd_modstruct = {
     "pam_motd",
     NULL,
     NULL,
     NULL,
     pam_sm_open_session,
     pam_sm_close_session,
     NULL,
};

#endif

/* end of module definition */
