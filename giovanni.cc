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
NS_LOG_COMPONENT_DEFINE("Task_1987799");
int
main(int argc, char* argv[])
{
    Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(1797));

       // ??? try and stick 15kb/s into the data rate
    Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("5Mb/s"));

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
    NodeContainer n0n2 = NodeContainer(csmaNodes.Get(1), csmaNodes.Get(0));
    NodeContainer n1n2 = NodeContainer(csmaNodes.Get(2), csmaNodes.Get(0));
    NetDeviceContainer d0d2 = csma.Install(n0n2);
    NetDeviceContainer d1d2 = csma.Install(n1n2);
    //************* FINE RETE CSMA *************



    //************* RETE WIFI *************
    NodeContainer wifiNodes;
    wifiNodes.Create(10);

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
    adhocDevices = wifi.Install(phy, mac, wifiNodes);
    // Mobility
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

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiNodes);
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
    NodeContainer n2n4 = NodeContainer(csmaNodes.Get(0), centralNetNodes.Get(0));
    NodeContainer n4n5 = NodeContainer(centralNetNodes.Get(0), secondNetNodes.Get(0));
    NodeContainer n4n10 = NodeContainer(centralNetNodes.Get(0), wifiNodes.Get(0));

    PointToPointHelper centralNet;
    centralNet.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    centralNet.SetChannelAttribute("Delay", StringValue("200ms"));

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

    for (uint32_t i = 0; i < wifiNodes.GetN(); i++){
        allNodes.Add(wifiNodes.Get(i));
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

    address.SetBase("10.1.2.0", "255.255.255.252");
    Ipv4InterfaceContainer i2i4 = address.Assign(d2d4);

    address.SetBase("10.1.2.4", "255.255.255.252");
    Ipv4InterfaceContainer i3i4 = address.Assign(d3d4);

    address.SetBase("10.1.2.8", "255.255.255.252");
    Ipv4InterfaceContainer i4i6 = address.Assign(d4d5);

    address.SetBase("10.1.2.12", "255.255.255.252");
    Ipv4InterfaceContainer i4i10 = address.Assign(d4d10);

    address.SetBase("10.1.3.0", "255.255.255.252");
    Ipv4InterfaceContainer i5i6 = address.Assign(d5d6);

    address.SetBase("10.1.3.4", "255.255.255.252");
    Ipv4InterfaceContainer i5i7 = address.Assign(d5d7);

    address.SetBase("10.1.4.0", "255.255.255.252");
    Ipv4InterfaceContainer i6i8 = address.Assign(d6d8);

    address.SetBase("10.1.4.8", "255.255.255.252");
    Ipv4InterfaceContainer i6i9 = address.Assign(d6d9);



    address.SetBase("10.1.5.0", "255.255.255.240");
    Ipv4InterfaceContainer wifiInterface = address.Assign(adhocDevices);



    //************* ON OFF APPLICATION *************
    uint16_t port1 = 112;
    BulkSendHelper burstSource1("ns3::TcpSocketFactory", InetSocketAddress(i0i2.GetAddress(0), port1));
    ApplicationContainer burstApp1 = burstSource1.Install(allNodes.Get(19));
    burstSource1.SetAttribute("MaxBytes", UintegerValue(1752));
    burstApp1.Start(Seconds(1.16));
    burstApp1.Stop(Seconds(10.0));

    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(i0i2.GetAddress(0), port1));
    ApplicationContainer sinkApps = sink.Install(allNodes.Get(0));
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(Seconds(10.0));
    uint32_t payloadSize1 = 1752; // Dimensione del payload del pacchetto
    burstSource1.SetAttribute("SendSize", UintegerValue(payloadSize1));


    uint16_t port2 = 113;
    BulkSendHelper burstSource2("ns3::TcpSocketFactory", InetSocketAddress(i1i2.GetAddress(0), port2));
    ApplicationContainer burstApp2 = burstSource2.Install(allNodes.Get(9));
    burstSource2.SetAttribute("MaxBytes", UintegerValue(1797));
    burstApp2.Start(Seconds(3.38));
    burstApp2.Stop(Seconds(10.0));

    uint32_t payloadSize2 = 1797; // Dimensione del payload del pacchetto
    burstSource2.SetAttribute("SendSize", UintegerValue(payloadSize2));


    PacketSinkHelper sink2("ns3::TcpSocketFactory", InetSocketAddress(i1i2.GetAddress(0), port2));
    ApplicationContainer sinkApps2 = sink2.Install(allNodes.Get(1));
    sinkApps2.Start(Seconds(0.0));
    sinkApps2.Stop(Seconds(10.0));



    uint16_t port3 = 114;
    BulkSendHelper burstSource3("ns3::TcpSocketFactory", InetSocketAddress(i0i2.GetAddress(0), port3));
    ApplicationContainer burstApp3 = burstSource3.Install(allNodes.Get(6));
    burstSource3.SetAttribute("MaxBytes", UintegerValue(1120));
    burstApp3.Start(Seconds(3.05));
    burstApp3.Stop(Seconds(10.0));


    uint32_t payloadSize3 = 1120; // Dimensione del payload del pacchetto
    burstSource2.SetAttribute("SendSize", UintegerValue(payloadSize3));




    PacketSinkHelper sink3("ns3::TcpSocketFactory", InetSocketAddress(i0i2.GetAddress(0), port3));
    ApplicationContainer sinkApps3 = sink3.Install(allNodes.Get(0));
    sinkApps3.Start(Seconds(0.0));
    sinkApps3.Stop(Seconds(10.0));
    /*
    uint16_t port2 = 10;
    BulkSendHelper burstSource2("ns3::TcpSocketFactory", InetSocketAddress(i1i2.GetAddress(0), port2));
    ApplicationContainer burstApp2 = burstSource2.Install(n6n9.Get(1));
    burstApp2.Start(Seconds(3.38));
    burstApp2.Stop(Seconds(10.0));
    burstSource1.SetAttribute("SendSize", UintegerValue(1797));


    uint16_t port3 = 11;
    BulkSendHelper burstSource3("ns3::TcpSocketFactory", InetSocketAddress(i0i2.GetAddress(0), port3));
    ApplicationContainer burstApp3 = burstSource3.Install(n5n6.Get(1));
    burstApp3.Start(Seconds(3.05));
    burstApp3.Stop(Seconds(10.0));
    burstSource1.SetAttribute("SendSize", UintegerValue(1120));
    */

    //************* FINE ON OFF APPLICATION *************

    //************* UDP ECHO APPLICATION *************

    uint16_t portUdp = 12 ; // Porta per l'applicazione UDP
    UdpEchoServerHelper echoServer(portUdp);
    //ApplicationContainer serverApp = echoServer.Install(n0n2.Get(0)); // Nodo 1 come server
    ApplicationContainer serverApp = echoServer.Install(n3n4.Get(0));
    serverApp.Start(Seconds(10.0));
    serverApp.Stop(Seconds(20.0));

    //UdpEchoClientHelper echoClient(i0i2.GetAddress(0), 12);
    UdpEchoClientHelper echoClient(i3i4.GetAddress(0), 12);
    echoClient.SetAttribute("MaxPackets", UintegerValue(250));
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(20.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1210));

    ApplicationContainer clientApp = echoClient.Install(allNodes.Get(12)); // Nodo 0 come client
    clientApp.Start(Seconds(11.0));
    clientApp.Stop(Seconds(20.0));


    uint32_t packetSize = 1210;
    uint8_t buffer[packetSize] = {0};

    std::string data = "Giovanni,Lentini,1987799,Flavio,Ialongo,2000932,Daniele, De Pascali, 1234567, Sandro, Brunetti, 1234567, Edoardo, Martire, 1234567";
    std::copy(data.begin(), data.end(), buffer);

    echoClient.SetFill(clientApp.Get(0), buffer, packetSize, packetSize);

    //************* FINE UDP ECHO APPLICATION *************

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    /*phy.EnablePcap("wifi", adhocDevices.Get(9), false);
    centralNet.EnablePcap("central", d2d4.Get(1), false);
    csma.EnablePcap("csma", d0d2.Get(0), false);

    secondNet.EnablePcap("secondNet", d5d6.Get(1), false);
    secondNet.EnablePcap("secondSubnet", d6d9.Get(1), false);*/


    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Stop(Seconds(20));
    Simulator::Destroy();


    Ptr<PacketSink> sink_1 = DynamicCast<PacketSink>(sinkApps.Get(0));
    std::cout << "Total Bytes Received (sink1): " << sink_1->GetTotalRx() << std::endl;

    Ptr<PacketSink> sink_2 = DynamicCast<PacketSink>(sinkApps2.Get(0));
    std::cout << "Total Bytes Received (sink2): " << sink_2->GetTotalRx() << std::endl;

    Ptr<PacketSink> sink_3 = DynamicCast<PacketSink>(sinkApps3.Get(0));
    std::cout << "Total Bytes Received (sink3): " << sink_3->GetTotalRx() << std::endl;
    return 0;
}
