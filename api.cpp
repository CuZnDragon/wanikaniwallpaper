#include "api.hpp"
#include <sstream>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>

std::stringstream buffer;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	const char *data = (const char*)ptr;
	buffer.write(data, size * nmemb);
	return size * nmemb;
}

namespace wanikani
{

std::vector<Kanji> API::get(std::string key)
{
	std::vector<Kanji> list;

	CURL *curl = curl_easy_init();
	CURLcode res;

	if(curl)
	{
		std::string url = "http://www.wanikani.com/api/user/" + key + "/kanji";

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		res = curl_easy_perform(curl);
	}
	
	Json::Value root;
	Json::Reader reader;

	reader.parse(buffer, root);

	Json::Value requested_information = root["requested_information"];
	
	for(int i = 0; i < requested_information.size(); i++)
	{
		std::string character = requested_information[i]["character"].asString();
		
		Json::Value stats = requested_information[i]["stats"];
		WaniKaniSRS SRS;
		
		if(stats.isNull())
			SRS = SRS_UNSEEN;
		else
		{
			std::string SRSString = stats["srs"].asString();

			if(SRSString == "apprentice")
				SRS = SRS_APPRENTICE;
			else if(SRSString == "guru")
				SRS = SRS_GURU;
			else if(SRSString == "master")
				SRS = SRS_MASTER;
			else if(SRSString == "enlighten")
				SRS = SRS_ENLIGHTENED;
			else if(SRSString == "burned")
				SRS = SRS_BURNED;
			else
				SRS = SRS_ERROR;
		}
		list.push_back(Kanji(character, SRS));
	}

	return list;
}

}
