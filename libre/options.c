#include <string.h>
#include <re.h>

#define HOST    "10.230.8.20"
#define USER    "alice"
#define URI     "sip:" HOST ":5060"
#define TOURI   "sip:" USER "@" HOST
#define FROMURI "sip:" USER "@" HOST

static struct sip *sip;
static struct sipsess_sock *sess_sock;
static struct sip_request *sipreq;
struct sip_dialog *dlg = NULL;

static void connect_handler(const struct sip_msg *msg, void *arg)
{
  re_printf("-->> Connection...\n");
}

static void sip_exit_handler(void *arg)
{
  (void)arg;
  re_printf("<<-- SIP closed...\n");
}

static void resp_handler(int err, const struct sip_msg *msg, void *arg)
{
  re_printf("<<-- Response handler.\n");
  re_printf("Argument from sip_drequestf : %s\n", (char*)arg);
  re_printf("Response code: %d\n", msg->scode);
  if (err){
    re_printf("Error in reposponse handler: %s\n", strerror(err));
    return;
  }
  re_printf("----- response received ----\n");
  re_printf("%s", msg->mb->buf);
  re_printf("----------------------------\n");
  re_cancel();
}

static int send_handler(enum sip_transp tp, const struct sa *src,
                        const struct sa *dst, struct mbuf *mb, void *arg)
{
  re_printf("-->> Send handler\n");
  re_printf("src: %J\n", src);
  re_printf("dst: %J\n", dst);
  printf("%.*s", (int)mb->size, mb->buf);
  re_printf("Argument passed: %s\n", (char*)arg);
  return 0;
}

static void signal_handler(int sig)
{
  re_printf("terminating on signal %d...\n", sig);

  /* wait for pending transactions to finish */
  if(dlg) mem_deref(dlg);
  mem_deref(sess_sock);
  sip_close(sip, false);
  return;
}

int main(int argc, const char *argv[])
{
  int err;
  struct sa laddr;

  re_printf("----- libre options ----- \n");

  (void)sys_coredump_set(true);

  err = libre_init();
  if(err){
    re_fprintf(stderr, "re init failed: %s\n", strerror(err));
    goto quit;
  }

  err = sip_alloc(&sip, NULL, 32, 32, 32, "MyRe test", sip_exit_handler, NULL);
  if(err){
    re_fprintf(stderr, "sip error: %s\n", strerror(err));
    goto quit;
  }

  err = net_default_source_addr_get(AF_INET, &laddr);
  if(err) {
    re_fprintf(stderr, "local address error: %s\n", strerror(err));
    goto quit;
  }
  sa_set_port(&laddr, 0);
  re_printf("Local address: %J\n", &laddr);

  err = sip_transp_add(sip, SIP_TRANSP_UDP, &laddr);
  if(err) {
    re_fprintf(stderr, "transport error: %s\n", strerror(err));
    goto quit;
  }

  err = sipsess_listen(&sess_sock, sip, 32, connect_handler, NULL);
  if(err){
    re_fprintf(stderr, "session listen error: %s\n", strerror(err));
    goto quit;
  }

  // create SIP dialog
  err = sip_dialog_alloc(&dlg, URI, TOURI,
                               "Alice User",    // from_name
                               FROMURI,
                               0,               // route vector
                               0 );             // route count
  if(err){
    re_fprintf(stderr, "failed allocate dialog: %s\n", strerror(err));
    goto quit;
  }

  // statefull (arg #3)
  err = sip_drequestf(&sipreq, sip, true, "OPTIONS", dlg,
                      0,                // CSeq. When 0, CSec will be generated
                      NULL,             // SIP authentication state
                      send_handler,     // Send handler
                      resp_handler,     // Response handler
                      "Passing arg",
                      "\r\n");
  re_main(signal_handler);

  if(err){
    re_fprintf(stderr, "failed to send request: %s\n", strerror(err));
    goto quit;
  }

quit:
  if(dlg) mem_deref(dlg);
  mem_deref(sip);
  mem_deref(sess_sock);

  libre_close();

  tmr_debug();
  mem_debug();

  return err;
}
