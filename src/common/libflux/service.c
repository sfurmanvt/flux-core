/*****************************************************************************\
 *  Copyright (c) 2018 Lawrence Livermore National Security, LLC.  Produced at
 *  the Lawrence Livermore National Laboratory (cf, AUTHORS, DISCLAIMER.LLNS).
 *  LLNL-CODE-658032 All rights reserved.
 *
 *  This file is part of the Flux resource manager framework.
 *  For details, see https://github.com/flux-framework.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the license,
 *  or (at your option) any later version.
 *
 *  Flux is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the terms and conditions of the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *  See also:  http://www.gnu.org/licenses/
\*****************************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <flux/core.h>

flux_future_t *flux_service_register (flux_t *h, const char *name)
{
    if (!h || !name) {
        errno = EINVAL;
        return NULL;
    }
    return flux_rpc_pack (h, "service.add", FLUX_NODEID_ANY, 0,
                          "{s:s}", "service", name);
}

flux_future_t *flux_service_unregister (flux_t *h, const char *name)
{
    if (!h || !name) {
        errno = EINVAL;
        return NULL;
    }
    return flux_rpc_pack (h, "service.remove", FLUX_NODEID_ANY, 0,
                          "{s:s}", "service", name);
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */