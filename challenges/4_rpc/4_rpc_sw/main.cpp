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

int rpc_cb(sr_session_ctx_t* session, uint32_t sub_id, const char* path, const sr_val_t* input, const size_t input_cnt, sr_event_t event,
                     uint32_t request_id, sr_val_t** output, size_t* output_cnt, void* private_data)
{
    size_t i;
    (void)session;
    (void)event;
    (void)request_id;
    (void)private_data;

    *output                      = (sr_val_t*)calloc(1, sizeof(**output));
    *output_cnt                  = 1;
    (*output)[0].xpath           = strdup("/module-containing-flag:give-me-the-flag/flag");
    (*output)[0].type            = SR_STRING_T;
    (*output)[0].dflt            = 0;
    (*output)[0].data.string_val = strdup("NOKCTF{SET_FLAG_FOR_CHALL_4}");
    return SR_ERR_OK;
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

    /* subscribe for providing the operational data */
    rc = sr_rpc_subscribe(session, "/module-containing-flag:give-me-the-flag", rpc_cb, NULL, 0, 0, &subscription);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }
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