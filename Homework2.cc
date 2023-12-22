#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

int main(int argc, char* argv[]) {

    bool verbose = true;
    uint32_t nCsma = 3;
    uint32_t nWifi = 9;
    bool tracing = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);

    if (verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    //here i'm creating the first subnet and i'm setting the data rate and transport delay
    NodeContainer csmaNodes;
    csmaNodes.Create(nCsma);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("200ms"));

    //here there is the second subnet, there are two routers, the first node of this subnet is the same of 
    //the previous subnet
    NodeContainer p2pNodes1;
    p2pNodes1.Add(csmaNodes.Get(2));//->last(third) node of the first subnet
    p2pNodes1.Create(1);

    PointToPointHelper p2p;
    p2p.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));
    
    //now i'm creating the third subnet, between a router(second node of the second subnet) and another server
    NodeContainer p2pNodes2;
    p2pNodes2.Add(p2pNodes1.Get(1));//->second node of the previous subnet
    p2pNodes2.Create(1);

    p2p.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));
}
