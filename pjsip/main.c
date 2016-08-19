#include <pjsua-lib/pjsua.h>

#define SIP_DOMAIN  "example.com"
#define USER        "alice"
#define PASS        "secret"

/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
  pjsua_perror(__FILE__, title, status);
  pjsua_destroy();
  exit(1);
}

int main()
{
  pjsua_acc_id acc_id;
  pj_status_t status;
  pjsua_logging_config log_cfg;
  pjsua_config cfg;
  pjsua_transport_config transport_cfg;
  pjsua_acc_config acc_cfg;

  status = pjsua_create ();
  PJ_LOG (3, (__FILE__, "========| pjsua_create."));

  pjsua_config_default(&cfg);
  PJ_LOG (3, (__FILE__, "========| pjsua_config_default."));

  pjsua_logging_config_default(&log_cfg);
  log_cfg.console_level = 0;
  PJ_LOG (3, (__FILE__, "========| pjsua_logging_config_default."));

  status = pjsua_init(&cfg, &log_cfg, NULL);
  if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
  PJ_LOG (3, (__FILE__, "========| pjsua_init."));

  pjsua_transport_config_default(&transport_cfg);
	transport_cfg.port = 5060;
  PJ_LOG (3, (__FILE__, "========| pjsua_transport_config_default."));

  status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error creating transport", status);
  PJ_LOG (3, (__FILE__, "========| pjsua_transport_create."));

  /* Initialization is done, now start pjsua */
  status = pjsua_start();
  if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);
  PJ_LOG (3, (__FILE__, "========| pjsua_start."));

  pjsua_acc_config_default(&acc_cfg);
  acc_cfg.id = pj_str("sip:" USER "@" SIP_DOMAIN);
  acc_cfg.reg_uri = pj_str("sip:" SIP_DOMAIN);
  acc_cfg.cred_count = 1;
  acc_cfg.cred_info[0].realm = pj_str(SIP_DOMAIN);
  acc_cfg.cred_info[0].scheme = pj_str("digest");
  acc_cfg.cred_info[0].username = pj_str(USER);
  acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  acc_cfg.cred_info[0].data = pj_str(PASS);

  PJ_LOG (3, (__FILE__, "========| pjsua_acc_config_default."));

  status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
  if (status != PJ_SUCCESS) error_exit("Error adding account", status);
  PJ_LOG (3, (__FILE__, "========| pjsua_acc_add."));

  pjsua_call_hangup_all();

/* done */
  pjsua_destroy ();
  return 0;
}
