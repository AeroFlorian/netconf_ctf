#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libyang/libyang.h>

#include "sysrepo.h"

volatile int exit_application = 0;

static int
get_items_cb(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *xpath,
        const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
    const struct ly_ctx *ly_ctx;

    (void)session;
    (void)sub_id;
    (void)request_xpath;
    (void)request_id;
    (void)private_data;

    printf("\n\n ========== DATA FOR \"%s\" \"%s\" REQUESTED =======================\n\n", module_name, xpath);

    ly_ctx = sr_acquire_context(sr_session_get_connection(session));
    lyd_new_path(NULL, ly_ctx, "/module-containing-flag:flag-container/flag", "NOKCTF{SET_FLAG_FOR_CHALL_2}", 0, parent);
    sr_release_context(sr_session_get_connection(session));

    return SR_ERR_OK;
}

static int
get_items_cb_error(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *xpath,
        const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
    const struct ly_ctx *ly_ctx;

    (void)session;
    (void)sub_id;
    (void)request_xpath;
    (void)request_id;
    (void)private_data;

    printf("\n\n ========== DATA FOR \"%s\" \"%s\" REQUESTED =======================\n\n", module_name, xpath);

    return SR_ERR_UNAUTHORIZED;
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
    rc = sr_oper_get_subscribe(session, "module-containing-flag", "/module-containing-flag:flag-container", get_items_cb, NULL, 0, &subscription);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }

    rc = sr_oper_get_subscribe(session, "module-not-containing-flag", "/module-not-containing-flag:no-flag-container", get_items_cb_error, NULL, 0, &subscription);
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