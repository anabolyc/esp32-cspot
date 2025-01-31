#include "ConfigJSON.h"
#include "JSONObject.h"
#include "Logger.h"
#include "ConstantParameters.h"

// provide weak deviceId (see ConstantParameters.h)
#if _MSC_VER
char deviceId[] = "142137fd329622137a14901634264e6f332e2411";
#else
char deviceId[] __attribute__((weak)) = "142137fd329622137a14901634264e6f332e2411";
#endif

ConfigJSON::ConfigJSON(std::string jsonFileName, std::shared_ptr<FileHelper> file)
{
    _file = file;
    _jsonFileName = jsonFileName;
}

bool ConfigJSON::load()
{
    // Config filename check
    if(_jsonFileName.length() > 0)
    {
      std::string jsonConfig;
      _file->readFile(_jsonFileName, jsonConfig);

      // Ignore config if empty
      if(jsonConfig.length() > 0)
      {
        auto root = cJSON_Parse(jsonConfig.c_str());

        if(cJSON_HasObjectItem(root, "deviceName"))
        {
          auto deviceNameObject = cJSON_GetObjectItemCaseSensitive(root, "deviceName");
          this->deviceName = std::string(cJSON_GetStringValue(deviceNameObject));
        }
        if(cJSON_HasObjectItem(root, "bitrate"))
        {
          auto bitrateObject = cJSON_GetObjectItemCaseSensitive(root, "bitrate");
          switch((uint16_t)cJSON_GetNumberValue(bitrateObject)){
            case 320:
              this->format = AudioFormat_OGG_VORBIS_320;
              break;
            case 160:
              this->format = AudioFormat_OGG_VORBIS_160;
              break;
            case 96:
              this->format = AudioFormat_OGG_VORBIS_96;
              break;
            default:
              this->format = AudioFormat_OGG_VORBIS_320;
              break;
          }
        }
        if(cJSON_HasObjectItem(root, "volume"))
        {
          auto volumeObject = cJSON_GetObjectItemCaseSensitive(root, "volume");
          this->volume = cJSON_GetNumberValue(volumeObject);
        }
        if (this->deviceId.empty()) this->deviceId = ::deviceId;
        cJSON_Delete(root);
      }
      else
      {
         // Config file not found or invalid
         // Set default values
         this->volume = 32767;
         this->deviceName = defaultDeviceName;
         this->format = AudioFormat_OGG_VORBIS_160;
         this->deviceId = ::deviceId;
      }
      return true;
    }
    else
    {
      return false;
    }
}

bool ConfigJSON::save()
{
    bell::JSONObject obj;

    obj["volume"] = this->volume;
    obj["deviceName"] = this->deviceName;
    switch(this->format){
        case AudioFormat_OGG_VORBIS_320:
            obj["bitrate"] = 320;
            break;
        case AudioFormat_OGG_VORBIS_160:
            obj["bitrate"] = 160;
            break;
        case AudioFormat_OGG_VORBIS_96:
            obj["bitrate"] = 96;
            break;
        default:
            obj["bitrate"] = 160;
            break;
    }

    return _file->writeFile(_jsonFileName, obj.toString());
}
