# Computer Networks (CS-331) - Assignment 3
## Authors
Daksh Jain (22110066), Harshit (22110095)

# Task-1: Network Loops

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

---

# Task-2 : Configure Host-based NAT

## Topology Update

- **New Host**: `h9` is added to Switch `S1`, connected with a 5ms delay. Hosts `h1` and `h2` are moved from `S1` to `h9`, establishing `h1-h9` and `h2-h9` links.
- **NAT Configuration**: Host `h9` will act as the NAT gateway with a public IP (`172.16.10.10`) and private IPs for internal hosts (`h1: 10.1.1.2` and `h2: 10.1.1.3`).
- **Communication**: Test communication between internal hosts (`h1`, `h2`) and external hosts (`h3`, `h5`, `h6`, `h8`).

## Setup

1. **Install Mininet**:
   - Install Mininet by following the [Mininet Installation Guide](http://mininet.org/download/).
   - Ensure Python 3.x is installed along with Mininet and its dependencies.

2. **Run Mininet with Custom Python Script**:
   - Save the Python script in a file named `q2.py` and run Mininet with the following commands:

     ```bash
     sudo mn -c
     chmod +x q2.py
     sudo python3 q2.py
     ```
---

## Testing Procedure

### Part A: Test Communication to External Hosts from Internal Hosts

1. **Ping from h1 to h5 (External)**:
   - Test whether host `h1` (internal) can ping `h5` (external).
   - In Mininet CLI, run:
     ```bash
     h1 ping -c 1 10.0.0.6
     ```

2. **Ping from h2 to h3 (External)**:
   - Test whether host `h2` (internal) can ping `h3` (external).
   - In Mininet CLI, run:
     ```bash
     h2 ping -c 1 10.0.0.3
     ```

### Part B: Test Communication to Internal Hosts from External Hosts

1. **Ping from h8 to h1 (Internal)**:
   - Test whether an external host (`h8`) can ping internal host `h1`.
   - In Mininet CLI, run:
     ```bash
     h8 ping -c 1 172.16.10.11
     ```

2. **Ping from h6 to h2 (Internal)**:
   - Test whether an external host (`h6`) can ping internal host `h2`.
   - In Mininet CLI, run:
     ```bash
     h6 ping -c 1 172.16.10.12
     ```

We get successful pings which indicate that the NAT gateway (h9) is correctly forwarding packets from the internal network to the external network.

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

Run these two tests three times each.

---

# Task-3: Network Routing 

## Network Setup
The network topology is as follows:

- Cost between Node 0 and Node 1 → 1  
- Cost between Node 0 and Node 2 → 3  
- Cost between Node 0 and Node 3 → 7  
- Cost between Node 1 and Node 2 → 1  
- Cost between Node 2 and Node 3 → 2  
- All other connections are assumed to be disconnected with cost = 999 (infinity)

---

## Code Implementation

### Implemented Functions:
| Function        | Description |
|-----------------|-------------|
| rtinitX()       | Initializes the distance table for Node X. |
| rtupdateX()     | Updates the distance table of Node X upon receiving routing updates from neighbors. |
| linkhandler0()  | Handles dynamic link cost changes for Node 0 (Extra Credit). |
| linkhandler1()  | Handles dynamic link cost changes for Node 1 (Extra Credit). |

These functions were implemented in the respective files: `node0.c`, `node1.c`, `node2.c`, `node3.c`

## Running the Code

```bash
gcc distance_vector.c node0.c node1.c node2.c node3.c -o distance_vector
./distance_vector > results.txt
```
