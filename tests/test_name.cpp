#include <gtest/gtest.h>
#include "../main.hpp"

TEST(ValidNameTest, ValidNames) {
    EXPECT_TRUE(validName("alice"));
    EXPECT_TRUE(validName("1bob123"));
    EXPECT_TRUE(validName("test_user"));
    EXPECT_TRUE(validName("test-user"));
    EXPECT_TRUE(validName("test-user_123"));
    EXPECT_TRUE(validName("A1_b-2__"));        
}

TEST(ValidNameTest, InvalidNames) {
    EXPECT_FALSE(validName(""));             
    EXPECT_FALSE(validName(" "));           
    EXPECT_FALSE(validName("example!"));       
    EXPECT_FALSE(validName("test test"));       
    EXPECT_FALSE(validName("example@example.com"));   
    EXPECT_FALSE(validName("123:45"));      
}
