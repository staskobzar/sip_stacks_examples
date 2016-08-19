#include <netinet/in.h>
#include <eXosip2/eXosip.h>
#include <assert.h>

#define USER    "alice"
#define PASS    "secret"
#define DOMAIN  "example.com"
#define URI     "sip:" USER "@" DOMAIN
#define PROXY   "sip:" DOMAIN

int main(int argc, const char *argv[])
{
  struct eXosip_t *ctx;
  int res,
      rid, // A unique identifier for the registration context
      port = 5060;
  osip_message_t *reg = NULL;

  // Init trace
  TRACE_INITIALIZE (6, NULL);

  // Init eXosip stack
  ctx = eXosip_malloc ();
  assert (ctx != NULL);
  assert (eXosip_init (ctx) == 0);

  // open a UDP socket
  if (eXosip_listen_addr (ctx, IPPROTO_UDP, NULL, port, AF_INET, 0) != 0)
  {
    eXosip_quit (ctx);
    fprintf (stderr, "Can't init transport layer.\n");
    return -1;
  }
  //====================
  printf ("eXosip UA Register\n");
  eXosip_lock (ctx);
  eXosip_add_authentication_info (
      ctx,
      USER,   // username
      USER,   // user id
      PASS,   // password
      NULL,
      NULL
      );
  rid = eXosip_register_build_initial_register (
          ctx,                        // eXosip context
          URI,  // from
          PROXY,        // proxy
          URI,  // contact
          3600,                       // expires
          &reg
        );
  if ( rid < 0 )
  {
    eXosip_unlock (ctx);
    fprintf (stderr, "Can not build register packet.\n");
    return -1;
  }

  printf ("Registration uniqu id: %d\n", rid);
  res = eXosip_register_send_register (ctx, rid, reg);
  printf ("Register packet respons: %d\n", res);
  eXosip_unlock (ctx);
  int i = 0;
  for (;;)
  {
    eXosip_event_t *evt;
    if (!(evt = eXosip_event_wait (ctx, 0, 1)))
    {
      eXosip_execute (ctx);
      eXosip_automatic_action (ctx);
      continue;
    }
    eXosip_execute (ctx);
    eXosip_automatic_action (ctx);
    printf ("Try %d\n", i++);
    switch (evt->type)
    {
      case EXOSIP_REGISTRATION_SUCCESS:
        printf ("Successfully registered.\n");
        return 0;
        break;
      default:
        printf ("Can not register.\n");
        break;
    }
    eXosip_event_free(evt);
  }
  eXosip_quit (ctx);
  return 0;
}
