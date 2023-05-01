/**
   MySpiffs.cpp
   @autor    Bruno Merz

   @version  2.0
   @created  30.12.2019
   @updated  18.04.2021

*/

//#define myDEBUG
#include "Debug.h"
#include "Helper.h"

#include "MySpiffs.h"
#include "MyTime.h"
#if defined(WITH_MQTT)
#include "MyMqtt.h"
extern MyMqtt mymqtt;
#endif

extern MyWifi mywifi;
extern MyTime mytime;
extern boolean reboot;

StaticJsonDocument<JSON_ARRAY_SIZE(JSON_WORDCLOCK_ARRAY_COUNT)+JSON_OBJECT_SIZE(JSON_WORDCLOCK_COUNT)> jsonDoc;
StaticJsonDocument<JSON_ARRAY_SIZE(JSON_RESOURCE_ARRAY_COUNT)+JSON_OBJECT_SIZE(JSON_RESOURCE_COUNT)> jsonResource;

/**
   Constructor, sets webpage to nothing
*/
MySpiffs::MySpiffs(void) {
  fsUploadFile = (File)NULL;
  _reloadResource = true;
}

/**
   Destructor, ends LittleFS
*/
MySpiffs::~MySpiffs(void) {
  LittleFS.end();
}

/**
   Read a Json File and deserialize it
*/
bool MySpiffs::readJson(String _filename, JsonDocument *_jsonDoc) {
  //read Json File
  DEBUG_PRINT("readJson: ");
  if (LittleFS.exists(_filename)) {
    //file exists, reading and loading
    File jsonFile = LittleFS.open(_filename, "r");
    if (jsonFile) {
      DeserializationError error = deserializeJson(*_jsonDoc, jsonFile);
      jsonFile.close();
      if (!error) {
#if defined(myDEBUG)
        serializeJson(*_jsonDoc, DBG_OUTPUT_PORT);
        DEBUG_PRINTLN("");
#endif
        return true;
      } else {
        DEBUG_PRINT("Failed to load json config: ");
        DEBUG_PRINTLN(error.c_str());
      }
    } else {
      DEBUG_PRINTLN("Failed to open " + _filename);
    }
  } else {
    DEBUG_PRINTLN("Coudnt find " + _filename);
  }

  return false;
}


/**
   Write JSON settings to LittleFS
*/
bool MySpiffs::writeSettings(bool saveConfig) {
  if (saveConfig) {
    DEBUG_PRINT("writeSettings: ");
    File configFile = LittleFS.open(JSON_CONFIG, "w");
    if (!configFile) {
      DEBUG_PRINTLN("failed to open config file for writing");
      return false;
    }
#if defined(myDEBUG)
    serializeJson(jsonDoc, DBG_OUTPUT_PORT);
    DEBUG_PRINTLN();
#endif
    serializeJson(jsonDoc, configFile);
    configFile.println();
    DEBUG_PRINTLN("Closing configFile");
    configFile.flush();
    configFile.close();
    return true;
  } else {
    DEBUG_PRINTLN("Didn't save settings");
    return false;
  }
}

/**
   Read settings from LittleFS
*/
bool MySpiffs::readSettings(void) {
  //read settings from File
  
  DEBUG_PRINT("readSettings: ");
  readJson(JSON_CONFIG, &jsonDoc);
  if(LittleFS.exists(JSON_UPDATE)) {
    DynamicJsonDocument doc(1024);
    readJson(JSON_UPDATE, &doc);
    JsonObject root = doc.as<JsonObject>();
    for (JsonPair kv : root) {
      String settingName(kv.key().c_str());
      String settingValue=kv.value().as<String>();
      setSetting(settingName, settingValue);
    }
    doc.clear();
    LittleFS.remove(JSON_UPDATE);
    writeSettings(true);
    reboot=true;
  }
  return true;
}


/**
   removes a file from LittleFS
*/
void MySpiffs::remove(String filename) {
  LittleFS.remove(filename);
}


/**
   open LittleFS file
*/
File MySpiffs::openFile(String filename, char *mode) {
  DEBUG_PRINTLN("openFile: " + filename);
  fsUploadFile = LittleFS.open(filename, mode);
  if (fsUploadFile)
    DEBUG_PRINTLN("openFile: ok");
  return fsUploadFile;
}


/**
   write LittleFS file
*/
void MySpiffs::writeFile(const uint8_t *buf, size_t len) {
  //DEBUG_PRINTLN("writeFile: len"+len);
  if (fsUploadFile)
    fsUploadFile.write(buf, len);
}

/**
   write LittleFS file
*/
void MySpiffs::writeFile(char *buf) {
  //DEBUG_PRINTLN("writeFile: len"+len);
  if (fsUploadFile)
    fsUploadFile.write((const uint8_t*)buf, strlen(buf));
}


