/*
 * admin.c
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "httpd.h"
#include "http_log.h"
#include "http_core.h"
#include "http_protocol.h"
#include "apr_strings.h"

#include <vlad/wrapper.h>

#include "mod_vlad.h"
#include "util.h"
#include "admin.h"
#include "proc.h"

void modvlad_generate_header(request_rec *a_r)
{
  if (!a_r)
    return;

  ap_rprintf(a_r, "%s\n", MODVLAD_XML_HEADER);
  ap_rprintf(a_r, "%s\n", MODVLAD_DOCTYPE_HEADER);
  ap_rprintf(a_r, "%s\n", MODVLAD_MISC_HEADER);
  ap_rprintf(a_r, "<html>\n  <body>\n");
  ap_rprintf(a_r, "    <h1>mod_vlad %s Administration</h1>\n", MODVLAD_VERSION);
  ap_rprintf(a_r, "    <h2>Transformation Sequence Manipulator</h2>\n");
}

void modvlad_generate_footer(request_rec *a_r)
{
  if (!a_r)
    return;

  ap_rprintf(a_r, "  </body>\n</html>\n");
}

void modvlad_generate_form(request_rec *a_r, modvlad_config_rec *a_conf)
{
  unsigned int i;
  unsigned int j;
  unsigned int slen = 0;
  unsigned int tlen = 0;
  unsigned int ilen = 0;
  const char *uri = NULL;

  if (!a_r || !a_conf)
    return;

  if (!(uri = ap_construct_url(a_r->pool, a_r->uri, a_r)))
    return;

  ap_rprintf(a_r, "    <h3>Current Sequence</h3>\n");
  ap_rprintf(a_r, "    <form name=\"delform\" method=\"POST\" action=\"%s\">\n", uri);
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"command\" value=\"delete\"/>\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"index\" value=\"\"/>\n");
  ap_rprintf(a_r, "      <table cols=\"3\" border=\"1\">\n");

  modvlad_client_seq_total(a_r->pool,
                           a_conf->pipe_cli[0],
                           a_conf->pipe_cli[1],
                           a_conf->mutex,
                           &slen);

  for (i = 0; i < slen; i++) {
    const char *st_name = NULL;
    apr_array_header_t *parm = NULL;
    unsigned int pcnt;

    parm = apr_array_make(a_r->pool, 1, sizeof(char *));

    modvlad_client_seq_get(a_r->pool,
                           a_conf->pipe_cli[0],
                           a_conf->pipe_cli[1],
                           a_conf->mutex,
                           i,
                           &st_name,
                           parm);

    ap_rprintf(a_r, "         <tr>\n");
    ap_rprintf(a_r, "           <td align=\"center\">%d</th>\n", i);
    ap_rprintf(a_r, "           <th align=\"center\">\n");
    ap_rprintf(a_r, "             %s(", st_name);

    for (pcnt = 0; pcnt < parm->nelts; pcnt++)
      ap_rprintf(a_r, pcnt == 0 ? "%s" : ", %s", ((char **)parm->elts)[pcnt]);

    ap_rprintf(a_r, ")\n");
    ap_rprintf(a_r, "           </th>\n");


    ap_rprintf(a_r, "           <td align=\"center\"><input type=\"button\" value=\"delete\" onclick=\"delform.index.value=%d;delform.submit();\"/></td>\n", i);
    ap_rprintf(a_r, "         </tr>\n");
  }  

  ap_rprintf(a_r, "      </table>\n");
  ap_rprintf(a_r, "    </form>\n");

  ap_rprintf(a_r, "    <h3>Available Transformations</h3>\n");

  modvlad_client_trans_total(a_r->pool,
                             a_conf->pipe_cli[0],
                             a_conf->pipe_cli[1],
                             a_conf->mutex,
                             &tlen);

  for (i = 0; i <  tlen; i++) {
    const char *tt_name = NULL;
    unsigned int tt_listlen;

    modvlad_client_trans_get(a_r->pool,
                             a_conf->pipe_cli[0],
                             a_conf->pipe_cli[1],
                             a_conf->mutex,
                             i,
                             &tt_name,
                             &tt_listlen);

    ap_rprintf(a_r, "    <form name=\"addform%d\" method=\"POST\" action=\"%s\">\n", i, uri);
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"command\" value=\"add\"/>\n");
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"args\" value=\"%d\"/>\n",  tt_listlen);
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"trans\" value=\"%s\"/>\n", tt_name);
    ap_rprintf(a_r, "      <table cols=\"3\" border=\"1\">\n");
    ap_rprintf(a_r, "        <tr>\n");
    ap_rprintf(a_r, "          <th align=\"center\" colspan=\"2\">%s</th>\n", tt_name);
    ap_rprintf(a_r, "          <th align=\"center\"><input type=\"button\" value=\"add\" onclick=\"addform%d.submit();\"/></th>\n", i);
    ap_rprintf(a_r, "        <tr>\n");

    for (j = 0; j < tt_listlen; j++) {
      ap_rprintf(a_r, "     <tr>\n");
      ap_rprintf(a_r, "       <td align=\"center\">parameter %d</td>\n", j);
      ap_rprintf(a_r, "       <td align=\"center\"><input type=\"text\" name=\"arg%d\" value=\"\" readonly=\"1\"/></td>\n", j);
      ap_rprintf(a_r, "       <td align=\"center\"><input type=\"button\" value=\"set\" onclick=\"addform%d.arg%d.value=document.idform.ident.value;\"/></td>\n", i, j);
      ap_rprintf(a_r, "     </tr>\n");
    }

    ap_rprintf(a_r, "      </table>\n");
    ap_rprintf(a_r, "    </form>\n");
  }

  ap_rprintf(a_r, "    <h3>Available Identifiers</h3>\n");
  ap_rprintf(a_r, "    <form name=\"idform\">\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"ident\" value=\"\"/>\n");
  ap_rprintf(a_r, "      <ul>\n");

  modvlad_client_ident_total(a_r->pool,
                             a_conf->pipe_cli[0],
                             a_conf->pipe_cli[1],
                             a_conf->mutex,
                             &ilen);

  for (i = 0; i < ilen; i++) {
    const char *it_name = NULL;

    modvlad_client_ident_get(a_r->pool,
                             a_conf->pipe_cli[0],
                             a_conf->pipe_cli[1],
                             a_conf->mutex,
                             i,
                             &it_name);

    ap_rprintf(a_r, "        <li>\n");
    ap_rprintf(a_r, "          <input name=\"id\" type=\"radio\" onclick=\"idform.ident.value='%s';\">\n", it_name);
    ap_rprintf(a_r, "            %s\n", it_name);
    ap_rprintf(a_r, "          </input>\n");
    ap_rprintf(a_r, "        </li>\n");
  }

  ap_rprintf(a_r, "      </ul>\n");
  ap_rprintf(a_r, "    </form>\n");
}

void modvlad_handle_form(request_rec *a_r, modvlad_config_rec *a_conf)
{
  char buffer[MODVLAD_MAXSTR_LEN];
  const char *cmd = NULL;
  apr_table_t *tab = NULL;

  memset(buffer, 0, MODVLAD_MAXSTR_LEN);

  ap_get_client_block(a_r, buffer, MODVLAD_MAXSTR_LEN);
  ap_unescape_url(buffer);

  modvlad_parse_args(a_r->pool, buffer, &tab);

  cmd = apr_table_get(tab, "command");

  if (!strcmp(cmd, "add")) {
    unsigned int i;
    unsigned int args;
    const char *name = NULL;
    apr_array_header_t *parms = NULL;

    parms = apr_array_make(a_r->pool, 1, sizeof(char *));

    args = atoi(apr_table_get(tab, "args"));
    name = apr_table_get(tab, "trans");

    for (i = 0; i < args; i++) {
      const char *value = NULL;
      value = apr_table_get(tab, apr_psprintf(a_r->pool, "arg%d", i));

      *(char **) apr_array_push(parms) = apr_pstrdup(a_r->pool, apr_table_get(tab, apr_psprintf(a_r->pool, "arg%d", i)));
    }

    if (modvlad_client_seq_add(a_r->pool,
                               a_conf->pipe_cli[0],
                               a_conf->pipe_cli[1],
                               a_conf->mutex,
                               name,
                               parms) != MODVLAD_OK)
      ap_rprintf(a_r, "    <blink>add error</blink>\n    <br/>\n");
    else
      ap_rprintf(a_r, "    <blink>add successful</blink>\n    <br/>\n");
  }
  else if (!strcmp(cmd, "delete")) {
    unsigned int args;

    args = atoi(apr_table_get(tab, "index"));

    if (modvlad_client_seq_del(a_r->pool,
                               a_conf->pipe_cli[0],
                               a_conf->pipe_cli[1],
                               a_conf->mutex,
                               args) != MODVLAD_OK)
      ap_rprintf(a_r, "    <blink>delete error</blink>\n    <br/>\n");
    else
      ap_rprintf(a_r, "    <blink>delete successful</blink>\n    <br/>\n");
  }
  else
    ap_rprintf(a_r, "    <blink>form error</blink>\n    <br/>\n");

  return;
}
