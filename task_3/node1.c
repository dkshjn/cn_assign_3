#include <stdio.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };


extern int TRACE;
extern int YES;
extern int NO;

int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table 
{
  int costs[4][4];
} dt1;


/* students to write the following two routines, and maybe some others */


rtinit1() 
{
  printf("Node 1: Setting up...\n");

  int i, j;
  // Distance table entries initialized for node 1 to a large value 999 (inf)
  for (i = 0; i < 4; i++){
    for (j = 0; j < 4; j++){
      dt1.costs [i][j] = 999;
    }
  }

  // Setting costs for each connection
  dt1.costs [0][0] = 1; // Cost to Node 0 is 1.
  dt1.costs [1][1] = 0; // Cost to itself is 0.
  dt1.costs [2][2] = 1; // Cost to reach Node 2 is 1.

  // Initial Distance vector packet to be sent to neighbors
  struct rtpkt initial_pkt;
  initial_pkt.sourceid = 1;


  // Minimum cost to each destination node
  int dest_node = 0;
  for (dest_node = 0; dest_node < 4; dest_node++)
    initial_pkt.mincost [dest_node] = dt1.costs[dest_node][dest_node];

  // Send the initial distance vector packet to all directly connected neighbors.

  // Node - 0
  initial_pkt.destid = 0;
  tolayer2(initial_pkt);

  // Node - 2
  initial_pkt.destid = 2;
  tolayer2(initial_pkt);
 

  printf("Node 1: Initial distance vector sent to neighbors 0 and 2.\n");

  // Print the initial distance table for Node - 1
  printdt1(&dt1);
  printf("Node 1: Initialization complete.\n\n");
}


rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;  
  {
    printf("Node 1: Received a routing update.\n");
  
    int neighbor_id = rcvdpkt->sourceid; // Neighbor ID which is sending the update.
    int *neighbor_dv = rcvdpkt->mincost; // Distance vector is obtained from the neighbor's packet.
    int cost_changed = 0; // To track if costs changed.
    int dest_node;
  
    printf("Node 1: Update received from Node %d with DV: {%d, %d, %d, %d}\n",
           neighbor_id, neighbor_dv[0], neighbor_dv[1], neighbor_dv[2], neighbor_dv[3]);
  
    // Iterate through all possible destination nodes.
    for (dest_node = 0; dest_node < 4; dest_node++){
      // Calculate the potential cost to reach 'dest_node' by routing through the 'neighbor_id'.

      // Cost is calculated as follows:
      // Cost = (Cost from Node 1 to neighbor_id) + (Neighbor's reported cost to dest_node)
      // This is the Bellman-Ford equation.
      int cost_via_neighbor = dt1.costs[neighbor_id][neighbor_id] + neighbor_dv[dest_node];
  
      if (cost_via_neighbor > 999) cost_via_neighbor = 999;
  
      // Compare this calculated cost with the currently stored cost to reach 'dest_node' via 'neighbor_id'.
      if(cost_via_neighbor < dt1.costs[dest_node][neighbor_id]){
        // If A shorter path to 'dest_node' via 'neighbor_id' is found then update the table.
        dt1.costs[dest_node][neighbor_id] = cost_via_neighbor;
        printf("Node 1: Found shorter path to Node %d via Node %d (New cost: %d).\n",
               dest_node, neighbor_id, cost_via_neighbor);
  
        // Change the flag to 1 to indicate that the cost has changed.
        cost_changed = 1;
      }
    }
  
    // If cost_changed is 1, it means the distance table has been updated.
    if (cost_changed == 1) {
      printf("Node 1: Distance table updated.\n");
  
      // Prepare a new packet for sending Node 1's updated distance vector.
      struct rtpkt outgoing_pkt;
      outgoing_pkt.sourceid = 1;
  
      // Recalculate Node 1's minimum cost distance vector.
      int current_dv[4]; // Stores the newly calculated min costs from Node 1.
      int i_dest, j_via;
  
      for (i_dest = 0; i_dest < 4; i_dest++) {
          int current_min = 999;
          for (j_via = 0; j_via < 4; j_via++) {
              if (dt1.costs[i_dest][j_via] < current_min) {
                  current_min = dt1.costs[i_dest][j_via];
              }
          }
          current_dv[i_dest] = current_min; // Stores the minimum cost found for 'i_dest'.
      }

      for (i_dest = 0; i_dest < 4; i_dest++) {
        dt1.costs[i_dest][i_dest] = current_dv[i_dest];
      }
  
      // Copy the recalculated distance vector into the outgoing packet.
      for (i_dest = 0; i_dest < 4; i_dest++) {
          outgoing_pkt.mincost[i_dest] = current_dv[i_dest];
      }
  
      // Send the updated distance vector packet to all direct neighbors.
       printf("Node 1: Sending updated DV {%d, %d, %d, %d} to neighbors 0 and 2.\n",
             current_dv[0], current_dv[1], current_dv[2], current_dv[3]);
  
      // Node - 0
      outgoing_pkt.destid = 0;
      tolayer2(outgoing_pkt);
  
      // Node - 2
      outgoing_pkt.destid = 2;
      tolayer2(outgoing_pkt);
  
      printf("Node 1: Updated distance vector broadcast to neighbors.\n");
  
      // Print the updated distance table for Node 1.
      printdt1(&dt1);
  
    } else {
         printf("Node 1: Received update but no changes were made.\n");
    }
     printf("Node 1: Finished processing update from Node %d.\n\n", neighbor_id);
  }


printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



linkhandler1(linkid, newcost)   
int linkid, newcost;   
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
  int old_direct_cost = dt1.costs[linkid][linkid];

  printf("\nNode 1: Link cost change detected for neighbor %d.\n", linkid);
  printf("Node 1: Previous direct cost = %d, New direct cost = %d.\n", old_direct_cost, newcost);

  // Update the distance table entry that represents the direct link cost.
  dt1.costs[linkid][linkid] = newcost;

  // Flag to check if changes in Node 1's overall shortest path costs.
  int shortest_paths_changed = 0;

  // Store the newly computed distance vector for Node 1.
  int recalculated_dv[4];

  printf("Node 1: Recalculating all shortest paths due to link cost change...\n");

  int dest_node, via_node;
  // Recaculate Node 1's minimum cost to every possible destination node.
  for (dest_node = 0; dest_node < 4; dest_node++) {
    int current_min_for_dest = 999;

    for (via_node = 0; via_node < 4; via_node++) {
        // Checks if the path to 'dest_node' via 'via_node' is cheaper.
        if (dt1.costs[dest_node][via_node] < current_min_for_dest) {
            current_min_for_dest = dt1.costs[dest_node][via_node];
        }
    }
    // Check if the new cost to reach 'dest_node' is less than the current minimum.
    if (current_min_for_dest != dt1.costs[dest_node][dest_node]) {
        printf("Node 1: Minimum cost to node %d changed (was %d, now %d). Neighbors to be notified.\n",
               dest_node, dt1.costs[dest_node][dest_node], current_min_for_dest);
        // Overwrite the dt1.costs[dest_node][dest_node] entry with the new overall minimum.
        dt1.costs[dest_node][dest_node] = current_min_for_dest;
        shortest_paths_changed = 1;
    }

    // Store the calculated absolute minimum cost.
    recalculated_dv[dest_node] = current_min_for_dest;
  }


  // If change in the shortest path costs detected, prepare to send the updated distance vector.
  if (shortest_paths_changed == 1) {
    printf("Node 1: Shortest paths updated. Preparing to broadcast new distance vector.\n");

    // Set up the routing packet to send.
    struct rtpkt update_packet;
    update_packet.sourceid = 1;

    // Load the recalculated distance vector into the packet.
    int idx;
    for (idx = 0; idx < 4; idx++) {
        update_packet.mincost[idx] = recalculated_dv[idx];
    }

    // Send the packet to each neighbor via layer 2 simulation function.
    printf("Node 1: Broadcasting updated DV {%d, %d, %d, %d} to neighbors 0 and 2.\n",
           recalculated_dv[0], recalculated_dv[1], recalculated_dv[2], recalculated_dv[3]);

    // Node - 0
    update_packet.destid = 0;
    tolayer2(update_packet);

    // Node - 2
    update_packet.destid = 2;
    tolayer2(update_packet);

    printf("Node 1: Update packets sent to all neighbors.\n");

  } else {
       printf("Node 1: Link cost change did not affect the overall shortest paths, therefore no broadcast needed.\n");
  }

  // Print the updated state of Node 1's distance table.
  printf("Node 1: Current distance table state after handling link change to %d:\n", linkid);
  printdt1(&dt1);
  printf("Node 1: Finished handling link cost change.\n\n");
}

