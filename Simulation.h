#pragma once

#include <concepts>
#include <stdexcept>
#include <vector>

enum class TradeType
{
	Long,
	Short
};

struct Trade
{
	double EntryTimestamp;
	double ExitTimestamp;
	float EntryPrice;
	float ExitPrice;
	TradeType Type;
};

template<class T>
concept SimulatorAPI = requires(T API)
{
	{T::Candle::Timestamp} -> std::same_as<double>;
	{T::Candle::Open} -> std::same_as<float>;
	{T::Candle::Close} -> std::same_as<float>;
	{T::Candle::High} -> std::same_as<float>;
	{T::Candle::Low} -> std::same_as<float>;
	{API.GetHistoricalData(std::string(), std::string(), int())} -> std::same_as<std::vector<typename T::Candle>>;
};

template<SimulatorAPI TAPI>
class Simulator
{
private:
	Trade _current_trade;
public:
	std::vector<Trade> Trades;
	float ProfitAndLoss;
	float MaxDrawdown;
	float MaxRunup;
	float Winrate;
	int Wins;
	int Losses;
	int TradeCount;
public:
	using API = TAPI;

	Simulator()
		:_current_trade(Trade{}), ProfitAndLoss(0.0f), MaxDrawdown(0.0f), MaxRunup(0.0f), Winrate(0.0f), Wins(0), Losses(0), TradeCount(0)
	{
	}

	void EnterTrade(float Price, double Timestamp, TradeType Type)
	{
		if (_current_trade.EntryTimestamp != 0.0) throw std::logic_error("EnterTrade called while trade was active");

		_current_trade.EntryTimestamp = Timestamp;
		_current_trade.EntryPrice = Price;
		_current_trade.Type = Type;
	}

	void ExitTrade(float Price, double Timestamp) noexcept
	{
		_current_trade.ExitPrice = Price;
		_current_trade.ExitTimestamp = Timestamp;

		float trade_pnl = (_current_trade.Type == TradeType::Long) ? _current_trade.ExitPrice - _current_trade.EntryPrice : _current_trade.EntryPrice - _current_trade.ExitPrice;
		if (trade_pnl > 0.0f) Wins++;
		else Losses++;

		if (MaxRunup < ProfitAndLoss) MaxRunup = ProfitAndLoss;
		else if (MaxDrawdown > ProfitAndLoss) ProfitAndLoss;

		TradeCount++;
		Trades.push_back(_current_trade);
		_current_trade = Trade{};
	}
};

//TODO add Simulator concept make sure it has required methods and variables

template<class TSim>
inline void Simulate(TSim& Simulator, const typename TSim::Parameters& Parms, const std::string& Symbol, const std::string& Timeframe, int NumberOfCandles)
{
	Simulator.Simulate(std::move(TSim::API::GetHistoricalData(Symbol, Timeframe, NumberOfCandles)), Parms);
}