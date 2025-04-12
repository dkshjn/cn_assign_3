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

# Task-2
