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

bool is_deleted = false;

void recreate_object(void* private_data)
{
    (void) private_data;
    sleep(1);
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
    sr_set_item(session, "/module-containing-flag:flag-container/objects-you-want-to-delete[name='PleaseHaveMercy']", NULL, 0);
    sr_apply_changes(session, 0);
    sr_disconnect(connection);
}


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
    pthread_t t1;

    printf("\n\n ========== DATA FOR \"%s\" \"%s\" REQUESTED =======================\n\n", module_name, xpath);

    if(is_deleted)
    {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        lyd_new_path(NULL, ly_ctx, "/module-containing-flag:flag-container/flag", "NOKCTF{SET_FLAG_FOR_CHALL_10}", 0, parent);
        sr_release_context(sr_session_get_connection(session));
        pthread_create(&t1, NULL, (void* (*)(void*))recreate_object, NULL);
        pthread_detach(t1);
        is_deleted = false;
    }

    return SR_ERR_OK;
}

static int set_item_cb(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *xpath,
        sr_event_t event, uint32_t request_id, void *private_data)
{
    sr_change_iter_t* it = NULL;
    int               rc = SR_ERR_OK;
    sr_change_oper_t  oper;
    sr_val_t*         old_val = NULL;
    sr_val_t*         new_val = NULL;
    char*             link_name;
    (void)xpath;
    (void)request_id;
    (void)private_data;

    rc = sr_get_changes_iter(session, "//.", &it);

    while ((rc = sr_get_change_next(session, it, &oper, &old_val, &new_val)) == SR_ERR_OK)
    {
        switch (oper)
        {
        case SR_OP_CREATED:
        case SR_OP_MODIFIED:
        case SR_OP_MOVED:
            break;

        case SR_OP_DELETED:
            is_deleted = true;
            break;
        }
        sr_free_val(old_val);
        sr_free_val(new_val);
    } // while

cleanup:
    sr_free_change_iter(it);
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
    rc = sr_oper_get_subscribe(session, "module-containing-flag", "/module-containing-flag:flag-container", get_items_cb, NULL, 0, &subscription);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }

    rc = sr_module_change_subscribe(session, "module-containing-flag", NULL, set_item_cb, NULL, 0, SR_SUBSCR_ENABLED, &subscription);
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