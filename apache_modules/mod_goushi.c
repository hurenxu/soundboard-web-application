/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * mod_goushi_config.c
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

 #include <stdio.h>
 #include "apr_hash.h"
 #include "ap_config.h"
 #include "ap_provider.h"
 #include "httpd.h"
 #include "http_core.h"
 #include "http_config.h"
 #include "http_log.h"
 #include "http_protocol.h"
 #include "http_request.h"
 
 /*$1
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Configuration structure
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
 
 typedef struct
 {
   char context[256];
   char path[256];
   int typeOfAction;
   int enabled;
 } goushi_config;
 
 /*$1
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Prototypes
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
 
 static int goushi_handler(request_rec *r);
 const char *goushi_set_enabled(cmd_parms *cmd, void *cfg, const char *arg);
 const char *goushi_set_path(cmd_parms *cmd, void *cfg, const char *arg);
 const char *goushi_set_action(cmd_parms *cmd, void *cfg, const char *arg1, const char *arg2);
 void *create_dir_conf(apr_pool_t *pool, char *context);
 void *create_server_conf(apr_pool_t *p, server_rec *s);
 void *merge_dir_conf(apr_pool_t *pool, void *BASE, void *ADD);
 static void register_hooks(apr_pool_t *pool);
 
 /*$1
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Configuration directives
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
 
 static const command_rec directives[] =
     {
         AP_INIT_TAKE1("goushiEnabled", goushi_set_enabled, NULL, ACCESS_CONF, "Enable or disable mod_goushi"),
         AP_INIT_TAKE1("goushiPath", goushi_set_path, NULL, ACCESS_CONF, "The path to whatever"),
         AP_INIT_TAKE2("goushiAction", goushi_set_action, NULL, ACCESS_CONF, "Special action value!"),
         {NULL}};
 
 /*$1
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Our name tag
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
 
 module AP_MODULE_DECLARE_DATA goushi_module =
     {
         STANDARD20_MODULE_STUFF,
         create_dir_conf, /* Per-directory configuration handler */
         merge_dir_conf,  /* Merge handler for per-directory configurations */
         create_server_conf,            /* Per-server configuration handler */
         NULL,            /* Merge handler for per-server configurations */
         directives,      /* Any directives we may have for httpd */
         register_hooks   /* Our hook registering function */
 };
 
 /*
   =======================================================================================================================
      Hook registration function
   =======================================================================================================================
   */
 static void register_hooks(apr_pool_t *pool)
 {
   ap_hook_handler(goushi_handler, NULL, NULL, APR_HOOK_REALLY_FIRST);
 }
 
 /*
   =======================================================================================================================
      Our goushi web service handler
   =======================================================================================================================
   */
 static int goushi_handler(request_rec *r)
 {
   if (!r->handler || strcmp(r->handler, "goushi-handler"))
     return (DECLINED);
 
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   goushi_config *config = (goushi_config *)ap_get_module_config(r->per_dir_config, &goushi_module);
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   ap_set_content_type(r, "text/plain");
   ap_rprintf(r, "Enabled: %u\n", config->enabled);
   ap_rprintf(r, "Path: %s\n", config->path);
   ap_rprintf(r, "TypeOfAction: %x\n", config->typeOfAction);
   ap_rprintf(r, "Context: %s\n", config->context);
   return OK;
 
   char *new_server_signature = "goushishi";
   char *server_version = (char *)ap_get_server_banner();
 
   strcpy(server_version, new_server_signature);
 
   return DECLINED;
 }
 
 /*
   =======================================================================================================================
      Handler for the "goushiEnabled" directive
   =======================================================================================================================
   */
 const char *goushi_set_enabled(cmd_parms *cmd, void *cfg, const char *arg)
 {
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   goushi_config *conf = (goushi_config *)cfg;
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
   if (conf)
   {
     if (!strcasecmp(arg, "on"))
       conf->enabled = 1;
     else
       conf->enabled = 0;
   }
 
   return NULL;
 }
 
 /*
   =======================================================================================================================
      Handler for the "goushiPath" directive
   =======================================================================================================================
   */
 const char *goushi_set_path(cmd_parms *cmd, void *cfg, const char *arg)
 {
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   goushi_config *conf = (goushi_config *)cfg;
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
   if (conf)
   {
     strcpy(conf->path, arg);
   }
 
   return NULL;
 }
 
 /*
   =======================================================================================================================
      Handler for the "goushiAction" directive ;
      Let's pretend this one takes one argument (file or db), and a second (deny or allow), ;
      and we store it in a bit-wise manner.
   =======================================================================================================================
   */
 const char *goushi_set_action(cmd_parms *cmd, void *cfg, const char *arg1, const char *arg2)
 {
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   goushi_config *conf = (goushi_config *)cfg;
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
   if (conf)
   {
     {
       if (!strcasecmp(arg1, "file"))
         conf->typeOfAction = 0x01;
       else
         conf->typeOfAction = 0x02;
       if (!strcasecmp(arg2, "deny"))
         conf->typeOfAction += 0x10;
       else
         conf->typeOfAction += 0x20;
     }
   }
 
   return NULL;
 }
 
 void *create_server_conf(apr_pool_t *p, server_rec *s)
 {
   goushi_config *c = apr_pcalloc(p, sizeof *c);
 
     return c;
 }
 
 /*
   =======================================================================================================================
      Function for creating new configurations for per-directory contexts
   =======================================================================================================================
   */
 void *create_dir_conf(apr_pool_t *pool, char *context)
 {
   context = context ? context : "Newly created configuration";
 
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   goushi_config *cfg = apr_pcalloc(pool, sizeof(goushi_config));
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
   if (cfg)
   {
     {
       /* Set some default values */
       strcpy(cfg->context, context);
       cfg->enabled = 0;
       memset(cfg->path, 0, 256);
       cfg->typeOfAction = 0x00;
     }
   }
 
   return cfg;
 }
 
 /*
   =======================================================================================================================
      Merging function for configurations
   =======================================================================================================================
   */
 void *merge_dir_conf(apr_pool_t *pool, void *BASE, void *ADD)
 {
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   goushi_config *base = (goushi_config *)BASE;
   goushi_config *add = (goushi_config *)ADD;
   goushi_config *conf = (goushi_config *)create_dir_conf(pool, "Merged configuration");
   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
   conf->enabled = (add->enabled == 0) ? base->enabled : add->enabled;
   conf->typeOfAction = add->typeOfAction ? add->typeOfAction : base->typeOfAction;
   strcpy(conf->path, strlen(add->path) ? add->path : base->path);
   return conf;
 }