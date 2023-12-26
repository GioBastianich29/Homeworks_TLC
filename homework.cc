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
    CsmaStarHelper csmaStar(2, csma);


    //************* FINE RETE CSMA *************



    //************* RETE WIFI *************
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(9);
    // TODO: allNodes.Add(wifiStaNodes); DA FARE ALLA FINE
    NodeContainer wifiApNode;
    wifiApNode.Create(1);
    //allNodes.Add(wifiApNode); DA FARE ALLA FINE

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

    PointToPointStarHelper secondNetStar(2, secondNet);


    //TODO: aggiungere a allNodes
    NodeContainer secondSubnetNodes;
    secondSubnetNodes.Create(2);
    NetDeviceContainer secondSubnetDevices;

    for (uint32_t i = 0; i < 2; i++){
        NodeContainer node(secondNetStar.GetSpokeNode(0), secondSubnetNodes.Get(i));
        secondSubnetDevices.Add(secondNet.Install(node).Get(0));
    }

    //************* FINE RETE 2 *************





    //************* RETE CENTRALE *************
    NodeContainer centralNetNodes;
    centralNetNodes.Create(2);



    PointToPointHelper centralNet;
    centralNet.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    centralNet.SetChannelAttribute("Delay", StringValue("200ms"));
    NodeContainer tempNode = NodeContainer(centralNetNodes.Get(0), centralNetNodes.Get(1));


    NetDeviceContainer centralNetDevices;
    centralNetDevices.Add(centralNet.Install(tempNode).Get(0));


    centralNet.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    centralNet.SetChannelAttribute("Delay", StringValue("20ms"));
    tempNode = NodeContainer(centralNetNodes.Get(0), secondNetStar.GetHub());
    centralNetDevices.Add(centralNet.Install(tempNode).Get(0));

    tempNode = NodeContainer(centralNetNodes.Get(0), csmaStar.GetHub());
    centralNetDevices.Add(centralNet.Install(tempNode).Get(0));


    tempNode = NodeContainer(centralNetNodes.Get(0), wifiApNode);
    centralNetDevices.Add(centralNet.Install(tempNode).Get(0));


    //************* FINE RETE CENTRALE *************



    //************* POPOLARE CONTAINER CON TUTTI I NODI *************
    NodeContainer allNodes;
    for (uint32_t i = 0; i < csmaStar.SpokeCount(); i++){
        allNodes.Add(csmaStar.GetSpokeNode(i));
    }
    allNodes.Add(csmaStar.GetHub());


    allNodes.Add(centralNetNodes.Get(0));
    allNodes.Add(centralNetNodes.Get(1));


    allNodes.Add(secondNetStar.GetHub());
    for (uint32_t i = 0; i < secondNetStar.SpokeCount(); i++){
        allNodes.Add(secondNetStar.GetSpokeNode(i));
    }

    for (uint32_t i = 0; i < 2; i++){
        allNodes.Add(secondSubnetNodes.Get(i));
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

    address.SetBase("10.1.1.0", "255.255.255.248");

    Ipv4InterfaceContainer centralInterface;
    centralInterface = address.Assign(centralNetDevices);

    address.SetBase("10.1.1.8", "255.255.255.248");
    csmaStar.AssignIpv4Addresses(address);


    address.SetBase("10.1.1.24", "255.255.255.248");
    secondNetStar.AssignIpv4Addresses(address);


    address.SetBase("10.1.1.16", "255.255.255.248");
    Ipv4InterfaceContainer secondSubnetInterface;
    secondSubnetInterface = address.Assign(secondSubnetDevices);


    address.SetBase("10.1.1.32", "255.255.255.240");
    Ipv4InterfaceContainer wifiInterface = address.Assign(adhocDevices);


    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    //************* FINE STRATO DI TRASPORTO *************








    //************* NETANIM *************
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

    //************* FINE NETANIM *************

}
