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
    InternetStackHelper stack;
    Ipv4AddressHelper address;

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("200ms"));
    CsmaStarHelper csmaStar(2, csma);

    csmaStar.InstallStack(stack);
    address.SetBase("10.1.1.0", "255.255.255.248");
    csmaStar.AssignIpv4Addresses(address);


    for (uint32_t i = 0; i < csmaStar.SpokeCount(); i++){
        std::cout << "Indirizzo ip hub: " << csmaStar.GetHubIpv4Address(i) << std::endl;
        std::cout << "Indirizzo ip spike: " << csmaStar.GetSpokeIpv4Address(i) << std::endl;
    }

    //************* FINE RETE CSMA *************

    //************* RETE 1 *************
    /*
    NodeContainer firstNetNodes;
    firstNetNodes.Add(csmaStar.Get(csmaLength - 1));
    firstNetNodes.Create(4); //il nodo centrale è quello in posizione 2 (router 4);

    PointToPointHelper firstNetworkP2P;
    firstNetworkP2P.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    firstNetworkP2P.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer firstNetworkDevice;

    for (uint32_t i = 0; i < firstNetNodes.GetN(); i++){
        std::cout << i << std::endl;
        if (i == 1){
            //creiamo il collegamento con il server 3
            firstNetworkP2P.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
            firstNetworkP2P.SetChannelAttribute("Delay", StringValue("20ms"));
        }
        else{
            firstNetworkP2P.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
            firstNetworkP2P.SetChannelAttribute("Delay", StringValue("20ms"));
        }
        NetDeviceContainer tempDevices = firstNetworkP2P.Install(firstNetNodes.Get(2), firstNetNodes.Get(i));

        firstNetworkDevice.Add(tempDevices.Get(0)); //tempDevices.Get(0) restituisce l'interfaccia del collegamento
                                             //rispetto al nodo centrale
                                             //es. tempDevices.Get(1) restituirebbe l'interfaccia del collegamento
                                             //rispetto al nodo i-esimo
    }

    stack.Install(firstNetNodes);
    address.SetBase("10.1.1.0", "255.255.255.248");
    Ipv4InterfaceContainer firstNetworkInterfaces = address.Assign(firstNetworkDevice);


    for (uint32_t i = 0; i < firstNetworkInterfaces.GetN(); i++){

        std::cout << "Indirizzo IP (first network)" << i << ": " << firstNetworkInterfaces.GetAddress(i) << std::endl;
    }
    */
    //************* FINE RETE 1 *************








}
