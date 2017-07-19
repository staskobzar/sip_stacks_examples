#include <stdio.h>
#include <sofia-sip/nua.h>

typedef struct application
{
  su_home_t       home[1];  /* memory home */
  su_root_t      *root;     /* root object */
  nua_t          *nua;      /* NUA stack object */
} app;

int main_init(app *app);
void main_deinit(app *app);
void app_callback(nua_event_t   event,
    int           status,
    char const   *phrase,
    nua_t        *nua,
    nua_magic_t  *magic,
    nua_handle_t *nh,
    nua_hmagic_t *hmagic,
    sip_t const  *sip,
    tagi_t        tags[]);


int main(int argc, const char *argv[])
{
  nua_handle_t *nh;
  app app[1] = {{{{(sizeof app)}}}};

  /* init */
  if(!main_init(app)) {
    goto exitpoint;
  }

  printf("--------------------- BEGIN ---------------------\n");
  app->nua = nua_create(app->root,
      app_callback,
      app,
      /* tags as necessary ...*/
      TAG_NULL());
  nh = nua_handle (app->nua, app->home, NUTAG_URL("sip:8888@192.168.1.139"), TAG_NULL());
  if(nh) {
    nua_options(nh, TAG_NULL());
    su_root_run(app->root);

    nua_destroy(app->nua);
  }
  printf("---------------------  END  ---------------------\n");

exitpoint:
  /* deinit */
  main_deinit(app);
  return 0;
}

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
  printf("Event: %d\n", event);
  printf("phrase: %s\n", phrase);
  if(sip && sip->sip_payload && sip->sip_payload->pl_data)
    printf("payload: %s\n", sip->sip_payload->pl_data);
  switch(event){
    case nua_r_shutdown:
      printf("Shutdown nua\n");
      if(status < 200){
        printf("shutdown in progres...\n");
      } else {
        su_root_break(((app*)magic)->root);
      }
      break;
    default:
      nua_shutdown(((app*)magic)->nua);
      break;
  }
}

void main_deinit(app *app)
{
  su_root_destroy(app->root);
  su_home_deinit(app->home);
  su_deinit();
}

int main_init(app *app)
{
  su_init();

  su_home_init(app->home);
  if(app->home == NULL) {
    return 0;
  }

  app->root = su_root_create(app);
  if(app->root == NULL) {
    return 0;
  }

  return 1;
}