/**
   close LittleFS file
*/
void MySpiffs::closeFile(void) {
  DEBUG_PRINTLN("closeFile:");
  if (fsUploadFile) {
    fsUploadFile.flush();
    fsUploadFile.close();
    fsUploadFile = (File)NULL;
   }
}


/**
   read file from LittleFS
*/
size_t MySpiffs::readBytes(uint8_t *buffer, int len) {
  DEBUG_PRINT("readBytes: len=");
  DEBUG_PRINTLN(len);
  size_t l=0;
  if (fsUploadFile) {
    l = fsUploadFile.readBytes((char *)buffer, len);
  }
  return l;
}


/**
   read file from LittleFS
*/
int MySpiffs::readFile(String filename, char *buffer) {
  DEBUG_PRINTLN("readFile: start '"+filename+"'");
  if (LittleFS.exists(filename)) {
    //file exists, reading and loading
    DEBUG_PRINTLN("readFile: reading "+filename);
    File fileObj = LittleFS.open(filename, "r");
    if (fileObj) {
      DEBUG_PRINTLN("readFile: File opened!");
      _fileSize = fileObj.size();
      DEBUG_PRINT("#bytes=");
      DEBUG_PRINTLN(_fileSize);
      int x=fileObj.readBytes(buffer,_fileSize);
      DEBUG_PRINT("readFile: readBytes result=");
      DEBUG_PRINTLN(x);
      *(buffer+_fileSize)='\0';
      fileObj.close();
    }
  } else {
    DEBUG_PRINTLN("readFile: no file in FS");
    *buffer   = '\0';      // file not found
    _fileSize = 0;
  }
  return _fileSize;
}

int MySpiffs::readFileS(String filename, String *buffer) {
  DEBUG_PRINTLN("readFileS: start '"+filename+"'");
  if (LittleFS.exists(filename)) {
    //file exists, reading and loading
    DEBUG_PRINTLN("readFileS: reading "+filename);
    File fileObj = LittleFS.open(filename, "r");
    if (fileObj) {
      DEBUG_PRINTLN("readFileS: File opened!");
      *buffer = fileObj.readString();
      fileObj.close();
    }
  } else {
    DEBUG_PRINTLN("readFileS: no file in FS");
    *buffer="";   // file not found
  }
  return buffer->length();
}


/**
   format LittleFS
*/
void MySpiffs::format(void) {
  bool formatted = LittleFS.format();
  if (formatted) {
    DEBUG_PRINTLN("\n\nSuccess formatting");
  } else {
    DEBUG_PRINTLN("\n\nError formatting");
  }
}


/**
   file exists?
*/
bool MySpiffs::exists(String filename) {
  return LittleFS.exists(filename);
}


/**
   set a setting Name=Value
*/
void MySpiffs::setSetting(String settingName, String settingValue) {
  boolean reboot = false;
  if(settingName == "hostname") {   // hostname changed?
    if(settingValue != jsonDoc[settingName].as<String>())
        reboot = true;
  }
  jsonDoc[settingName] = settingValue;

  if(reboot) {
    writeSettings(true);
    ESP.restart();
  }
}

/**
   set rgb setting Name=Value
*/
void MySpiffs::setSetting(String settingName, COLOR_T color) {
  char rgbValue[10];
  sprintf(rgbValue,"#%.6x",color);
  jsonDoc[settingName] = rgbValue;

}

/**
   set all settings
*/
void MySpiffs::setSettings(String settings) {
  DEBUG_PRINTLN("setSettings spiffs: '" + settings + "'");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, settings);
  JsonObject root = doc.as<JsonObject>();

  for (JsonPair kv : root) {
    String settingName(kv.key().c_str());
    String settingValue=kv.value().as<String>();
    setSetting(settingName, settingValue);

    //if(settingName == "ticker_text") {
    //  ticker.setTickerText(settingValue);
    //}
    if(settingName == "time") {
      mytime.setTime((char *)settingValue.c_str());
    }
  }

  writeSettings(true);
}


/**
   get value for a specific name
*/
String MySpiffs::getSetting(String settingName) {
  JsonVariant jv = jsonDoc[settingName];
  if (jv.isNull())
    _settingValue = "";
  else
    _settingValue = jv.as<String>();
    
  DEBUG_PRINTLN("getSetting spiffs: "+settingName+"='"+_settingValue+"'");
  return _settingValue;
}


/**
   get char* value for a specific name
*/
int MySpiffs::getCharSetting(String settingName, char *value, int lng) {
  JsonVariant jv = jsonDoc[settingName];
  if (jv.isNull())
    *value = '\0';
  else
    strncpy(value, jv.as<String>().c_str(), lng);
  return strlen(value);
}
/**
   get int value for a specific name
*/
int MySpiffs::getIntSetting(String settingName) {
  JsonVariant jv = jsonDoc[settingName];
  if (jv.isNull())
    return -1;
  else
    return strtol(jv.as<String>().c_str(), NULL, 0);
}


