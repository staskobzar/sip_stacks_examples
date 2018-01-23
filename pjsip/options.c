/**
 * Simple script to send OPTIONS packet with pjsip library.
 *
 */
#include <pjlib.h>
#include <pjsip.h>

#define NAME      "options"

#define TARGET    "sip:55555@192.168.1.139:5060"
#define FROM      "sip:alice@192.168.1.139"

static pj_bool_t RESPONDED = PJ_FALSE;
static pj_bool_t logging_on_rx_msg(pjsip_rx_data *rdata);
static pj_status_t logging_on_tx_msg(pjsip_tx_data *tdata);
static pjsip_module msg_logger =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-msg-log", 13 },      /* Name.    */
  -1,                         /* Id      */
  PJSIP_MOD_PRIORITY_TRANSPORT_LAYER-1, /* Priority          */
  NULL,                       /* load()    */
  NULL,                       /* start()    */
  NULL,                       /* stop()    */
  NULL,                       /* unload()    */
  &logging_on_rx_msg,         /* on_rx_request()  */
  &logging_on_rx_msg,         /* on_rx_response()  */
  &logging_on_tx_msg,         /* on_tx_request.  */
  &logging_on_tx_msg,         /* on_tx_response()  */
  NULL,                       /* on_tsx_state()  */

};

/* Notification on incoming messages */
static pj_bool_t logging_on_rx_msg(pjsip_rx_data *rdata)
{
  PJ_LOG(3,(NAME, "RX %d bytes %s from %s %s:%d:\n"
        "%.*s\n"
        "--end msg--",
        rdata->msg_info.len,
        pjsip_rx_data_get_info(rdata),
        rdata->tp_info.transport->type_name,
        rdata->pkt_info.src_name,
        rdata->pkt_info.src_port,
        (int)rdata->msg_info.len,
        rdata->msg_info.msg_buf));

  RESPONDED = PJ_TRUE;

  return PJ_FALSE;
}

/* Notification on outgoing messages */
static pj_status_t logging_on_tx_msg(pjsip_tx_data *tdata)
{
  PJ_LOG(3,(NAME, "TX %d bytes %s to %s %s:%d:\n"
        "%.*s\n"
        "--end msg--",
        (tdata->buf.cur - tdata->buf.start),
        pjsip_tx_data_get_info(tdata),
        tdata->tp_info.transport->type_name,
        tdata->tp_info.dst_name,
        tdata->tp_info.dst_port,
        (int)(tdata->buf.cur - tdata->buf.start),
        tdata->buf.start));
  return PJ_SUCCESS;
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

  status = pjsip_endpt_register_module(endpt, &msg_logger);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

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
  status = pjsip_endpt_send_request_stateless(endpt, tdata, NULL, NULL);
  if (status != PJ_SUCCESS) {
    PJ_LOG(3, (NAME, "ERROR: Failed to create request."));
    return 1;
  }

  // main loop. Exit on response.
  for (;;) {
    pjsip_endpt_handle_events(endpt, NULL);
    if (RESPONDED == PJ_TRUE) break;
  }
  // done
  pj_caching_pool_destroy(&cpool);

  return 0;
}
