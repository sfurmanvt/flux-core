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

/* flux-kvs.c - flux kvs subcommand */

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <getopt.h>
#include <assert.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdarg.h>
#include <json.h>
#include <czmq.h>

#include "xzmalloc.h"
#include "log.h"
#include "shortjson.h"

#include "flux.h"
#include "kvs.h"
#include "api.h"

#define OPTIONS "h"
static const struct option longopts[] = {
    {"help",       no_argument,  0, 'h'},
    { 0, 0, 0, 0 },
};

void cmd_get (flux_t h, int argc, char **argv);
void cmd_put (flux_t h, int argc, char **argv);
void cmd_unlink (flux_t h, int argc, char **argv);
void cmd_link (flux_t h, int argc, char **argv);
void cmd_readlink (flux_t h, int argc, char **argv);
void cmd_mkdir (flux_t h, int argc, char **argv);
void cmd_version (flux_t h, int argc, char **argv);
void cmd_wait (flux_t h, int argc, char **argv);
void cmd_watch (flux_t h, int argc, char **argv);
void cmd_dropcache (flux_t h, int argc, char **argv);
void cmd_dropcache_all (flux_t h, int argc, char **argv);


void usage (void)
{
    fprintf (stderr,
"Usage: flux-kvs get         key [key...]\n"
"       flux-kvs put         key=val [key=val...]\n"
"       flux-kvs unlink      key [key...]\n"
"       flux-kvs link        target link_name\n"
"       flux-kvs readlink    key\n"
"       flux-kvs mkdir       key [key...]\n"
"       flux-kvs watch       key\n"
"       flux-kvs version\n"
"       flux-kvs wait N\n"
"       flux-kvs dropcache\n"
"       flux-kvs dropcache-all\n"
);
    exit (1);
}

int main (int argc, char *argv[])
{
    flux_t h;
    int ch;
    char *cmd;

    log_init ("flux-kvs");

    while ((ch = getopt_long (argc, argv, OPTIONS, longopts, NULL)) != -1) {
        switch (ch) {
            case 'h': /* --help */
                usage ();
                break;
            default:
                usage ();
                break;
        }
    }
    if (optind == argc)
        usage ();
    cmd = argv[optind++];

    if (!(h = flux_api_open ()))
        err_exit ("flux_api_open");

    if (!strcmp (cmd, "get"))
        cmd_get (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "put"))
        cmd_put (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "unlink"))
        cmd_unlink (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "link"))
        cmd_link (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "readlink"))
        cmd_readlink (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "mkdir"))
        cmd_mkdir (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "version"))
        cmd_version (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "wait"))
        cmd_wait (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "watch"))
        cmd_watch (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "dropcache"))
        cmd_dropcache (h, argc - optind, argv + optind);
    else if (!strcmp (cmd, "dropcache-all"))
        cmd_dropcache_all (h, argc - optind, argv + optind);
    else
        usage ();

    flux_api_close (h);
    log_fini ();
    return 0;
}

void cmd_get (flux_t h, int argc, char **argv)
{
    JSON o;
    int i;

    if (argc == 0)
        msg_exit ("get: specify one or more keys");
    for (i = 0; i < argc; i++) {
        if (kvs_get (h, argv[i], &o) < 0)
            err_exit ("%s", argv[i]);
        else
            printf ("%s\n", Jtostr (o));
        Jput (o);
    }
}

void cmd_put (flux_t h, int argc, char **argv)
{
    int i;

    if (argc == 0)
        msg_exit ("put: specify one or more key=value pairs");
    for (i = 0; i < argc; i++) {
        char *key = xstrdup (argv[i]);
        char *val = strchr (key, '=');
        JSON o;
        if (!val)
            msg_exit ("put: you must specify a value as key=value");
        *val++ = '\0';
        if ((o = Jfromstr (val))) {
            if (kvs_put (h, key, o) < 0)
                err_exit ("%s", key);
        } else {
            if (kvs_put_string (h, key, val) < 0)
                err_exit ("%s", key);
        }
        Jput (o);
        free (key);
    }
    if (kvs_commit (h) < 0)
        err_exit ("kvs_commit");
}

void cmd_unlink (flux_t h, int argc, char **argv)
{
    int i;

    if (argc == 0)
        msg_exit ("unlink: specify one or more keys");
    for (i = 0; i < argc; i++) {
        /* FIXME: unlink nonexistent silently fails */
        /* FIXME: unlink directory silently succedes */
        if (kvs_unlink (h, argv[i]) < 0)
            err_exit ("%s", argv[i]);
    }
    if (kvs_commit (h) < 0)
        err_exit ("kvs_commit");
}

void cmd_link (flux_t h, int argc, char **argv)
{
    if (argc != 2)
        msg_exit ("link: specify target and link_name");
    if (kvs_symlink (h, argv[1], argv[0]) < 0)
        err_exit ("%s", argv[1]);
    if (kvs_commit (h) < 0)
        err_exit ("kvs_commit");
}

void cmd_readlink (flux_t h, int argc, char **argv)
{
    int i;
    char *target;

    if (argc == 0)
        msg_exit ("readlink: specify one or more keys"); 
    for (i = 0; i < argc; i++) {
        if (kvs_get_symlink (h, argv[i], &target) < 0)
            err_exit ("%s", argv[i]);
        else
            printf ("%s\n", target);
        free (target);
    }
}

void cmd_mkdir (flux_t h, int argc, char **argv)
{
    int i;

    if (argc == 0)
        msg_exit ("mkdir: specify one or more directories");
    for (i = 0; i < argc; i++) {
        if (kvs_mkdir (h, argv[i]) < 0)
            err_exit ("%s", argv[i]);
    }
    if (kvs_commit (h) < 0)
        err_exit ("kvs_commit");
}

void cmd_version (flux_t h, int argc, char **argv)
{
    int vers;
    if (argc != 0)
        msg_exit ("version: takes no arguments");
    if (kvs_get_version (h, &vers) < 0)
        err_exit ("kvs_get_version");
    printf ("%d\n", vers);
}

void cmd_wait (flux_t h, int argc, char **argv)
{
    int vers;
    if (argc != 1)
        msg_exit ("wait: specify a version");
    vers = strtoul (argv[0], NULL, 10);
    if (kvs_wait_version (h, vers) < 0)
        err_exit ("kvs_get_version");
    printf ("%d\n", vers);
}

void cmd_watch (flux_t h, int argc, char **argv)
{
    JSON o = NULL;

    if (argc != 1)
        msg_exit ("watch: specify one key");
    if (kvs_get (h, argv[0], &o) < 0 && errno != ENOENT) 
        err_exit ("%s", argv[0]);
    printf ("%s\n", o ? Jtostr (o) : "NULL");
    for (;;) {
        if (kvs_watch_once (h, argv[0], &o) < 0 && errno != ENOENT)
            err_exit ("%s", argv[0]);
        printf ("%s\n", o ? Jtostr (o) : "NULL");
    }
    /* FIXME: handle SIGINT? */
    /* FIXME: handle directory */
    Jput (o);
}

void cmd_dropcache (flux_t h, int argc, char **argv)
{
    if (argc != 0)
        msg_exit ("dropcache: takes no arguments");
    if (kvs_dropcache (h) < 0)
        err_exit ("kvs_dropcache");
}

void cmd_dropcache_all (flux_t h, int argc, char **argv)
{
    if (argc != 0)
        msg_exit ("dropcache-all: takes no arguments");
    if (flux_event_send (h, NULL, "kvs.dropcache") < 0)
        err_exit ("flux_event_send");
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */