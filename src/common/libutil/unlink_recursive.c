/*****************************************************************************\
 *  Copyright (c) 2014 Lawrence Livermore National Security, LLC.  Produced at
 *  the Lawrence Livermore National Laboratory (cf, AUTHORS, DISCLAIMER.LLNS).
 *  LLNL-CODE-658032 All rights reserved.
 *
 *  This file is part of the Flux resource manager framework.
 *  For details, see https://github.com/flux-framework.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the license, or (at your option)
 *  any later version.
 *
 *  Flux is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the terms and conditions of the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *  See also:  http://www.gnu.org/licenses/
 \*****************************************************************************/

#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>

#include <czmq.h>
#include "unlink_recursive.h"

static int unlinkat_recursive (int fd)
{
    DIR *dir;
    struct dirent *d, entry;
    struct stat sb;
    int cfd;
    int count = 0;

    if ((dir = fdopendir (fd))) {
        while (readdir_r (dir, &entry, &d) == 0 && d != NULL) {
            if (!strcmp (d->d_name, ".") || !strcmp (d->d_name, ".."))
                continue;
            if (fstatat (fd, d->d_name, &sb, AT_SYMLINK_NOFOLLOW) < 0)
                continue;
            if (S_ISDIR (sb.st_mode)) {
                if ((cfd = openat (fd, d->d_name, O_DIRECTORY)) < 0)
                    continue;
                count += unlinkat_recursive (cfd);
                if (unlinkat (fd, d->d_name, AT_REMOVEDIR) == 0)
                    count++;
            } else {
                if (unlinkat (fd, d->d_name, 0) == 0)
                    count++;
            }
        }
        closedir (dir);
    }
    return count;
}

int unlink_recursive (const char *dirpath)
{
    int count = 0;
    DIR *dir = opendir (dirpath);
    if (dir) {
        count += unlinkat_recursive (dirfd (dir));
        if (rmdir (dirpath) == 0)
            count++;
    }
    return count;
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */