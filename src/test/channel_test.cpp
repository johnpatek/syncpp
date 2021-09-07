#include <syncpp.h>
#include <gtest/gtest.h>
#include <thread>

TEST(channel_test,single_test)
{
    int sent,received;
    syncpp::channel<int> test_channel;   

    std::thread receiver([&]
    {
        test_channel.receive(received);
    });
    
    std::thread sender([&]
    {
        sent = 5;
        test_channel.send(sent);
    });

    sender.join();
    receiver.join();
    EXPECT_EQ(sent,received);
}

TEST(channel_test,multi_test)
{
    std::vector<std::pair<int,int>> data_vector;    
    syncpp::channel<int> test_channel;   

    data_vector.reserve(10);

    while(data_vector.size() < data_vector.capacity())
    {
        data_vector.push_back(
            std::move(std::make_pair<int,int>(data_vector.size(),0)));
    }

    std::thread receiver([&]
    {
        int index = 0;
        int received;
        while(test_channel.receive(received))
        {
            data_vector[index++].second = received;
        }
    });
    
    std::thread sender([&]
    {
        for (const std::pair<int,int>& data: data_vector)
        {
            test_channel.send(data.first);
        }
        test_channel.close();
    });

    sender.join();
    receiver.join();
    for (const std::pair<int,int>& data: data_vector)
    {
        EXPECT_EQ(data.first,data.second);
    }
}