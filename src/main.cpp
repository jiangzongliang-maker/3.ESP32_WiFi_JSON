#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "../lib/ArduinoJson/ArduinoJson.h"
#include <ESP32_Heweather.h>

// 记得解压ArduinoJson文件夹

// JOSN是一种轻量级的数据交换格式，遵循一下规则：
// 1.并列的数据之间用逗号(,)分隔
// 2.映射用冒号(:)表示
// 3.并列数据的集合(数组)用方括号([])表示
// 4.映射的集合(对象)用大括号({})表示

//	WiFi的初始化和连接
void WiFi_Connect()
{
	WiFi.begin("TP-LINK_255FEC", "88888888");
	while (WiFi.status() != WL_CONNECTED)
	{ //这里是阻塞程序，直到连接成功
		delay(300);
		Serial.print(".");
	}
}

// bilibili api: follower
String UID = "358836343";
String followerUrl = "http://api.bilibili.com/x/relation/stat?vmid=" + UID; // 粉丝数

long follower = 0; // 粉丝数

// wether,直接用和风api会出现乱码，故使用该库
String UserKey = "e168968bf8e94571b980086b5ca7cd83"; // 私钥 https://dev.heweather.com/docs/start/get-api-key
String Location = "101120608";						 // 城市代码 https://github.com/heweather/LocationList,表中的 Location_ID
String Unit = "m";									 // 公制-m/英制-i
String Lang = "zh";									 // 语言 英文-en/中文-zh
float ROUND = 1.5;									 // 更新间隔<分钟>实时天气API 10~20分钟更新一次
WeatherNow weatherNow;

DynamicJsonDocument doc(1024);

//	获取粉丝数
void getBiliBiliFollower()
{
	HTTPClient http;
	http.begin(followerUrl); // HTTP begin
	int httpCode = http.GET();

	if (httpCode > 0)
	{
		// httpCode will be negative on error
		Serial.printf("HTTP Get Code: %d\r\n", httpCode);

		if (httpCode == HTTP_CODE_OK) // 收到正确的内容
		{
			String resBuff = http.getString();

			//	输出示例：{"mid":123456789,"following":226,"whisper":0,"black":0,"follower":867}}
			Serial.println(resBuff);

			//	使用ArduinoJson_6.x版本，具体请移步：https://github.com/bblanchon/ArduinoJson
			deserializeJson(doc, resBuff); //开始使用Json解析
			follower = doc["data"]["follower"];
			Serial.printf("Follers: %ld \r\n", follower);
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}

//	获取天气
void getWeather()
{
	if (weatherNow.get())
	{ // 获取天气更新
		Serial.println(F("======Weahter Now Info======"));
		Serial.print("Server Response: ");
		Serial.println(weatherNow.getServerCode()); // 获取API状态码
		Serial.print(F("Last Update: "));
		Serial.println(weatherNow.getLastUpdate()); // 获取服务器更新天气信息时间
		Serial.print(F("Temperature: "));
		Serial.println(weatherNow.getTemp()); // 获取实况温度
		Serial.print(F("FeelsLike: "));
		Serial.println(weatherNow.getFeelLike()); // 获取实况体感温度
		Serial.print(F("Icon: "));
		Serial.println(weatherNow.getIcon()); // 获取当前天气图标代码
		Serial.print(F("Weather Now: "));
		Serial.println(weatherNow.getWeatherText()); // 获取实况天气状况的文字描述
		Serial.print(F("windDir: "));
		Serial.println(weatherNow.getWindDir()); // 获取实况风向
		Serial.print(F("WindScale: "));
		Serial.println(weatherNow.getWindScale()); // 获取实况风力等级
		Serial.print(F("Humidity: "));
		Serial.println(weatherNow.getHumidity()); // 获取实况相对湿度百分比数值
		Serial.print(F("Precip: "));
		Serial.println(weatherNow.getPrecip()); // 获取实况降水量,毫米
		Serial.println(F("========================"));
	}
	else
	{ // 更新失败
		Serial.println("Update Failed...");
		Serial.print("Server Response: ");
		Serial.println(weatherNow.getServerCode()); // 参考 https://dev.heweather.com/docs/start/status-code
	}
	delay(ROUND * 60000);
}

void setup()
{

	Serial.begin(115200); // open the serial port at 115200 bps;
	delay(100);

	Serial.print("Connecting.. ");

	WiFi_Connect();

	Serial.println("WiFi connected");

	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	weatherNow.config(UserKey, Location, Unit, Lang); // 配置请求信息
}

void loop()
{

	// getBiliBiliFollower();
	getWeather();
	delay(5000);
	delay(5000);
}
