// PcapTutorialPart3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "CommonHeaders.h"
#include "PacketStats.cpp"

static void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie);



int main()
{
    // IPv4 address of the interface we want to sniff
    std::string interfaceIPAddr = "192.168.1.2";

    // find the interface by IP address
    auto* dev = pcpp::PcapLiveDeviceList::getInstance().getDeviceByIp(interfaceIPAddr);
    if (dev == nullptr)
    {
        std::cerr << "Cannot find interface with IPv4 address of '" << interfaceIPAddr << "'" << std::endl;
        return 1;
    }

    // before capturing packets let's print some info about this interface
    std::cout
        << "Interface info:" << std::endl
        << "   Interface name:        " << dev->getName() << std::endl // get interface name
        << "   Interface description: " << dev->getDesc() << std::endl // get interface description
        << "   MAC address:           " << dev->getMacAddress() << std::endl // get interface MAC address
        << "   Default gateway:       " << dev->getDefaultGateway() << std::endl // get default gateway
        << "   Interface MTU:         " << dev->getMtu() << std::endl; // get interface MTU

    if (!dev->getDnsServers().empty())
    {
        std::cout << "   DNS server:            " << dev->getDnsServers().front() << std::endl;
    }

    // open the device before start capturing/sending packets
    if (!dev->open())
    {
        std::cerr << "Cannot open device" << std::endl;
        return 1;
    }

    // create the stats object
    PacketStats stats;

    std::cout << std::endl << "Starting async capture..." << std::endl;

    // start capture in async mode. Give a callback function to call to whenever a packet is captured and the stats object as the cookie
    dev->startCapture(onPacketArrives, &stats);

    // sleep for 10 seconds in main thread, in the meantime packets are captured in the async thread
    std::this_thread::sleep_for(std::chrono::seconds(20));
    

    // stop capturing packets
    dev->stopCapture();

    // print results
    std::cout << "Results:" << std::endl;
    stats.printToConsole();

}

/**
 * A callback function for the async capture which is called each time a packet is captured
 */
static void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie)
{
    // extract the stats object form the cookie
    auto* stats = static_cast<PacketStats*>(cookie);

    // parsed the raw packet
    pcpp::Packet parsedPacket(packet);

    if (parsedPacket.isPacketOfType(pcpp::HTTP)) {
        auto* httpRequestLayer = parsedPacket.getLayerOfType<pcpp::HttpRequestLayer>();
        if (httpRequestLayer == nullptr)
        {
            std::cerr << "Something went wrong, couldn't find HTTP request layer" << std::endl;
        }

        std::cout << httpRequestLayer->getFieldByName(PCPP_HTTP_HOST_FIELD)->getFieldValue() << std::endl;
    }

    // collect stats from packet
    stats->consumePacket(parsedPacket);
}

