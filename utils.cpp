#include "main.hpp"
#include <ranges>
#include <compare>
#include <iostream>

void logError(Ctx& ctx, Time t, ErrorCode e) {
    ctx.log.emplace_back(t.to_string() + " 13 " + std::string(errTxt(e)));
}

void place(Ctx& ctx, const std::string& name, int idx, Time t, int outId) {
    auto& tb = ctx.tables[idx];
    tb.client = name;
    tb.since  = t.minutes;
    ctx.clientsInfo[name] = {ClientStatus::Seated, idx};
    ctx.freeTables.erase(idx);
    if (outId){
        ctx.log.emplace_back(std::format("{} {} {} {}",
                t.to_string(), outId, name, idx + 1));
    }
}

void processQueue(Ctx& ctx, Time t, int freedTable) {
    if (ctx.q.empty()){ 
        return;
    }
    std::string nextClient = ctx.q.front(); 
    ctx.q.pop_front();
    place(ctx, nextClient, freedTable, t, 12);
}


void finish(Ctx& ctx, const std::string& name, Time now) {
    int idx = ctx.clientsInfo[name].occupiedTable;
    Table& tb = ctx.tables[idx];
    if (tb.since < 0){
        return;
    }

    int dur = now.minutes - tb.since;
    tb.busy += dur;
    tb.revenue += ((dur + 59) / 60) * ctx.price;
    tb.since = -1;
    tb.client.clear();
    ctx.freeTables.insert(idx);
}

// Client arrived
void e1(Ctx& ctx, Time t, const std::string& name) {
    if (t < ctx.open || t >= ctx.close) {
        logError(ctx, t, ErrorCode::NotOpenYet);
        return;
    }
    if (ctx.clientsInfo.contains(name)){
        logError(ctx, t, ErrorCode::YouShallNotPass);
        return;
    }
    ctx.clientsInfo[name] = {ClientStatus::Inside, -1};
}

// Client sat at the table
void e2(Ctx& ctx, Time t, const std::string& name, long long table) {
    if (!ctx.clientsInfo.contains(name)) {
        logError(ctx,t,ErrorCode::ClientUnknown);
        return;
    }

    long long tableIndex = table - 1;
    if (tableIndex < 0 || tableIndex >= ctx.numberOfTables || !ctx.tables[tableIndex].client.empty()){
        logError(ctx, t, ErrorCode::PlaceIsBusy);
        return;
    }
    if (ctx.clientsInfo[name].status == ClientStatus::Seated){
        finish(ctx, name, t);
    }else if (ctx.clientsInfo[name].status == ClientStatus::Waiting){
        ctx.q.erase(std::ranges::find(ctx.q, name));
    }
    place(ctx, name, tableIndex, t, 0);
}

// Client is waiting
void e3(Ctx& ctx, Time t, const std::string& name) {
    if (!ctx.clientsInfo.contains(name)) {
        logError(ctx, t, ErrorCode::ClientUnknown);
        return;
    }

    // I assume, the client will remain IN the club, despite the error
    if (!ctx.freeTables.empty()){
        logError(ctx, t, ErrorCode::ICanWaitNoLonger);
        return;
    }

    if (ctx.q.size() >= static_cast<size_t>(ctx.numberOfTables)){
        ctx.log.emplace_back(t.to_string() + " 11 " + name);
        ctx.clientsInfo.erase(name);
        return;
    }

    ctx.q.push_back(name);
    ctx.clientsInfo[name].status = ClientStatus::Waiting;
}

// Client has gone 
void e4(Ctx& ctx, Time t, const std::string& name) {
    if (!ctx.clientsInfo.contains(name)) {
        logError(ctx,t,ErrorCode::ClientUnknown);
        return;
    }
    Client curClient = ctx.clientsInfo[name];
    if (curClient.status == ClientStatus::Seated) {
        finish(ctx, name, t);
        processQueue(ctx, t, curClient.occupiedTable);
    } else if (curClient.status == ClientStatus::Waiting) {
        ctx.q.erase(std::ranges::find(ctx.q, name));
    }
    ctx.clientsInfo.erase(name);
}


[[noreturn]]
void formatFail(std::string_view reason, std::string_view line) {
    std::cerr << "Format error: " << reason << "\nOffending line: "
              << (line.empty() ? "<EOF>" : line) << '\n';
    std::exit(0);
}
