#include <sofia-sip/sip_util.h>
#include <sofia-sip/su_log.h>
#include <sofia-sip/nua_tag.h>
#include <sofia-sip/su_wait.h>
#include <sofia-sip/nua.h>

#define DOMAIN    "example.com"
#define USER      "alice"
#define PASS      "secret"
#define URI       "sip:" USER "@" DOMAIN

#define PROXY     "sip:192.168.1.138"

typedef struct application
{
  su_home_t       home[1];  /* memory home */
  su_root_t      *root;     /* root object */
  nua_t          *nua;      /* NUA stack object */
} Application;

void app_callback(nua_event_t   event,
                  int           status,
                  char const   *phrase,
                  nua_t        *nua,
                  nua_magic_t  *magic,
                  nua_handle_t *nh,
                  nua_hmagic_t *hmagic,
                  sip_t const  *sip,
                  tagi_t        tags[])
{
  switch (event) {
    case nua_r_register:
      su_log ("REGISTER response %d %s\n", status, phrase);
      if (status == 200) break;
      nua_authenticate (nh,
          NUTAG_AUTH("Digest:\"" DOMAIN "\":" USER ":" PASS),
          TAG_END());
      break;
    default:
      su_log ("UNKNOWN %d %s [%s]\n", status, phrase, nua_event_name (event));
      break;
  }
}

int main(int argc, const char *argv[])
{
  Application app[1] = { { { {(sizeof app)} } } };
  nua_handle_t *hl;

  su_init ();

  su_home_init(app->home); /* initialize memory handling */

  app->root = su_root_create(app); /* initialize root object */
  if (app->root != NULL) {
    app->nua = nua_create(app->root,
                          app_callback,
                          app,
                          NUTAG_PROXY(PROXY),
                          SIPTAG_FROM_STR(URI),
                          TAG_NULL());
    if (app->nua == NULL)
      su_log("ERROR: Invalid app->nua\n");

    hl = nua_handle (app->nua,
        app->home,
        SIPTAG_FROM_STR(URI),
        NUTAG_URL("sip:" DOMAIN),
        SIPTAG_TO_STR(URI),
        TAG_NULL());
    if (hl) {
      const char *expire = "3600";
      nua_register( hl,
          NUTAG_M_DISPLAY("Alice UAC"),
          NUTAG_M_USERNAME(USER),
          SIPTAG_EXPIRES_STR(expire),
          NUTAG_OUTBOUND("no-options-keepalive"), NUTAG_OUTBOUND("no-validate"), NUTAG_KEEPALIVE(0),
          TAG_NULL());
      su_log ("Send REGISTER.\n");
      su_root_run(app->root);
      nua_destroy(app->nua);
    } else {
      su_log ("ERROR: Invalid handler.\n");
    }

    /* deinit root object */
    su_root_destroy(app->root);
    app->root = NULL;
  }

  su_home_deinit(app->home); /* deinitialize memory handling */
  su_deinit();
  return 0;
}

/* vim: let g:syntastic_c_include_dirs = ['/usr/include/sofia-sip-1.12'] */
