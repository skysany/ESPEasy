#include "../Static/WebStaticData.h"

#include "../Globals/Cache.h"
#include "../Helpers/ESPEasy_Storage.h"
#include "../WebServer/HTML_wrappers.h"
#include "../WebServer/LoadFromFS.h"

String generate_external_URL(const String& fname) {
    String url;
    url.reserve(80 + fname.length());
    url = F("https://cdn.jsdelivr.net/gh/letscontrolit/ESPEasy@mega-20211224/static/");
    url += fname;
    return url;
}


void serve_CSS() {
  const String cssFile = F("esp.css");
  if (fileExists(cssFile))
  {
    addHtml(F("<style>"));
    streamFromFS(cssFile);
    addHtml(F("</style>"));
    return;
  }
  #ifndef WEBSERVER_CSS
  addHtml(F("<link"));
  addHtmlAttribute(F("rel"), F("stylesheet"));
  addHtmlAttribute(F("type"), F("text/css"));
  addHtmlAttribute(F("href"), generate_external_URL(F("espeasy_default.css")));
  addHtml('/');
  addHtml('>');
  #else
  addHtml(F("<style>"));

  // Send CSS in chunks
  TXBuffer.addFlashString((PGM_P)FPSTR(DATA_ESPEASY_DEFAULT_MIN_CSS));
  addHtml(F("</style>"));
  #endif
}

void serve_favicon() {
  #ifdef WEBSERVER_FAVICON_CDN
  addHtml(F("<link"));
  addHtmlAttribute(F("rel"), F("icon"));
  addHtmlAttribute(F("type"), F("image/x-icon"));
  addHtmlAttribute(F("href"), generate_external_URL(F("favicon.ico")));
  addHtml('/');
  addHtml('>');
  #endif
}

void serve_JS(JSfiles_e JSfile) {
    String url;
    switch (JSfile) {
        case JSfiles_e::UpdateSensorValuesDevicePage:
          url = F("update_sensor_values_device_page.js");
          break;
        case JSfiles_e::FetchAndParseLog:
          url = F("fetch_and_parse_log.js");
          break;
        case JSfiles_e::SaveRulesFile:
          url = F("rules_save.js");
          break;
        case JSfiles_e::GitHubClipboard:
          url = F("github_clipboard.js");
          break;
        case JSfiles_e::Reboot:
          url = F("reboot.js");
          break;
        case JSfiles_e::Toasting:
          url = F("toasting.js");
          break;
    }

    if (!fileExists(url))
    {
        #ifndef WEBSERVER_INCLUDE_JS
        url = generate_external_URL(url);
        addHtml(F("<script"));
        addHtml(F(" defer"));
        addHtmlAttribute(F("src"), url);
        addHtml('>');
        html_add_script_end();
        return;
        #else
        html_add_script(true);
        switch (JSfile) {
          case JSfiles_e::UpdateSensorValuesDevicePage:
            #ifdef WEBSERVER_DEVICES
            TXBuffer.addFlashString((PGM_P)FPSTR(DATA_UPDATE_SENSOR_VALUES_DEVICE_PAGE_JS));
            #endif
            break;
          case JSfiles_e::FetchAndParseLog:
            #ifdef WEBSERVER_LOG
            TXBuffer.addFlashString((PGM_P)FPSTR(DATA_FETCH_AND_PARSE_LOG_JS));
            #endif
            break;
          case JSfiles_e::SaveRulesFile:
            #ifdef WEBSERVER_RULES
            TXBuffer.addFlashString((PGM_P)FPSTR(jsSaveRules));
            #endif
            break;
          case JSfiles_e::GitHubClipboard:
            #ifdef WEBSERVER_GITHUB_COPY
            TXBuffer.addFlashString((PGM_P)FPSTR(DATA_GITHUB_CLIPBOARD_JS));
            #endif
            break;
          case JSfiles_e::Reboot:
            TXBuffer.addFlashString((PGM_P)FPSTR(DATA_REBOOT_JS));
            break;
          case JSfiles_e::Toasting:
            TXBuffer.addFlashString((PGM_P)FPSTR(jsToastMessageBegin));
            // we can push custom messages here in future releases...
            addHtml(F("Submitted"));
            TXBuffer.addFlashString((PGM_P)FPSTR(jsToastMessageEnd));
            break;

        }
        html_add_script_end();
        return;
        #endif
    }
    // Now stream the file directly from the file system.
    html_add_script(false);
    streamFromFS(url);
    html_add_script_end();
}