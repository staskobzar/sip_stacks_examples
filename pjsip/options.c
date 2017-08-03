/**
 * Simple script to send OPTIONS packet with pjsip library.
 *
 */
#include <pjlib.h>
#include <pjsip.h>

#define NAME      "options"

#define TARGET    "sip:55555@192.168.1.139:5060"
#define FROM      "sip:alice@192.168.1.139"

static void endpt_send_cb(pjsip_send_state *st, pj_ssize_t sent, pj_bool_t *cont)
{
  (void)sent;
  (void)cont;
  char buf[1024];
  pjsip_msg_print(st->tdata->msg, buf, sizeof(buf));
  PJ_LOG(3, (NAME, "Callback sent message:\n\n%s", buf));
}

int main(int argc, const char *argv[])
{
  pj_caching_pool cpool;
  pjsip_endpoint *endpt;
  pj_status_t status;
  pjsip_tx_data *tdata;
  pj_sockaddr_in addr;
  pjsip_transport *tp;
  pj_str_t target = pj_str(TARGET);
  pj_str_t from   = pj_str(FROM);

  // I do not want PJSIP to print its logs.
  // Reset log level
  pj_log_set_level(3);

  pj_init();
  pj_caching_pool_init(&cpool, NULL, 0);
  status = pjsip_endpt_create(&cpool.factory, NAME, &endpt);
  if (status != PJ_SUCCESS) {
    PJ_LOG(3, (NAME, "ERROR: Failed to create end-point."));
    return 1;
  }
  pj_sockaddr_in_init(&addr, NULL, 15060);

  PJ_LOG(3, (NAME, "======= OPTIONS example ======"));
  status = pjsip_endpt_create_request(endpt,
                                      &pjsip_options_method,  // method OPTIONS
                                      &target,                // request URI
                                      &from,                  // from header value
                                      &target,                // to header value
                                      NULL,                   // Contact header
                                      NULL,                   // Call-ID
                                      -1,                     // CSeq
                                      NULL,                   // body
                                      &tdata);

  if (status != PJ_SUCCESS) {
    PJ_LOG(3, (NAME, "ERROR: Failed to create request."));
    return 1;
  }

  status = pjsip_udp_transport_start( endpt, &addr, NULL, 1, &tp);
  if (status != PJ_SUCCESS) {
    PJ_LOG(3, (NAME, "ERROR: Failed to start transport."));
    return 1;
  }
  status = pjsip_endpt_acquire_transport(endpt, PJSIP_TRANSPORT_UDP, &addr, sizeof(addr), NULL, &tp);
  if (status != PJ_SUCCESS) {
    PJ_LOG(3, (NAME, "ERROR: Failed to acquire transport."));
    return 1;
  }

  // this function will call pjsip_transport_send()
  status = pjsip_endpt_send_request_stateless(endpt, tdata, NULL, endpt_send_cb);
  if (status != PJ_SUCCESS) {
    PJ_LOG(3, (NAME, "ERROR: Failed to create request."));
    return 1;
  }

  // done
  pj_caching_pool_destroy(&cpool);

  return 0;
}
