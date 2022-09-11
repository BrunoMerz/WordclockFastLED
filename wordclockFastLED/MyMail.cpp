/**

   MyMail.cpp
   @autor    Bruno Merz

   @version  1.0
   @created  15.02.2021
   @updated  

   Version history:
   V 1.0:  - Created.
   
   Receive/Send Email
*/

#define myDEBUG
#include "Debug.h"

#include "Configuration.h"

#ifdef WITH_MAIL

#include "MyMail.h"


extern MyDisplay display;
extern Ticker ticker;
extern IconRenderer iconRenderer;
extern Effekte effekte;
extern MySpiffs myspiffs;
extern int lastMinutes;

extern word matrix[16];


void connectWiFi()
{
    WiFi.disconnect();

    Serial.print("Connecting to AP");
/*
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(200);
    }
*/
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
}

/* Callback function to get the Email reading status */
void imapCallback(IMAP_Status status)
{
    /* Print the current status */
    Serial.println(status.info());

    /* Show the result when reading finished */
    if (status.success())
    {
        /* Print the result */
        /* Get the message list from the message list data */
        //IMAP_MSG_List msgList = imap.data();
        //printMessages(msgList.msgItems, imap.headerOnly());

        /* Clear all stored data in IMAPSession object */
        //imap.empty();
    }
}

/*
 * Constructor
 */
MyMail::MyMail(void) {
  _initDone = false;
  _lastTime = millis();
  _imap = 0;
  _config = 0;
  _session = 0;
}

boolean MyMail::init(void) {
  // Create objects
  _imap    = new IMAPSession();
  _config  = new IMAP_Config();
  _session = new ESP_Mail_Session();

  /*  Set the network reconnection option */
  MailClient.networkReconnect(true);
  _imap->debug(1);
  _imap->callback(imapCallback);

  // IMAP Host
  _imapHost = myspiffs.getSetting(F("imapHost"));
  if(!_imapHost.length())
    return false;
  _session->server.host_name = _imapHost.c_str();
  DEBUG_PRINTF("MyMail::init hostName=<%s>\n", _session->server.host_name.c_str());

  // IMAP Port
  _imapPort = myspiffs.getIntSetting(F("imapPort"));
  if(_imapPort == 65535)
    _imapPort = 993;
  _session->server.port = _imapPort;
  DEBUG_PRINTF("MyMail::init imapPort=<%d>\n", _session->server.port);

  //  IMAP Email Adresse
  _imapMail = myspiffs.getSetting(F("imapMail"));
  if(!_imapMail.length())
    return false;
  _session->login.email = _imapMail.c_str();
  DEBUG_PRINTF("MyMail::init imapMail=<%s>\n", _session->login.email.c_str());

  //  IMAP Password
  _imapPassword = myspiffs.getSetting(F("imapPassword"));
  if(!_imapMail.length())
    return false;
  _session->login.password = _imapPassword.c_str();
  DEBUG_PRINTF("MyMail::init imapPassword=<%s>\n", _session->login.password.c_str());

  // IMAP  Folder
  _folder = myspiffs.getSetting(F("imapFolder"));
  if(!_folder.length())
    _folder = "INBOX";
  DEBUG_PRINTF("MyMail::init Folder=<%s>\n", _folder.c_str());
  
  _session->time.ntp_server = myspiffs.getSetting(F("ntp_server"));
  _session->time.gmt_offset = 1;
  _session->time.day_light_offset = 1;
 
  _session->network_connection_handler = connectWiFi;


  _folder  = "INBOX";
  _session->server.host_name = "imap.gmx.net";
  _session->server.port = 993;
  _session->login.email = "bmerz51@gmx.de";
  _session->login.password = "Gmxzr810$10";


  _config->fetch.uid = "";
  _config->search.criteria = "UID SEARCH ALL";
  _config->search.unseen_msg = true;
  _config->storage.type = esp_mail_file_storage_type_none;
  _config->download.header = true;
  _config->download.text = false;
  _config->download.html = false;
  _config->download.attachment = false;
  _config->download.inlineImg = false;
  _config->enable.html = false;
  _config->enable.text = false;

  /* Set to enable the sort the result by message UID in the ascending order */
  _config->enable.recent_sort = true;

  /* Set to report the download progress via the default serial port */
  _config->enable.download_status = false;

  /* Set the limit of number of messages in the search results */
  _config->limit.search = 10; // 50;
  
  /** Set the maximum size of message stored in 
     * IMAPSession object in byte
  */
  _config->limit.msg_size = 16; //512;
  /** Set the maximum attachments and inline images files size
     * that can be downloaded in byte. 
     * The file which its size is largger than this limit may be saved 
     * as truncated file.
  */
  _config->limit.attachment_size = 0; //1024 * 1024 * 5;


  _imap->setNetworkStatus(true);

  _initDone = true;
  DEBUG_PRINTLN("Mail init done");
  return true;
}

