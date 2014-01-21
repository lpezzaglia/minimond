/* args.c */

#include <stdlib.h>
#include "mingmond.h"

config cfg;

config *ConfigDefaultCreate(config *c) {
    s_strncpy(c->user, MINGMOND_USER, NAME_MAX-1);
    s_strncpy(c->logfile, MINGMOND_LOG, NAME_MAX-1);

    c->pidfile[0] = '\0';

    c->daemonize = DAEMONIZE;
    c->debug_fg = DEBUG_FG;
    c->drop_privileges = DROP_PRIVILEGES;

    c->collectors = default_collectors;
    c->printers = default_printers;

    return c;
}

void parse_args(int argc, char **argv, config *c) {
    int arg;

    if(!(ConfigDefaultCreate(c))) {
        fatal_error("Failed to load default configuration.\n");
    }
    while ((arg = getopt(argc, argv, "hl:p:v")) != -1) {
        switch(arg) {
            case 'l':
                s_strncpy(c->logfile, optarg, MAX_LINE);
                break;
            case 'p':
                s_strncpy(c->pidfile, optarg, MAX_LINE);
                break;
            case 'v':
                printf("%s\n",PACKAGE_STRING);
                exit(EXIT_SUCCESS);
                break;
            case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case '?':
                usage_and_fail(argv[0]);
                break;
        }
    }
}