/**
   MyMail.h
   @autor    Bruno Merz

   @version  1.0
   @created  15.02.2021
   @updated  

   Version history:
   V 1.0:  - Created.
   
   Receive/Send Email

*/

#pragma once

#include <ESP_Mail_Client.h>

#include "MySpiffs.h"
#include "Ticker.h"
#include "MyDisplay.h"
#include "Effekte.h"


/**
 * Class definition
 */
class MyMail {
  public:
    MyMail(void);
    boolean init(void);
    boolean check(void);

  private:
    IMAPSession *_imap;
    ESP_Mail_Session *_session;
    IMAP_Config *_config;

    String _imapHost;
    String _imapMail;
    String _imapPassword;
    uint16_t _imapPort;

    String _folder;
    boolean _initDone;
    boolean _showMailCount;
    boolean _showSubject;
    unsigned long _lastTime;
};
