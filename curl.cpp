#include <iostream>
#include <string>
#include <curl/curl.h>


using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


string curl (string ticker_url)
{
  CURL *curl;
    CURLcode res;
    std::string const readBuffer;
    
    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, ticker_url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
      res = curl_easy_perform(curl);
      curl_easy_cleanup(curl);
  }
  return readBuffer;
}

int main ()
{
  string tick, ticker_url, data;
  cout << "Please enter ticker symbol\n";
  cin >> tick;

  ticker_url = "https://api.tdameritrade.com/v1/marketdata/" + tick + "/pricehistory?apikey=E8IEMIAEBAQ0XQYROWOABXAJGYMQZYZY&periodType=day&startDate=1557249268";
  data = curl(ticker_url);
  cout << data;
}
