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

    //first subnet
    //here i'm creating the first subnet and i'm setting the data rate and transport delay
    NodeContainer csmaNodes;
    csmaNodes.Create(nCsma);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("200ms"));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    //second subnet 
    //here there is the second subnet, there are two routers, the first node of this subnet is the same of 
    //the previous subnet
    NodeContainer p2pNodes1;
    p2pNodes1.Add(csmaNodes.Get(2));//->last(third) node of the first subnet
    p2pNodes1.Create(1);

    PointToPointHelper p2p;
    p2p.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer p2pDevices1;
    p2pDevices1 = p2p.Install(p2pNodes1);
    
    //third subnet
    //now i'm creating the third subnet, between a router(second node of the second subnet) and another server
    NodeContainer p2pNodes2;
    p2pNodes2.Create(1);
    p2pNodes2.Add(p2pNodes1.Get(1));//->second node of the previous subnet

    p2p.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("200ms"));

    NetDeviceContainer p2pDevices2;
    p2pDevices2 = p2p.Install(p2pNodes2);

    //fourth subnet
    NodeContainer p2pNodes3;
    p2pNodes3.Add(p2pNodes2.Get(1));// -> router of the third subnet
    p2pNodes3.Create(1);

    p2p.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer p2pDevices3;
    p2pDevices3 = p2p.Install(p2pNodes3);

    //fifth subnet
    NodeContainer p2pNodes4;
    p2pNodes4.Add(p2pNodes3.Get(1));
    p2pNodes4.Create(1);

    p2p.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer p2pDevices4;
    p2pDevices4 = p2p.Install(p2pNodes4);

    //sixth subnet
    NodeContainer p2pNodes5;
    p2pNodes5.Add(p2pNodes4.Get(0));
    p2pNodes5.Create(1);

    p2p.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer p2pDevices5;
    p2pDevices5 = p2p.Install(p2pNodes5);

    //seventh subnet
    NodeContainer p2pNodes6;
    p2pNodes6.Add(p2pNodes4.Get(1));
    p2pNodes6.Create(1);

    p2p.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer p2pDevices6;
    p2pDevices6 = p2p.Install(p2pNodes6);

    //eighth subnet
    NodeContainer p2pNodes7;
    p2pNodes7.Add(p2pNodes4.Get(1));
    p2pNodes7.Create(1);

    p2p.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer p2pDevices7;
    p2pDevices7 = p2p.Install(p2pNodes7);

    //ninth subnet
    NodeContainer p2pNodes8;
    p2pNodes8.Add(p2pNodes1.Get(1));
    p2pNodes8.Create(1);

    p2p.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer p2pDevices8;
    p2pDevices8 = p2p.Install(p2pNodes8);

    //tenth subnet
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nWifi);
    NodeContainer wifiApNode = p2pNodes8.Get(1);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");

    WifiHelper wifi;
    wifi.SetStandard (WIFI_STANDARD_80211g);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper mac;
    NetDeviceContainer adhocDevices;
    mac.SetType("ns3::AdhocWifiMac");
    adhocDevices = wifi.Install(phy, mac, wifiStaNodes);
   
    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(10.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel",
                              "Bounds",
                              RectangleValue(Rectangle(-100, 100, -100, 100)));
    mobility.Install(wifiStaNodes);

    InternetStackHelper stack;
    stack.Install(csmaNodes);
    stack.Install(p2pNodes1.Get(1));
    stack.Install(p2pNodes2.Get(0));
    stack.Install(p2pNodes3.Get(1));
    stack.Install(p2pNodes4.Get(1));
    stack.Install(p2pNodes5.Get(1));
    stack.Install(p2pNodes6.Get(1));
    stack.Install(p2pNodes7.Get(1));
    stack.Install(wifiStaNodes);
    stack.Install(wifiApNode);

    //IP assignement
    Ipv4AddressHelper address;

    //first subnet IP
    address.SetBase("10.1.1.0", "255.255.255.248");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    //second subnet IP
    address.SetBase("10.1.2.0", "255.255.255.252");
    Ipv4InterfaceContainer p2pInterfaces1;
    p2pInterfaces1 = address.Assign(p2pDevices1);

    //third subnet IP
    address.SetBase("10.1.3.0", "255.255.255.252");
    Ipv4InterfaceContainer p2pInterfaces2;
    p2pInterfaces2 = address.Assign(p2pDevices2);

    //fourth subnet IP
    address.SetBase("10.1.4.0", "255.255.255.252");
    Ipv4InterfaceContainer p2pInterfaces3;
    p2pInterfaces3 = address.Assign(p2pDevices3);

    //fifth subnet IP
    address.SetBase("10.1.5.0", "255.255.255.252");
    Ipv4InterfaceContainer p2pInterfaces4;
    p2pInterfaces4 = address.Assign(p2pDevices4);

    //sixth subnet IP
    address.SetBase("10.1.6.0", "255.255.255.252");
    Ipv4InterfaceContainer p2pInterfaces5;
    p2pInterfaces5 = address.Assign(p2pDevices5);

    //seventh subnet IP
    address.SetBase("10.1.7.0", "255.255.255.252");
    Ipv4InterfaceContainer p2pInterfaces6;
    p2pInterfaces6 = address.Assign(p2pDevices6);

    //eighth subnet IP
    address.SetBase("10.1.8.0", "255.255.255.252");
    Ipv4InterfaceContainer p2pInterfaces7;
    p2pInterfaces7 = address.Assign(p2pDevices7);

    //ninth subnet IP
    address.SetBase("10.1.9.0", "255.255.255.252");
    Ipv4InterfaceContainer p2pInterfaces8;
    p2pInterfaces8 = address.Assign(p2pDevices8);

    //tenth subnet IP
    address.SetBase("10.1.10.0", "255.255.255.240");
    address.Assign(adhocDevices);
    

    //Application layer, we have to create the server

    //Here we have to create the tcp comunication
}
