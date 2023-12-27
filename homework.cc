#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/csma-star-helper.h"
#include "ns3/point-to-point-star.h"
#include "ns3/netanim-module.h"
#include "ns3/log.h"
using namespace ns3;
int
main(int argc, char* argv[])
{

    bool verbose = true;
    //uint32_t nCsma = 3;
    //uint32_t nWifi = 9;
    bool tracing = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    // first subnet
    // here i'm creating the first subnet and i'm setting the data rate and transport delay

    //************* RETE CSMA *************
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("200ms"));

    NodeContainer csmaNodes;
    csmaNodes.Create(3);
    NodeContainer n0n2 = NodeContainer(csmaNodes.Get(0), csmaNodes.Get(1));
    NodeContainer n1n2 = NodeContainer(csmaNodes.Get(0), csmaNodes.Get(2));
    NetDeviceContainer d0d2 = csma.Install(n0n2);
    NetDeviceContainer d1d2 = csma.Install(n1n2);
    //************* FINE RETE CSMA *************



    //************* RETE WIFI *************
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(9);
    NodeContainer wifiApNode;
    wifiApNode.Create(1);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager"); // <---- Use this in the homework
    // you can do in this way if you want to set a constant data rate
    //wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate6Mbps"));

    WifiMacHelper mac;

    // Ad Hoc Mode
    NetDeviceContainer adhocDevices;
    mac.SetType("ns3::AdhocWifiMac");
    adhocDevices = wifi.Install(phy, mac, wifiStaNodes);
    Ssid ssid = Ssid("1987799");
    NetDeviceContainer apDevices;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevices = wifi.Install(phy, mac, wifiApNode);
    // Mobility
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
        "MinX",
        DoubleValue(10.0),
        "MinY",
        DoubleValue(10.0),
        "DeltaX",
        DoubleValue(5.0),
        "DeltaY",
        DoubleValue(2.0),
        "GridWidth",
        UintegerValue(5),
        "LayoutType",
        StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiStaNodes);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    //************* FINE RETE WIFI *************


    //************* RETE 2 *************

    PointToPointHelper secondNet;
    secondNet.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    secondNet.SetChannelAttribute("Delay", StringValue("20ms"));

    NodeContainer secondNetNodes;
    secondNetNodes.Create(5);
    NodeContainer n5n6 = NodeContainer(secondNetNodes.Get(0), secondNetNodes.Get(1));
    NodeContainer n5n7 = NodeContainer(secondNetNodes.Get(0), secondNetNodes.Get(2));
    NodeContainer n6n8 = NodeContainer(secondNetNodes.Get(1), secondNetNodes.Get(3));
    NodeContainer n6n9 = NodeContainer(secondNetNodes.Get(1), secondNetNodes.Get(4));

    NetDeviceContainer d5d6 = secondNet.Install(n5n6);
    NetDeviceContainer d5d7 = secondNet.Install(n5n7);
    NetDeviceContainer d6d8 = secondNet.Install(n6n8);
    NetDeviceContainer d6d9 = secondNet.Install(n6n9);

   //************* FINE RETE 2 *************


   //************* RETE CENTRALE *************

    NodeContainer centralNetNodes;
    centralNetNodes.Create(2);

    NodeContainer n3n4 = NodeContainer(centralNetNodes.Get(0), centralNetNodes.Get(1));
    NodeContainer n2n4 = NodeContainer(centralNetNodes.Get(0), csmaNodes.Get(0));
    NodeContainer n4n5 = NodeContainer(centralNetNodes.Get(0), secondNetNodes.Get(0));
    NodeContainer n4n10 = NodeContainer(centralNetNodes.Get(0), wifiApNode);

    PointToPointHelper centralNet;
    centralNet.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    centralNet.SetChannelAttribute("Delay", StringValue("200ms"));
    NodeContainer tempNode = NodeContainer(n3n4);


    NetDeviceContainer d3d4 = centralNet.Install(n3n4);

    centralNet.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    centralNet.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer d2d4 = centralNet.Install(n2n4);
    NetDeviceContainer d4d5 = centralNet.Install(n4n5);
    NetDeviceContainer d4d10 = centralNet.Install(n4n10);
    //************* FINE RETE CENTRALE *************



    //************* POPOLARE CONTAINER CON TUTTI I NODI *************
    NodeContainer allNodes;
    for (uint32_t i = 1; i < csmaNodes.GetN(); i++){
        allNodes.Add(csmaNodes.Get(i));
    }
    allNodes.Add(csmaNodes.Get(0));


    allNodes.Add(centralNetNodes.Get(0));
    allNodes.Add(centralNetNodes.Get(1));


    allNodes.Add(secondNetNodes.Get(0));
    for (uint32_t i = 1; i < secondNetNodes.GetN(); i++){
        allNodes.Add(secondNetNodes.Get(i));
    }

    allNodes.Add(wifiApNode);

    for (uint32_t i = 0; i < 9; i++){
        allNodes.Add(wifiStaNodes.Get(i));
    }
    //************* FINE POPOLAZIONE NODI *************





    //************* STRATO DI TRASPORTO *************
    InternetStackHelper stack;
    Ipv4AddressHelper address;

    stack.Install(allNodes);

    address.SetBase("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer i0i2 = address.Assign(d0d2);

    address.SetBase("10.1.1.4", "255.255.255.252");
    Ipv4InterfaceContainer i1i2 = address.Assign(d1d2);

    address.SetBase("10.1.1.8", "255.255.255.252");
    Ipv4InterfaceContainer i2i4 = address.Assign(d2d4);

    address.SetBase("10.1.1.12", "255.255.255.252");
    Ipv4InterfaceContainer i3i4 = address.Assign(d3d4);

    address.SetBase("10.1.1.16", "255.255.255.252");
    Ipv4InterfaceContainer i4i6 = address.Assign(d4d5);

    address.SetBase("10.1.1.20", "255.255.255.252");
    Ipv4InterfaceContainer i4i10 = address.Assign(d4d10);

    address.SetBase("10.1.1.24", "255.255.255.252");
    Ipv4InterfaceContainer i5i6 = address.Assign(d5d6);

    address.SetBase("10.1.1.28", "255.255.255.252");
    Ipv4InterfaceContainer i5i7 = address.Assign(d5d7);

    address.SetBase("10.1.1.32", "255.255.255.252");
    Ipv4InterfaceContainer i6i8 = address.Assign(d6d8);

    address.SetBase("10.1.1.36", "255.255.255.252");
    Ipv4InterfaceContainer i6i9 = address.Assign(d6d9);



    address.SetBase("10.1.2.0", "255.255.255.240");
    address.Assign(apDevices);
    address.Assign(adhocDevices);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    //************* FINE STRATO DI TRASPORTO *************








    //************* NETANIM *************
    /*
    AnimationInterface anim("HOMEWORKANIMATION.xml"); // Mandatory
    for (uint32_t i = 0; i < wifiStaNodes.GetN(); ++i)
    {
        anim.UpdateNodeDescription(wifiStaNodes.Get(i), "STA"); // Optional
        anim.UpdateNodeColor(wifiStaNodes.Get(i), 255, 0, 0);   // Optional
    }
    for (uint32_t i = 0; i < wifiApNode.GetN(); ++i)
    {
        anim.UpdateNodeDescription(wifiApNode.Get(i), "AP"); // Optional
        anim.UpdateNodeColor(wifiApNode.Get(i), 0, 255, 0);  // Optional
    }

    anim.UpdateNodeDescription(csmaStar.GetHub(), "CSMA"); // Optional
    anim.UpdateNodeColor(csmaStar.GetHub(), 0, 0, 255);

    for (uint32_t i = 0; i < csmaStar.SpokeCount(); ++i)
    {
        anim.UpdateNodeDescription(csmaStar.GetSpokeNode(i), "CSMA"); // Optional
        anim.UpdateNodeColor(csmaStar.GetSpokeNode(i), 0, 0, 255);    // Optional
    }

    anim.UpdateNodeDescription(secondNetStar.GetHub(), "NET2"); // Optional
    anim.UpdateNodeColor(secondNetStar.GetHub(), 0, 111, 255);

    for (uint32_t i = 0; i < secondNetStar.SpokeCount(); ++i)
    {
        anim.UpdateNodeDescription(secondNetStar.GetSpokeNode(i), "NET2"); // Optional
        anim.UpdateNodeColor(secondNetStar.GetSpokeNode(i), 0, 111, 255);    // Optional
    }

    for (uint32_t i = 0; i < secondSubnetNodes.GetN(); ++i)
    {
        anim.UpdateNodeDescription(secondSubnetNodes.Get(i), "SUBNET2"); // Optional
        anim.UpdateNodeColor(secondSubnetNodes.Get(i), 0, 111, 255);    // Optional
    }

    anim.UpdateNodeDescription(centralNetNodes.Get(0), "CENTRALNET"); // Optional
    anim.UpdateNodeColor(centralNetNodes.Get(0), 123, 0, 255);

    anim.UpdateNodeDescription(centralNetNodes.Get(1), "CENTRALNET"); // Optional
    anim.UpdateNodeColor(centralNetNodes.Get(1), 123, 0, 255);

    anim.EnablePacketMetadata(); // Optional
    // anim.EnableQueueCounters(Seconds(0),Seconds(10));
    anim.EnableIpv4RouteTracking("routingtable-wireless.xml",
        Seconds(0),
        Seconds(5),
        Seconds(0.25));         // Optional
    anim.EnableWifiMacCounters(Seconds(0), Seconds(10)); // Optional
    anim.EnableWifiPhyCounters(Seconds(0), Seconds(10)); // Optional
    */
    //************* FINE NETANIM *************

}
