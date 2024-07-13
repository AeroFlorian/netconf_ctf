#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <libyang/libyang.h>

#include "sysrepo.h"

volatile int exit_application = 0;

void periodic_notif(void* private_data)
{
    (void) private_data;
    while(!exit_application)
    {
        sr_conn_ctx_t *connection = NULL;
        sr_session_ctx_t *session = NULL;
        sr_subscription_ctx_t *subscription = NULL;
        int rc = SR_ERR_OK;

        /* turn logging on */
        sr_log_stderr(SR_LL_WRN);

        /* connect to sysrepo */
        rc = sr_connect(0, &connection);
        if (rc != SR_ERR_OK) {
            return;
        }

        /* start session */
        rc = sr_session_start(connection, SR_DS_RUNNING, &session);
        if (rc != SR_ERR_OK) {
            return;
        }

        const ly_ctx* ctx = sr_acquire_context(connection);
        lyd_node* notif = NULL;
        lyd_new_path(NULL, ctx, "/module-containing-flag:here-is-the-flag-take-it", NULL , LYD_NEW_PATH_UPDATE, &notif);
        lyd_new_path(notif, ctx, "/module-containing-flag:here-is-the-flag-take-it/flag", "NOKCTF{SET_FLAG_FOR_CHALL_5}", LYD_NEW_PATH_UPDATE, NULL);
        char* tree = NULL;
        lyd_print_mem(&tree, notif, LYD_XML, LYD_PRINT_KEEPEMPTYCONT | LYD_PRINT_WD_ALL_TAG | LYD_PRINT_WITHSIBLINGS);
        printf("%s\n", tree);
        sr_notif_send_tree(session, notif, 0, 0);
        sr_release_context(connection);
        sr_disconnect(connection);
        sleep(1);
    }
}



static void
sigint_handler(int signum)
{
    (void)signum;

    exit_application = 1;
}

int
main(int argc, char **argv)
{
    sr_conn_ctx_t *connection = NULL;
    sr_session_ctx_t *session = NULL;
    sr_subscription_ctx_t *subscription = NULL;
    int rc = SR_ERR_OK;

    /* turn logging on */
    sr_log_stderr(SR_LL_WRN);

    /* connect to sysrepo */
    rc = sr_connect(0, &connection);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }

    /* start session */
    rc = sr_session_start(connection, SR_DS_RUNNING, &session);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }
    pthread_t t1;

    pthread_create(&t1, NULL, (void* (*)(void*))periodic_notif, NULL);
    pthread_detach(t1);

    printf("\n\n ========== LISTENING FOR REQUESTS ==========\n\n");

    /* loop until ctrl-c is pressed / SIGINT is received */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);
    while (!exit_application) {
        sleep(1000);
    }

    printf("Application exit requested, exiting.\n");

cleanup:
    sr_disconnect(connection);
    return rc ? EXIT_FAILURE : EXIT_SUCCESS;
}