/**
   get rgb value for a specific name
*/
COLOR_T MySpiffs::getRGBSetting(String settingName) {
  COLOR_T c;
  JsonVariant jv = jsonDoc[settingName];
  if (jv.isNull())
    c = (COLOR_T) 0xffffff;
  else {
    _settingValue = jv.as<String>();
    if(_settingValue.startsWith("#"))
      c = (COLOR_T) strtol( &_settingValue.c_str()[1], NULL, 16);   // #FFAABB
    else
      c = (COLOR_T) strtol( _settingValue.c_str(), NULL, 0);        // 0xFFAABB or 16755387
  }

  return c;
}


/**
   get boolean value for a specific name
*/
bool MySpiffs::getBoolSetting(String settingName) {
  bool ret=false;
  JsonVariant jv = jsonDoc[settingName];
  if (!jv.isNull()) {
    _settingValue = jv.as<String>();
    if (_settingValue.equalsIgnoreCase("true"))
      ret=true;
    else if(_settingValue.equalsIgnoreCase("on"))
      ret=true;
    else if (_settingValue.equals("1"))
      ret=true;
  }
  DEBUG_PRINTF("getBoolSetting: %s=%d\n",settingName.c_str(), ret);
  return ret;
}


/**
   get double value for a specific name
*/
double MySpiffs::getDoubleSetting(String settingName) {
  double ret=false;
  JsonVariant jv = jsonDoc[settingName];
  if (jv.isNull())
      return -1;
  else
    return jv.as<String>().toDouble();
}


/**
   clear value for a specific name
*/
void MySpiffs::removeSetting(String settingName) {
  jsonDoc.remove(settingName);
  writeSettings(true);
}

/**
   get all settings
*/
String MySpiffs::getSettings(void) {
  _settings = "";
  size_t numBytes = serializeJson(jsonDoc, _settings);
  if (numBytes)
    DEBUG_PRINTLN("getSettings spiffs: '" + _settings + "'");
  else {
    DEBUG_PRINTLN("getSettings spiffs: Failed to serialize jsonDoc");
  }
  return _settings;
}

