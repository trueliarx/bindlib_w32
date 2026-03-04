/* this function for Windows NT specific resolver stuff L. Kahn */

#include <windows.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <stdio.h>
#include <ctype.h>
#include <resolv.h>
#include <netdb.h>
#include "conf/portability.h"
#include <stdlib.h>

char *pathnetworks, *pathhosts, *pathresconf;
int res_paths_initialized = 0;


/* lgk new function to initialize path variables that are outside the scope of the main */
/* for backwards compatibility allow either resolv.conf or resolv.ini */
void init_res_paths()
{
    FILE *fp = NULL;
    DWORD ret;

    pathhosts = (char *)malloc(MAX_PATH);
    if (pathhosts == NULL) {
        syslog(LOG_ERR, "malloc(pathhosts) failed: %m\n");
        return;
    }
    ret = ExpandEnvironmentStringsA(_PATH_HOSTS, pathhosts, MAX_PATH);
    if (ret == 0 || ret > MAX_PATH) 
        syslog(LOG_ERR, "ExpandEnvironmentStringsA(_PATH_HOSTS) failed or truncated\n");

    pathnetworks = (char *)malloc(MAX_PATH);
    if (pathnetworks == NULL) {
        syslog(LOG_ERR, "malloc(pathnetworks) failed: %m\n");
        free(pathhosts);
        return;
    }
    ret = ExpandEnvironmentStringsA(_PATH_NETWORKS, pathnetworks, MAX_PATH);
    if (ret == 0 || ret > MAX_PATH)
        syslog(LOG_ERR, "ExpandEnvironmentStringsA(_PATH_NETWORKS) failed or truncated\n");

    pathresconf = (char *)malloc(MAX_PATH);
    if (pathresconf == NULL) {
        syslog(LOG_ERR, "malloc(pathresconf) failed: %m\n");
        free(pathhosts);
        free(pathnetworks);
        return;
    }
    ret = ExpandEnvironmentStringsA(_PATH_RESCONF, pathresconf, MAX_PATH);
    if (ret == 0 || ret > MAX_PATH)
        syslog(LOG_ERR, "ExpandEnvironmentStringsA(_PATH_RESCONF) failed or truncated\n");

    /* Try to open the resolved path; if it fails, try the alternate path */
    if (fopen_s(&fp, pathresconf, "r") != 0) {
        /* try alternate name */
        ret = ExpandEnvironmentStringsA(_ALT_PATH_RESCONF, pathresconf, MAX_PATH);
        if (ret == 0 || ret > MAX_PATH) {
            syslog(LOG_ERR, "ExpandEnvironmentStringsA(_ALT_PATH_RESCONF) failed or truncated\n");
        } else {
            if (fopen_s(&fp, pathresconf, "r") == 0) {
                fclose(fp);
            }
        }
    } else {
        fclose(fp);
    }

    res_paths_initialized = 1;
}