boolean MyMail::check(void) {
  IMAP_MSG_List msgList;
  unsigned long newTime;

  if(!_initDone)
    if(!init()) {
      DEBUG_PRINTLN("init Mail failed");
      return false;
    }
  DEBUG_PRINTLN("check now");
  DEBUG_FLUSH();

  if(strlen(_session->server.host_name.c_str())) {
    newTime = millis();
    if(newTime < _lastTime+30000) //180000
      return true;
    _lastTime = newTime;
    
    DEBUG_PRINTF("Trying to read mails from '%s'\n", _session->server.host_name.c_str());
    DEBUG_FLUSH();
    
    /* Connect to server with the session and config */
    if (!_imap->connect(_session, _config)) {
      DEBUG_PRINTLN("connect failed: "+_imap->errorReason());
      return false;
    }

    if (!_imap->selectFolder(_folder.c_str())) {
      DEBUG_PRINTLN("selectFolder failed: "+_imap->errorReason());
      return false;
    }

    MailClient.readMail(_imap, false);
    /* Clear all stored data in IMAPSession object */

    msgList = _imap->data();
    DEBUG_PRINTF("Anzahl Mails %d\n",msgList.msgItems.size());

  
    for (size_t i = 0; i < msgList.msgItems.size(); i++)
    {
        /* Iterate to get each message data through the message item data */
        IMAP_MSG_Item msg = msgList.msgItems[i];

        Serial.println("################################");
        DEBUG_PRINTF("Message Number: %s\n", msg.msgNo);
        DEBUG_PRINTF("Message UID: %s\n", msg.UID);
        DEBUG_PRINTF("Message ID: %s\n", msg.ID);
        DEBUG_PRINTF("Accept Language: %s\n", msg.acceptLang);
        DEBUG_PRINTF("Content Language: %s\n", msg.contentLang);
        DEBUG_PRINTF("From: %s\n", msg.from);
        DEBUG_PRINTF("From Charset: %s\n", msg.fromCharset);
        DEBUG_PRINTF("To: %s\n", msg.to);
        DEBUG_PRINTF("To Charset: %s\n", msg.toCharset);
        DEBUG_PRINTF("CC: %s\n", msg.cc);
        DEBUG_PRINTF("CC Charset: %s\n", msg.ccCharset);
        DEBUG_PRINTF("Date: %s\n", msg.date);
        DEBUG_PRINTF("Subject: %s\n", msg.subject);
        DEBUG_PRINTF("Subject Charset: %s\n", msg.subjectCharset);

        /* If the result contains the message info (Fetch mode) */
        if (!_imap->headerOnly())
        {
            DEBUG_PRINTF("Text Message: %s\n", msg.text.content);
            DEBUG_PRINTF("Text Message Charset: %s\n", msg.text.charSet);
            DEBUG_PRINTF("Text Message Transfer Encoding: %s\n", msg.text.transfer_encoding);
            DEBUG_PRINTF("HTML Message: %s\n", msg.html.content);
            DEBUG_PRINTF("HTML Message Charset: %s\n", msg.html.charSet);
            DEBUG_PRINTF("HTML Message Transfer Encoding: %s\n\n", msg.html.transfer_encoding);

//            if (msg.attachments.size() > 0)
//                printAttacements(msg);

//            if (msg.rfc822.size() > 0)
//                printRFC822Messages(msg);
        }

        DEBUG_PRINTLN();
    }
    _imap->empty();
    /* Close the seeion in case the session is still open */
    _imap->closeSession();
  }
  return true;
}

#endif
