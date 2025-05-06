#include <gtest/gtest.h>
#include "../main.hpp"

TEST(TimeParse, Valid) {
    Time t;
    EXPECT_TRUE(Time::try_parse("00:00", t));
    EXPECT_EQ(t.minutes, 0);
    EXPECT_TRUE(Time::try_parse("07:11", t));
    EXPECT_EQ(t.minutes, 7*60 + 11);
}

TEST(TimeParse, InvalidFormat) {
    Time t;
    EXPECT_FALSE(Time::try_parse("1:30",  t));   
    EXPECT_FALSE(Time::try_parse("24:00", t)); 
    EXPECT_FALSE(Time::try_parse("12:60", t));   
    EXPECT_FALSE(Time::try_parse("12:61", t));   
    EXPECT_FALSE(Time::try_parse("xx:xx", t));  
    EXPECT_FALSE(Time::try_parse("12345", t)); 
}
