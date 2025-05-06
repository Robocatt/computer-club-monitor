#include "main.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>


#ifndef UNIT_TEST 
signed main(int argc, char* argv[]){
    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <input_file.txt>\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    if(!in){
        std::cerr << "Cannot open file " << argv[1] << "\n";
        return 1;
    }

    std::string line;

    if(!std::getline(in, line)){
        formatFail("Number of tables not found", "<EOF>");
    }
    long long nTables;
    try{ 
        nTables = std::stoll(line);
    }catch(...){ 
        formatFail("Error converting number of tables to number", line);
    }
    if(nTables <= 0){
        formatFail("Number of tables must be >0", line);
    }

    if(!std::getline(in, line)) {
        formatFail("Working hours not found", "<EOF>");
    }
    std::istringstream ss(line); 
    std::string openS, closeS;
    ss >> openS >> closeS;
    auto open = parseTime(openS), close = parseTime(closeS);
    
    if(!open || !close){
        formatFail("Incorrect working hours", line);
    }
    
    if(*open >= *close) {
        formatFail("Openning hours can not exceed closing hours", line);
    }

    if(!std::getline(in, line)) {
        formatFail("Price  not found", "<EOF>");
    }

    long long price;
    try{ 
        price = std::stoll(line);
    }catch(...){ 
        formatFail("Error converting price to number", line);
    }

    if(price <= 0){
        formatFail("Price must be >0", line);
    }

    Ctx ctx(nTables, *open, *close, price);

    // main loop over events
    while(std::getline(in, line)){
        if(line.empty()){ 
            continue;
        }
        std::istringstream tokens(line);
        std::vector<std::string> params;
        for(std::string s; tokens >> s; ){
            params.push_back(std::move(s));
        }
        if(params.size() < 3){
            formatFail("Incorrect event format", line);
        }

        auto eventTime = parseTime(params[0]);
        if(!eventTime){ 
            formatFail("Incorrect time for event", line);
        }
        if(*eventTime < ctx.previousTime){ 
            formatFail("Events are not monotonic in time", line);
        }
        long long id;
        try{
            id = std::stoll(params[1]);
        }catch(...){ 
            formatFail("Incorrect event id", line);
        }
        
        static constexpr int expectedSizes[] = { 0, 3, 4, 3, 3 };
                                        // id:   0, 1  2  3  4

        if (id < 1 || id > 4 || params.size() > INT32_MAX || static_cast<int>(params.size()) != expectedSizes[id]
            || !validName(params[2])){
            formatFail("Invalid event format", line); 
        }

        Time t = *eventTime;
        ctx.log.emplace_back(line);

        switch (id) {
            case 1: 
                e1(ctx, t, params[2]); 
                break;
            case 2:
                long long tNum;
                try{
                    tNum = std::stoll(params[3]);
                    e2(ctx, t, params[2], tNum);
                }catch(...){ 
                    formatFail("Incorrect table number", line);
                }
                break;
            case 3: 
                e3(ctx, t, params[2]);
                break;
            case 4:
                e4(ctx, t, params[2]);
                break;
        }
        ctx.previousTime = t;
    }

    ctx.log.insert(ctx.log.begin(), ctx.open.to_string());

    std::vector<std::string> names;
    for(auto&[name, _] : ctx.clientsInfo) {
        names.push_back(name);
    }

    std::sort(names.begin(), names.end());
    for(auto& name : names){
        ctx.log.emplace_back(ctx.close.to_string() + " 11 " + name);
        if(ctx.clientsInfo[name].status == ClientStatus::Seated){ 
            finish(ctx, name, ctx.close);
        }
    }
    ctx.log.emplace_back(ctx.close.to_string());

    
    for(std::string& s : ctx.log){ 
        std::cout << s << "\n";
    }
    
    for(long long i = 0; i < ctx.numberOfTables; ++i){
        Table& t = ctx.tables[i];
        std::cout << i + 1 << ' ' << t.revenue << ' '
                << std::format("{:02}:{:02}", t.busy / 60, t.busy % 60)<<"\n";
    }
    std::cout.flush();
    return 0;
}
#endif   