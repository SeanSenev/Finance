#include <iostream>
#include <string>
#include <curl/curl.h>
#include <sstream>
#include <vector>
#include <numeric>
#include <cmath>
#include "functions.h"
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

std::vector<float> Closes(string data)
{
  vector<string> result;
  stringstream s_stream(data);
  while(s_stream.good()) {
    string substr;
    getline(s_stream, substr, ','); 
    result.push_back(substr);
  }

  vector<float> closes;
  for(int i = 3; i < result.size(); i++){
    float close = stof(result.at(i).substr(8,12));
    closes.push_back(close);
    i += 5;
  }

  return closes;
}

void printVector(vector<float> closes)
{
  for (std::vector<float>::const_iterator i = closes.begin(); i != closes.end(); ++i)
    std::cout << *i << '\n';
}

std::vector<float> Daily_Returns(vector<float> closes)
{
  vector <float> previous_close;
  for (int i = 1; i < closes.size(); i++)
  {
    previous_close.push_back(closes.at(i));
  }

  vector <float> returns;
  for (int i = 0; i < closes.size()-1 ; i++)
  {
    float return_ = ((previous_close.at(i)/closes.at(i)) - 1 ) * 100;
    returns.push_back(return_);
  }
  return returns;
}

int main ()
{
  string tick, ticker_url, data;
  
  cout << "Please enter ticker symbol " <<endl;
  cin >> tick;

  ticker_url = "https://api.tdameritrade.com/v1/marketdata/" + tick + "/pricehistory?apikey=E8IEMIAEBAQ0XQYROWOABXAJGYMQZYZY&periodType=day&startDate=1557249268";
  data = curl(ticker_url);

  
  vector<float> closes = Closes(data);
  vector<float> returns = Daily_Returns(closes);


}
