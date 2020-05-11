#include <iostream>
#include <string>
#include <curl/curl.h>
#include <sstream>
#include <vector>
#include <numeric>
#include <cmath>

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string get_data (string tick)
{
  string ticker_url = "https://api.tdameritrade.com/v1/marketdata/"+ tick + "/pricehistory?apikey=E8IEMIAEBAQ0XQYROWOABXAJGYMQZYZY&periodType=month&frequencyType=daily&startDate=1557408600000";
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

void printVector(vector<float> closes)
{
  for (std::vector<float>::const_iterator i = closes.begin(); i != closes.end(); ++i)
    std::cout << *i << '\n';
}

double Volatility(vector<float> returns){
  double sum = std::accumulate(returns.begin(), returns.end(), 0.0);
  double mean = sum / returns.size();

  vector<float> deviation;
  for(int i = 0; i < returns.size(); i++){
    deviation.push_back((returns.at(i)-mean) * (returns.at(i)-mean));
  }
  double sum1 = std::accumulate(deviation.begin(), deviation.end(), 0.0);

  double volatility = sum1 / deviation.size()-1;

  return std::sqrt(volatility);

}

vector <float> get_Portfolio(string tick1, string tick2, string tick3, string tick4, string tick5)
{
  vector<float> c1 = Daily_Returns(Closes(get_data(tick1)));
  vector<float> c2 = Daily_Returns(Closes(get_data(tick2)));
  vector<float> c3 = Daily_Returns(Closes(get_data(tick3)));
  vector<float> c4 = Daily_Returns(Closes(get_data(tick4)));
  vector<float> c5 = Daily_Returns(Closes(get_data(tick5)));

  vector<float> portfolio_weighted_returns;
  for(int i = 0; i < c1.size(); i++){
    portfolio_weighted_returns.push_back((c1.at(i)+c2.at(i)+c3.at(i)+c4.at(i)+c5.at(i))* float(0.20));
  }

  return (portfolio_weighted_returns);
}

double sumVector(vector<float> returns)
{
  double sum = 0;
  for(int i = 0; i < returns.size(); i++)
  {
    sum += returns.at(i);
  }
  return sum;
}

float getSharpe(vector<float> returns, double volatility)
{
  double sum = std::accumulate(returns.begin(), returns.end(), 0.0);
  double mean = (sum / returns.size());
  double sharpe = (mean -(2/252)) / volatility;

  return sharpe;
}

int main ()
{
  int choice;
  while(true)
  {
    cout << "Press (1) for Single Stock Metrics, press (2) for Portfolio Metrics, press (3) to Exit:" << endl;
    cin >> choice;
    if (choice == 1){
      string ticker;
      cout << "Please enter the desired Ticker Symbol: " ;
      cin >> ticker;

      vector<float> prices = Closes(get_data(ticker));
      cout << "---------------------------------------------" << endl;
      cout << "Printing Closing Prices for " << ticker << endl;
      printVector(prices);
      cout << "---------------------------------------------" << endl;
      
      vector<float> dailys = Daily_Returns(prices);
      double dailys_overall = 0;
      double sum_ = std::accumulate(dailys.begin(), dailys.end(), 0.0);
      double mean_ = (sum_ / dailys.size());
      for(int i = 0; i < dailys.size(); i++)
      {
        dailys_overall += dailys.at(i);
      }    
      cout << "Stock's Overall Returns: " << dailys_overall << + "%" << endl; 
      cout << "---------------------------------------------" << endl;
      cout << "Daily Average Returns: " << mean_<< + "%" << endl; 
      cout << "---------------------------------------------" << endl;
      cout << "Stock's Volatility: " << Volatility(dailys) << + "%" << endl;
      cout << "---------------------------------------------" << endl;
      cout << "Portfolio's Sharpe Ratio is: " << getSharpe(dailys, Volatility(dailys)) << endl;
      cout << "---------------------------------------------" << endl;    
    
    }

    if (choice == 2){
      string ticker_url, data;
      string tick1, tick2, tick3, tick4, tick5;
      
      cout << "Please enter the 5 stocks you would like in your portfolio" << endl;
      cin >> tick1 >> tick2 >> tick3 >> tick4 >> tick5;

      vector<float> portfolio_returns = (get_Portfolio(tick1, tick2, tick3, tick4, tick5));

      cout << "---------------------------------------------" << endl;
      cout << "Printing Portfolio's Daily Returns:" << endl; 
      printVector(portfolio_returns);
      cout << "---------------------------------------------" << endl;
      cout << "Portfolio's Overall Portfolio Returns: " << sumVector(portfolio_returns) << + "%" << endl;
      cout << "---------------------------------------------" << endl;
      cout << "Portfolio's Daily Portfolio Volatility: " << Volatility(portfolio_returns) << + "%" << endl;
      cout << "---------------------------------------------" << endl;
      cout << "Portfolio's Sharpe Ratio is: " << getSharpe(portfolio_returns, Volatility(portfolio_returns)) << endl;
      cout << "---------------------------------------------" << endl;
      double sum = std::accumulate(portfolio_returns.begin(), portfolio_returns.end(), 0.0);
      double mean = (sum / portfolio_returns.size());
      cout << "Portfolio's Average Daily Returns is: " << mean  << + "%" << endl; 
      cout << "---------------------------------------------" << endl;
    }
    if (choice ==3)
    {
      break;
    }
  }
}

// AMD 
// TSLA
// GOOG
// FB
// BYND 