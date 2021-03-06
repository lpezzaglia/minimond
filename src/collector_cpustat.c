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

#define CPUSTAT "/proc/stat"

/*
 * cpu_aidle      Percent of time since boot idle CPU (module cpu_module)
 * cpu_num        Total number of CPUs (module cpu_module)
 * cpu_speed      CPU Speed in terms of MHz (module cpu_module)
 *
 * cpu_user       Percentage of CPU utilization that occurred while executing at the user level (module cpu_module)
 * cpu_nice       Percentage of CPU utilization that occurred while executing at the user level with nice priority (module cpu_module)
 * cpu_system     Percentage of CPU utilization that occurred while executing at the system level (module cpu_module)
 * cpu_idle       Percentage of time that the CPU or CPUs were idle and the system did not have an outstanding disk I/O request (module cpu_module)
 * cpu_wio        Percentage of time that the CPU or CPUs were idle during which the system had an outstanding disk I/O request (module cpu_modul
 * cpu_intr       cpu_intr (module cpu_module)
 * cpu_sintr      cpu_sintr (module cpu_module)
 * cpu_steal      cpu_steal (module cpu_module)
 * boottime       The last time that the system was started (module sys_module)
 */

metric_group *cpustat_collect(metric_group *mg) {
    FILE *cpustat = NULL;
    metric_file_open(&cpustat, CPUSTAT);

    cpustat_collect_from_file(mg, cpustat);

    metric_file_close(cpustat);

    return mg;
}

metric_group *cpustat_collect_from_file(metric_group *mg, FILE *f) {
    char buf[MAX_LINE];
    char name[MAX_LINE];
    char name_buf[MAX_LINE];

    int count = 0;
    int metric_count = 0;
    long unsigned d[METRIC_GROUP_MAX_SIZE];
    const char *labels[] = { "cpu_user", "cpu_nice", "cpu_system", "cpu_idle",
                             "cpu_wio", "cpu_intr", "cpu_sintr", "cpu_steal",
                             "cpu_guest", NULL, "boottime",
                             NULL };

    mg->type = VALUE_LONG;
    s_strncpy(mg->name, "cpustat", NAME_MAX);

    while (fgets(buf, MAX_LINE, f)) {
        switch(buf[0]) {
            case 'c':
                /*
                 * cpu  user      nice     sys      idle       iowait   irq    softirq steal guest
                 * cpu  516284101 34772015 48855234 3210397546 94612030 288167 1684015 0     0
                 * %s   d[0]      d[1]     d[2]     d[3]       d[4]     d[5]   d[6]    d[7]  d[8]
                 */
                /*                       0   1   2   3   4   5   6   7   8 */
                count = sscanf(buf," %32s %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                        name_buf,
                        &d[0],
                        &d[1],
                        &d[2],
                        &d[3],
                        &d[4],
                        &d[5],
                        &d[6],
                        &d[7],
                        &d[8]
                        );

                break;
        }

        if (count != 10) continue;

        for( count=0 ; labels[count] != NULL; count++, metric_count_incr(&metric_count) ) { 

            snprintf(name, MAX_LINE, "%s_%s", name_buf, labels[count] );
            s_strncpy(mg->metrics[metric_count].name, name, NAME_MAX);
            mg->metrics[metric_count].val.l = d[count];

        }

    }

    return mg;
}
