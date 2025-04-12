# Computer Networks (CS-331) - Assignment 3

## Authors
Daksh Jain (22110066), Harshit (22110095)

# Task-1

## Steps to Run the Topology and Perform Ping Tests

## 1. Run the Topology without STP

```bash
sudo mn -c
sudo python3 q1.py
```
## 2. Test Pings without STP

```bash
h3 ping -c 1 h1
h5 ping -c 1 h7
h8 ping -c 1 h2
```

All pings will fail (100% packet loss).

## 3. Enable STP in the Code
In q1.py file, ```stp=True``` is added:

```bash
s1 = self.addSwitch('s1', stp=True)
s2 = self.addSwitch('s2', stp=True)
s3 = self.addSwitch('s3', stp=True)
s4 = self.addSwitch('s4', stp=True)
```

## 4. Run the Topology Again (With STP)

```bash
sudo mn -c
sudo python3 q1.py
```

## 5. Test Pings Again (With STP Enabled)
Run each ping 3 times with atleast 30 seconds gap:

```bash
h3 ping -c 1 h1
h5 ping -c 1 h7
h8 ping -c 1 h2
```
All pings will now work.

# Task-2 : Host-based NAT Configuration

## Overview

This assignment involves configuring and testing Host-based NAT functionality within a Mininet network. The updated topology includes a new host (`h9`) acting as a NAT gateway, with public and private IPs, enabling communication between internal and external hosts. The tests include pinging between internal and external hosts, as well as running `iperf3` tests to measure bandwidth.

## Topology Update

- **New Host**: `h9` is added to Switch `S1`, connected with a 5ms delay. Hosts `h1` and `h2` are moved from `S1` to `h9`, establishing `h1-h9` and `h2-h9` links.
- **NAT Configuration**: Host `h9` will act as the NAT gateway with a public IP (`172.16.10.10`) and private IPs for internal hosts (`h1: 10.1.1.2` and `h2: 10.1.1.3`).
- **Communication**: Test communication between internal hosts (`h1`, `h2`) and external hosts (`h3`, `h5`, `h6`, `h8`), including IP forwarding, SNAT, and DNAT rules.

## Setup

1. **Install Mininet**:
   - Install Mininet by following the [Mininet Installation Guide](http://mininet.org/download/).
   - Ensure Python 3.x is installed along with Mininet and its dependencies.

2. **Create the Updated Topology**:
   - The updated topology includes the new NAT host (`h9`), private IPs for internal hosts, and connections with delays.

3. **Run Mininet with Custom Python Script**:
   - Save the Python script in a file named q2.py and run Mininet with the following commands:
   
     ```bash
     sudo mn -c
     sudo python3 q2.py
     ```

4. **Configure IP Forwarding and NAT Rules**:
   - Enable IP forwarding on `h9` to forward traffic between the internal and external networks.
   - Configure `iptables` for SNAT (source NAT) and DNAT (destination NAT) on `h9`.

---

## Testing Procedure

### Part A: Test Communication to External Hosts from Internal Hosts

1. **Ping from h1 to h5 (External)**:
   - Test whether host `h1` (internal) can ping `h5` (external).
   - In Mininet CLI, run:
     ```bash
     mininet> h1 ping -c 1 10.0.0.6
     ```

2. **Ping from h2 to h3 (External)**:
   - Test whether host `h2` (internal) can ping `h3` (external).
   - In Mininet CLI, run:
     ```bash
     mininet> h2 ping -c 1 10.0.0.3
     ```

   - **Expected Outcome**: Successful pings indicate that the NAT gateway (h9) is correctly forwarding packets from the internal network to the external network, using SNAT.

### Part B: Test Communication to Internal Hosts from External Hosts

1. **Ping from h8 to h1 (Internal)**:
   - Test whether an external host (`h8`) can ping internal host `h1`.
   - In Mininet CLI, run:
     ```bash
     mininet> h8 ping -c 1 172.16.10.11
     ```

2. **Ping from h6 to h2 (Internal)**:
   - Test whether an external host (`h6`) can ping internal host `h2`.
   - In Mininet CLI, run:
     ```bash
     mininet> h6 ping -c 1 172.16.10.12
     ```

   - **Expected Outcome**: The pings should be successful, indicating that the DNAT rules set on h9 are correctly translating the public IP (`172.16.10.11` for `h1` and `172.16.10.12` for `h2`) back to their private IPs.

### Part C: Run `iperf3` Bandwidth Tests

1. **`iperf3` Test 1: h1 (Server) and h6 (Client)**:
   - Start an `iperf3` server on `h1`:
     ```bash
     h1 iperf3 -s &
     ```
   - Run the `iperf3` client on `h6`:
     ```bash
     h6 iperf3 -c 172.16.10.11 -t 120
     ```

2. **`iperf3` Test 2: h8 (Server) and h2 (Client)**:
   - Start an `iperf3` server on `h8`:
     ```bash
     h8 iperf3 -s &
     ```
   - Run the `iperf3` client on `h2`:
     ```bash
     h2 iperf3 -c 172.16.10.12 -t 120
     ```

3. **`iperf3` Test 3: h1 (Server) and h2 (Client)**:
   - Start an `iperf3` server on `h1`:
     ```bash
     h1 iperf3 -s &
     ```
   - Run the `iperf3` client on `h2`:
     ```bash
     h2 iperf3 -c 172.16.10.11 -t 120
     ```

   - **Expected Outcome**: Bandwidth results should be measured and displayed in the `iperf3` outputs. If NAT and routing are correctly configured, these tests will show the throughput between internal and external hosts.

---

## NAT Rules Configuration

1. **SNAT (Source NAT)**:
   - For outbound traffic from internal hosts to external hosts, configure SNAT on `h9`:
     ```bash
     h9 cmd('iptables -t nat -A POSTROUTING -o h9-eth0 -j MASQUERADE')
     ```

2. **DNAT (Destination NAT)**:
   - For inbound traffic to internal hosts from external sources, configure DNAT on `h9`:
     ```bash
     h9 cmd('iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.11 -j DNAT --to-destination 10.1.1.2')
     h9 cmd('iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.12 -j DNAT --to-destination 10.1.1.3')
     ```
---
