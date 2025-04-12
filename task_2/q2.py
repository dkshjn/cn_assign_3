#!/usr/bin/python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import OVSBridge
from mininet.link import TCLink
from mininet.cli import CLI
from mininet.log import setLogLevel
import time
import os

class NATTopo(Topo):
    def build(self):

        # Switches
        s1 = self.addSwitch('s1', stp=True)
        s2 = self.addSwitch('s2', stp=True)
        s3 = self.addSwitch('s3', stp=True)
        s4 = self.addSwitch('s4', stp=True)

        # Switch for hosts(h1 and h2) behind NAT
        s5 = self.addSwitch('s5') 

        # Hosts outside NAT (same as q1).
        h3 = self.addHost('h3', ip='10.0.0.4/24', defaultRoute='via 10.0.0.1')
        h4 = self.addHost('h4', ip='10.0.0.5/24', defaultRoute='via 10.0.0.1')
        h5 = self.addHost('h5', ip='10.0.0.6/24', defaultRoute='via 10.0.0.1')
        h6 = self.addHost('h6', ip='10.0.0.7/24', defaultRoute='via 10.0.0.1')
        h7 = self.addHost('h7', ip='10.0.0.8/24', defaultRoute='via 10.0.0.1')
        h8 = self.addHost('h8', ip='10.0.0.9/24', defaultRoute='via 10.0.0.1')

        # NAT Gateway Host (h9)
        natGW = self.addHost('h9', ip=None)

        # Hosts behind NAT (Private Network)
        h1 = self.addHost('h1', ip=None)
        h2 = self.addHost('h2', ip=None)

        # Switch-Switch links (Latency: 7ms)
        self.addLink(s1, s2, cls=TCLink, delay='7ms')
        self.addLink(s2, s3, cls=TCLink, delay='7ms')
        self.addLink(s3, s4, cls=TCLink, delay='7ms')
        self.addLink(s4, s1, cls=TCLink, delay='7ms')
        self.addLink(s1, s3, cls=TCLink, delay='7ms')

        # Public Host-Switch links (Latency: 5ms)
        self.addLink(h3, s2, cls=TCLink, delay='5ms')
        self.addLink(h4, s2, cls=TCLink, delay='5ms')
        self.addLink(h5, s3, cls=TCLink, delay='5ms')
        self.addLink(h6, s3, cls=TCLink, delay='5ms')
        self.addLink(h7, s4, cls=TCLink, delay='5ms')
        self.addLink(h8, s4, cls=TCLink, delay='5ms')

        # NAT Gateway Connections
        # Public side: h9 connects to s1
        self.addLink(natGW, s1, cls=TCLink, delay='5ms', intfName1='h9-eth0')
        # Private side: h9 connects to the private switch s5
        self.addLink(natGW, s5, cls=TCLink, delay='1ms', intfName1='h9-eth1')

        # Private Host Connections (to private switch s5)
        self.addLink(h1, s5, cls=TCLink, delay='1ms')
        self.addLink(h2, s5, cls=TCLink, delay='1ms')

def configure_nat(net):
    h1 = net.get('h1')
    h2 = net.get('h2')
    h9 = net.get('h9') # NAT Gateway

    # h1 configuration
    h1.cmd("ifconfig h1-eth0 10.1.1.2/24 up")
    h1.cmd("ip route add default via 10.1.1.1")
    print(f"h1 IP: {h1.IP()}")

    # h2 configuration
    h2.cmd("ifconfig h2-eth0 10.1.1.3/24 up")
    h2.cmd("ip route add default via 10.1.1.1")
    print(f"h2 IP: {h2.IP()}")

    # Public interface h9 (connected to s1)
    h9.cmd("ifconfig h9-eth0 10.0.0.1/24 up")
    h9.cmd("ip addr add 172.16.10.10/24 dev h9-eth0")
    h9.cmd("ip addr add 172.16.10.11/24 dev h9-eth0")
    h9.cmd("ip addr add 172.16.10.12/24 dev h9-eth0")

    # Private interface h9 (connected to s_priv)
    h9.cmd("ifconfig h9-eth1 10.1.1.1/24 up")

    # Enable IP forwarding
    h9.cmd("sysctl -w net.ipv4.ip_forward=1")
    print("Enabled IP forwarding on h9")

    # Flush existing rules
    h9.cmd("iptables -F")
    h9.cmd("iptables -t nat -F")
    h9.cmd("iptables -X")
    h9.cmd("iptables -t nat -X")

    # --- NAT Table Rules ---

    # Outbound NAT: Masquerade traffic from private net leaving public interface
    h9.cmd("iptables -t nat -A POSTROUTING -s 10.1.1.0/24 -o h9-eth0 -j MASQUERADE")

    # Inbound DNAT: Redirect traffic destined for public alias IPs to internal hosts
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.11 -p icmp -j DNAT --to-destination 10.1.1.2")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.12 -p icmp -j DNAT --to-destination 10.1.1.3")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.11 -p tcp --dport 5201 -j DNAT --to-destination 10.1.1.2:5201")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.12 -p tcp --dport 5201 -j DNAT --to-destination 10.1.1.3:5201")

    # --- Filter Table Rules ---
    # Allow traffic originating from private network going out
    h9.cmd("iptables -A FORWARD -i h9-eth1 -o h9-eth0 -s 10.1.1.0/24 -j ACCEPT")

    # Allow returning traffic coming from public net to private net
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -m state --state RELATED,ESTABLISHED -j ACCEPT")

    # Allow new incoming connections that were DNAT'd
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -p icmp -d 10.1.1.2 -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -p icmp -d 10.1.1.3 -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -p tcp -d 10.1.1.2 --dport 5201 -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -p tcp -d 10.1.1.3 --dport 5201 -j ACCEPT")

    # Configure routes on public hosts to reach the 172.16.10.x network via h9's 10.0.0.1 IP
    print("\n*** Adding routes on public hosts...")
    for h_name in ['h3', 'h4', 'h5', 'h6', 'h7', 'h8']:
        h = net.get(h_name)
        print(f"Routes on {h_name}:")
        print(h.cmd("ip route"))

def run():
    topo = NATTopo()
    net = Mininet(topo=topo, link=TCLink, switch=OVSBridge, controller=None)

    net.start()
    configure_nat(net)
    CLI(net)

    net.stop()


if __name__ == '__main__':
    setLogLevel('info')
    run()