/**
   get state
*/
String MySpiffs::getState(void) {
  File file;
  char txt[100];
  int total=0;

  const static char json[] PROGMEM = "\n\nJSON:\n";
  const static char files[] PROGMEM = "\n\nFILES:\n";
  const static char sz[] PROGMEM = ", size=";
  const static char ce[] PROGMEM = ", creation=";
  const static char la[] PROGMEM = ", last write=";
  const static char nl[] PROGMEM = "\n";
  const static char eq[] PROGMEM = "=";
  const static char cb[] PROGMEM = ", ";
  const static char az[] PROGMEM = "\nAll files total size=";
  const static char tb[] PROGMEM = "\nFilesystem total bytes=";
  const static char ub[] PROGMEM = "\nFilesystem used bytes=";
  const static char fb[] PROGMEM = "\nFilesystem free bytes=";
  const static char st[] PROGMEM = "****";
  const static char mo[] PROGMEM = "\nMax open Files=";

  Helper::writeState(json);

  JsonObject root = jsonDoc.as<JsonObject>();

  for (JsonPair kv : root) {
    //Helper::writeState(kv.key().c_str(),eq);
    writeFile((char *)kv.key().c_str());
    Helper::writeState(eq);
    if(kv.key() == "wifi_pass") {
      Helper::writeState(st);
      Helper::writeState(cb);
    } else {
      if(kv.value().as<String>().length()) {
        writeFile((char *)kv.value().as<String>().c_str());
      }
      Helper::writeState(cb);
    }
  }

  Helper::writeState(files);

#if defined(ESP32)
  File rt = LittleFS.open("/");
  file = rt.openNextFile();
  while (file) {
    sprintf(txt,"%-20s", file.name());
    Helper::writeState(nl, txt);
    sprintf(txt,"%.6d", file.size());
    Helper::writeState(sz, txt);
    //time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();

    //struct tm * tmstruct = localtime(&cr);
    //sprintf(txt,"%d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    //Helper::writeState(ce, txt);
    struct tm * tmstruct = localtime(&lw);
    sprintf(txt,"%d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    Helper::writeState(la, txt);

    total+=file.size();
    file = rt.openNextFile();
  }
  Helper::writeState(az, total);
  Helper::writeState(tb, LittleFS.totalBytes());
  Helper::writeState(ub, LittleFS.usedBytes());
#else
  FSInfo fs_info;
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    sprintf(txt,"%-20s", dir.fileName().c_str());
    Helper::writeState(nl, txt);
    sprintf(txt,"%.6d", dir.fileSize());
    Helper::writeState(sz, txt);
 
    file = LittleFS.open(dir.fileName(), "r");
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();

    struct tm * tmstruct = localtime(&cr);
    sprintf(txt,"%d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    Helper::writeState(ce, txt);
    tmstruct = localtime(&lw);
    sprintf(txt,"%d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    Helper::writeState(la, txt);

    //Helper::writeState(nl);
    total+=dir.fileSize();
  }
  Helper::writeState(az, total);
  LittleFS.info(fs_info);
  Helper::writeState(tb, fs_info.totalBytes);
  Helper::writeState(ub, fs_info.usedBytes);
  Helper::writeState(fb, fs_info.totalBytes-fs_info.usedBytes);
  Helper::writeState(mo, fs_info.maxOpenFiles);
  
#endif
  return "";
}

uint32_t MySpiffs::getFreeSpace(void) {
#if defined(ESP8266)
  FSInfo fs_info;
  LittleFS.info(fs_info);
  return fs_info.totalBytes-fs_info.usedBytes;
#else
  return LittleFS.totalBytes()-LittleFS.usedBytes();
#endif
}


/**
   getLanguage
*/
String MySpiffs::getLanguage(void) {
  const static char languages[][3] PROGMEM = {"de","de","de","de","ch","gb","gb","fr","it","nl","es","pt","gr","ae"};
  int lang = getIntSetting(F("language"));
  if(lang<0 || lang > LANGUAGE_COUNT)
    lang = 0;
  strcpy_P(_lang, &(languages[lang][0]));
  return _lang;
}


/**
   getResource
*/
String MySpiffs::getResource(String key) {
  return getResource(key, key);
}


/**
   getResource
*/
String MySpiffs::getResource(String key, String std) {
  if(_reloadResource) {
    readJson(JSON_RESOURCE, &jsonResource);
    _reloadResource = false;
  }

  String value = jsonResource[getLanguage()][key].as<String>();
  if(value == "null")
    return std;
  else
    return value;
}

/**
   setResourceFalse
*/
void MySpiffs::reloadResource(void) {
  _reloadResource = true;
}


/**
   getHtmlFilename checks if Update.htm is required
*/
String MySpiffs::getHtmlFilename(String html, bool checkUpdate) {
  if(checkUpdate && LittleFS.exists("/result.html") && LittleFS.exists("/update.html")) {
    return("/update.html");
  }
  else {
    return "/" + html + ".html";
  }
}


/**
   cleanup unneccessary files
*/
void MySpiffs::cleanup(void) {
#if defined(ESP32)
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while(file){
    String fn(file.name());
    if(fn.endsWith(".tar") || fn.endsWith(".bin") || fn.endsWith(".txt"))
      LittleFS.remove(file.name());
    file = root.openNextFile();
  }
#else
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    String fn(dir.fileName());
    if(fn.endsWith(".tar") || fn.endsWith(".bin") || fn.endsWith(".txt"))
      LittleFS.remove(dir.fileName());
  }
#endif
}

/**
   read file size from LittleFS
*/
int MySpiffs::fileSize(String filename) {
  int fs=-1;
  if (LittleFS.exists(filename)) {
    File fileObj = LittleFS.open(filename, "r");
    if (fileObj) {
      fs=fileObj.size();
      fileObj.close();
    }
  }
  return fs;
}


/**
   Log Funktions
*/
void MySpiffs::writeLog(uint32_t val, boolean withDateTime, boolean hex) {
  char dest[100];
  if(hex)
    sprintf(dest,"#%.6x",val);
  else
    sprintf(dest,"%d",val);
  writeLog(dest, withDateTime);
}

void MySpiffs::writeLog(const __FlashStringHelper *src, boolean withDateTime) {
  char dest[100];
  strncpy_P(dest, (char *)src, sizeof(dest));
  writeLog(dest, withDateTime);
}


void MySpiffs::writeLog(char *txt, boolean withDateTime) {
  logHandle = LittleFS.open(LOG_FILE, "a");

  if(logHandle) {
    if(logHandle.size() > 20000) {
      logHandle.close();
      LittleFS.rename(LOG_FILE, LOG_FILE_OLD);
      logHandle = LittleFS.open(LOG_FILE, "w");
    }
    if(withDateTime) {
      String t =  mytime.getDate();
      logHandle.print(t.substring(0, t.length()-1).c_str());
      logHandle.print(": ");
      t =  mytime.getTime();
      logHandle.print(t.c_str());
      logHandle.print(": ");
    }
    logHandle.print(txt);
    logHandle.flush();
    logHandle.close();
  }
}
