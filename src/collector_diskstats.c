/* collector.c */

/*
 * MiniMond, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject to
 * receipt of any required approvals from the U.S. Dept. of Energy).  All
 * rights reserved.
 * 
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer Department
 * at  TTD@lbl.gov.
 * 
 * NOTICE.  This software is owned by the U.S. Department of Energy.  As
 * such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide
 * license in the Software to reproduce, prepare derivative works, and
 * perform publicly and display publicly.  Beginning five (5) years after
 * the date permission to assert copyright is obtained from the U.S.
 * Department of Energy, and subject to any subsequent five (5) year
 * renewals, the U.S. Government is granted for itself and others acting
 * on its behalf a paid-up, nonexclusive, irrevocable, worldwide license
 * in the Software to reproduce, prepare derivative works, distribute
 * copies to the public, perform publicly and display publicly, and to
 * permit others to do so.
 */

#include <stdio.h>
#include <unistd.h>
#include "minimond.h"
#include "unistd.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"

#define DISKSTATS "/proc/diskstats"


metric_group *diskstats_collect(metric_group *mg) {
    FILE *diskstats = NULL;
    metric_file_open(&diskstats, DISKSTATS);

    diskstats_collect_from_file(mg, diskstats);

    metric_file_close(diskstats);

    return mg;
}

metric_group *diskstats_collect_from_file(metric_group *mg, FILE *f) {

    char buf[MAX_LINE];
    char name[MAX_LINE];
    char name_buf[MAX_LINE];

    int count = 0;
    int metric_count = 0;
    long unsigned d[METRIC_GROUP_MAX_SIZE];

    /* These names are from mod_diskstat */
    const char *labels[] = { "readIO", "readMerge", "readSectors",
                             "readTicks", "writeIO", "writeMerge",
                             "writeSectors", "writeTicks", "inFlight",
                             "ioTicks", "inQueue", NULL
                           };

    mg->type = VALUE_LONG;
    s_strncpy(mg->name, "diskstats", NAME_MAX);

    while (fgets(buf, MAX_LINE, f)) {
        /*
         * maj    min   dev  rIO  rMer rSec  rTick   wIO  wMer  wSec  wTick inFli ioTick  inQueue
         * 8      17    sdb1 1887 3905 46336 29476   0    0     0     0     0     28308   29467
         * %*     %*    %s   d[0] d[1] d[2]  d[3]    d[4] d[5]  d[6]  d[7]  d[8]  d[9]    d[10]
         */
        count = sscanf(buf," %*d %*d %32[^\t ] %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                name_buf,
                &d[0],
                &d[1],
                &d[2],
                &d[3],
                &d[4],
                &d[5],
                &d[6],
                &d[7],
                &d[8],
                &d[9],
                &d[10]
                );

        if (count != 12) continue;

        for( count=0 ; labels[count] != NULL; count++, metric_count_incr(&metric_count) ) { 


            snprintf(name, MAX_LINE, "%s_%s", name_buf, labels[count] );

            MetricSetName(mg, metric_count, name);

            mg->metrics[metric_count].val.l = d[count];

        }

    }


    return mg;
}
