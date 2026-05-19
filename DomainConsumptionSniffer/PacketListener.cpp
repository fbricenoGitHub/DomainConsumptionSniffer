#include "PacketListener.h"
#include <functional>

PacketListener::PacketListener(DomainHttpAccessCounter* accessCounter, CommandLineArgumentValidator* argumentValidator) {
	accessCounter = accessCounter;
	this->argumentValidator = argumentValidator;	
}


void PacketListener::Listen() const {

    // find the interface by IP address
    auto* dev = pcpp::PcapLiveDeviceList::getInstance().getDeviceByIp(this->argumentValidator->getIpAddressArgument());
    if (dev == nullptr)
    {
        cerr << "Cannot find interface with IPv4 address of '" << this->argumentValidator->getIpAddressArgument() << "'" << endl;
    }

    // before capturing packets let's print some info about this interface
    cout
        << "Interface info:" << endl
        << "   Interface name:        " << dev->getName() << endl // get interface name
        << "   Interface description: " << dev->getDesc() << endl // get interface description
        << "   MAC address:           " << dev->getMacAddress() << endl // get interface MAC address
        << "   Default gateway:       " << dev->getDefaultGateway() << endl // get default gateway
        << "   Interface MTU:         " << dev->getMtu() << endl; // get interface MTU

    if (!dev->getDnsServers().empty())
    {
        cout << "   DNS server:            " << dev->getDnsServers().front() << endl;
    }

    // open the device before start capturing/sending packets
    if (!dev->open())
    {
        cerr << "Cannot open device" << endl;
        return;
    }

    cout << endl << "Starting async capture..." << endl;
    PacketStats stats;

    void (PacketListener:: * methodPtr)(pcpp::RawPacket * packet, pcpp::PcapLiveDevice * dev, void* cookie) = &PacketListener::onPacketArrives;

    dev->startCapture(methodPtr, stats);
    
    dev->startCapture(onPacketArrives, &stats);


    dev->startCapture([](pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie) {
        auto* sniffer = static_cast<PacketListener*>(cookie);
        sniffer->onPacketArrives(packet, dev);
        }, this);




    
    // sleep for 10 seconds in main thread, in the meantime packets are captured in the async thread
    this_thread::sleep_for(chrono::seconds(this->argumentValidator->getMonitorTimeArgument()));


    // stop capturing packets
    dev->stopCapture();
}



void PacketListener::onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie) {
    // extract the stats object form the cookie
    PacketStats* stats = static_cast<PacketStats*>(cookie);

    // parsed the raw packet
    pcpp::Packet parsedPacket(packet);

    auto* httpRequestLayer = parsedPacket.getLayerOfType<pcpp::HttpRequestLayer>();
    if (httpRequestLayer == nullptr)
    {
        cerr << "Something went wrong, couldn't find HTTP request layer" << endl;
        return;
    }

    PacketListener::accessCounter->incrementDomainCount(httpRequestLayer->getFieldByName(PCPP_HTTP_HOST_FIELD)->getFieldValue());


    // collect stats from packet
    stats->consumePacket(parsedPacket);
}
