#pragma once
#include <string>        
#include <string_view>   
#include <vector>
#include <deque>
#include <algorithm>    
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <compare>      
#include <format>      
#include <ostream>    
#include <iomanip>       
#include <cctype>     
#include <ranges>   

struct Time {
    int minutes = 0;
    constexpr Time(int m = 0) noexcept : minutes(m) {}

    [[nodiscard]] static constexpr bool try_parse(std::string_view sv, Time& out) noexcept {
        if (sv.size() != 5 || sv[2] != ':') {
            return false;
        }

        unsigned char c0 = static_cast<unsigned char> (sv[0]);
        unsigned char c1 = static_cast<unsigned char> (sv[1]);
        unsigned char c3 = static_cast<unsigned char> (sv[3]);
        unsigned char c4 = static_cast<unsigned char> (sv[4]);

        if(!(std::isdigit(c0) && std::isdigit(c1) && std::isdigit(c3) && std::isdigit(c4))) {
            return false;
        }

        int h = (c0 - '0') * 10 + (c1 - '0');
        int m = (c3 - '0') * 10 + (c4 - '0');
        if(h > 23 || m > 59){
            return false;
        }
        out.minutes = h * 60 + m;
        return true;
    }

    std::string to_string() const {
        return std::format("{:02}:{:02}", minutes / 60, minutes % 60);
    }
    
    friend std::ostream& operator<<(std::ostream& os,const Time&t){
        return os <<std::setw(2)<<std::setfill('0')<<t.minutes/60<<':'
                 <<std::setw(2)<<std::setfill('0')<<t.minutes%60;
    }
    
    auto operator<=>(Time const&) const = default;
};

[[maybe_unused]] 
inline static std::optional<Time> parseTime(std::string_view sv){
    Time t;
    return Time::try_parse(sv, t) ? std::optional<Time>{t} : std::nullopt;
}

enum class ErrorCode{
    YouShallNotPass = 0,
    NotOpenYet,
    PlaceIsBusy,
    ClientUnknown,
    ICanWaitNoLonger
};

constexpr std::string_view errTxt(ErrorCode e) noexcept{
    using enum ErrorCode;
    switch(e){
        case YouShallNotPass: return "YouShallNotPass";
        case NotOpenYet:      return "NotOpenYet";
        case PlaceIsBusy:     return "PlaceIsBusy";
        case ClientUnknown:   return "ClientUnknown";
        case ICanWaitNoLonger:return "ICanWaitNoLonger!";
        default : return "Unhandled exception type";
    }
}


constexpr bool nameChecker(char c) noexcept {
    unsigned char uc = static_cast<unsigned char>(c);
    return std::isalnum(uc) || c == '_' || c == '-';
}

[[nodiscard]]
inline bool validName(std::string_view s) noexcept {
    return !s.empty() && std::ranges::all_of(s, nameChecker);
}

struct Table{
    std::string client;
    int since = -1; // -1 = empty
    int busy = 0;
    long long revenue = 0;
};

enum class ClientStatus{
    Inside = 0,
    Waiting,
    Seated
};

struct Client{ 
    ClientStatus status;
    int occupiedTable = -1; // -1 = No table 
};

struct Ctx{
    long long numberOfTables;
    Time open, close;
    long long price;
    std::vector<Table> tables;
    std::unordered_set<int> freeTables;
    std::unordered_map<std::string, Client> clientsInfo;
    std::deque<std::string> q;
    std::vector<std::string> log;
    Time previousTime{0};
    Ctx(long long n_,Time o, Time c, long long p) : numberOfTables(n_),open(o),close(c),price(p),tables(n_) {
        for(long long i = 0; i < numberOfTables; ++i){
           freeTables.insert(i);
        } 
    }
};


void finish(Ctx& context, const std::string& name, Time now );

void logError(Ctx& ctx, Time t, ErrorCode e);

void place(Ctx& ctx, const std::string& name, int idx, Time t, int outId);

void processQueue(Ctx& ctx, Time t, int freedTable);

void e1(Ctx&ctx, Time t, const std::string& name);

void e2(Ctx&ctx, Time t, const std::string& name, long long table);

void e3(Ctx&ctx, Time t, const std::string& name);

void e4(Ctx&ctx, Time t, const std::string& name);

[[noreturn]] 
void formatFail(std::string_view reason, std::string_view line);