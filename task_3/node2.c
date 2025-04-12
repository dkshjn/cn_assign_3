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

struct distance_table 
{
  int costs[4][4];
} dt2;


/* students to write the following two routines, and maybe some others */

void rtinit2() 
{
  printf("Node 2: Setting up...\n");

  int i, j;
  // Distance table entries initialized for node 2 to a large value 999 (inf)
  for (i = 0; i < 4; i++){
    for (j = 0; j < 4; j++){
      dt2.costs [i][j] = 999;
    }
  }

  // Setting costs for each connection
  dt2.costs [0][0] = 3; // Cost to Node 0 is 3.
  dt2.costs [1][1] = 1; // Cost to reach Node 1 is 1.
  dt2.costs [2][2] = 0; // Cost to Node 2 itself is 0.
  dt2.costs [3][3] = 2; // Cost to reach Node 3 is 2.

  // Initial Distance vector packet to be sent to neighbors
  struct rtpkt initial_pkt;
  initial_pkt.sourceid = 2;


  // Minimum cost to each destination node
  int dest_node = 0;
  for (dest_node = 0; dest_node < 4; dest_node++)
    initial_pkt.mincost [dest_node] = dt2.costs[dest_node][dest_node];

  // Send the initial distance vector packet to all directly connected neighbors.

  // Node - 0
  initial_pkt.destid = 0;
  tolayer2(initial_pkt);

  // Node - 1
  initial_pkt.destid = 1;
  tolayer2(initial_pkt);

  // Node - 3
  initial_pkt.destid = 3;
  tolayer2(initial_pkt); 

  printf("Node 2: Initial distance vector sent to neighbors 0, 1, and 3.\n");

  // Print the initial distance table for Node - 2
  printdt2(&dt2);
  printf("Node 2: Initialization complete.\n\n");
}


void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
  {
    printf("Node 2: Received a routing update.\n");
  
    int neighbor_id = rcvdpkt->sourceid; // Neighbor ID which is sending the update.
    int *neighbor_dv = rcvdpkt->mincost; // Distance vector is obtained from the neighbor's packet.
    int cost_changed = 0; // To track if costs changed.
    int dest_node;
  
    printf("Node 2: Update received from Node %d with DV: {%d, %d, %d, %d}\n",
           neighbor_id, neighbor_dv[0], neighbor_dv[1], neighbor_dv[2], neighbor_dv[3]);
  
    // Iterate through all possible destination nodes.
    for (dest_node = 0; dest_node < 4; dest_node++){
      // Calculate the potential cost to reach 'dest_node' by routing through the 'neighbor_id'.

      // Cost is calculated as follows:
      // Cost = (Cost from Node 2 to neighbor_id) + (Neighbor's reported cost to dest_node)
      // This is the Bellman-Ford equation.
      int cost_via_neighbor = dt2.costs[neighbor_id][neighbor_id] + neighbor_dv[dest_node];
  
      if (cost_via_neighbor > 999) cost_via_neighbor = 999;
  
      // Compare this calculated cost with the currently stored cost to reach 'dest_node' via 'neighbor_id'.
      if(cost_via_neighbor < dt2.costs[dest_node][neighbor_id]){
        // If A shorter path to 'dest_node' via 'neighbor_id' is found then update the table.
        dt2.costs[dest_node][neighbor_id] = cost_via_neighbor;
        printf("Node 2: Found shorter path to Node %d via Node %d (New cost: %d).\n",
               dest_node, neighbor_id, cost_via_neighbor);
  
        // Change the flag to 1 to indicate that the cost has changed.
        cost_changed = 1;
      }
    }
  
    // If cost_changed is 1, it means the distance table has been updated.
    if (cost_changed == 1) {
      printf("Node 2: Distance table updated.\n");
  
      // Prepare a new packet for sending Node 2's updated distance vector.
      struct rtpkt outgoing_pkt;
      outgoing_pkt.sourceid = 2;
  
      // Recalculate Node 2's minimum cost distance vector.
      int current_dv[4]; // Stores the newly calculated min costs from Node 2.
      int i_dest, j_via;
  
      for (i_dest = 0; i_dest < 4; i_dest++) {
          int current_min = 999;
          for (j_via = 0; j_via < 4; j_via++) {
              if (dt2.costs[i_dest][j_via] < current_min) {
                  current_min = dt2.costs[i_dest][j_via];
              }
          }
          current_dv[i_dest] = current_min; // Stores the minimum cost found for 'i_dest'.
      }

      for (i_dest = 0; i_dest < 4; i_dest++) {
        dt2.costs[i_dest][i_dest] = current_dv[i_dest];
      }
  
      // Copy the recalculated distance vector into the outgoing packet.
      for (i_dest = 0; i_dest < 4; i_dest++) {
          outgoing_pkt.mincost[i_dest] = current_dv[i_dest];
      }
  
      // Send the updated distance vector packet to all direct neighbors.
       printf("Node 2: Sending updated DV {%d, %d, %d, %d} to neighbors 0, 1, and 3.\n",
             current_dv[0], current_dv[1], current_dv[2], current_dv[3]);
  
      // Node - 0
      outgoing_pkt.destid = 0;
      tolayer2(outgoing_pkt);
  
      // Node - 1
      outgoing_pkt.destid = 1;
      tolayer2(outgoing_pkt);
  
      // Node - 3
      outgoing_pkt.destid = 3;
      tolayer2(outgoing_pkt);
  
      printf("Node 2: Updated distance vector broadcast to neighbors.\n");
  
      // Print the updated distance table for Node 2.
      printdt2(&dt2);
  
    } else {
         printf("Node 2: Received update but no changes were made.\n");
    }
     printf("Node 2: Finished processing update from Node %d.\n\n", neighbor_id);
}


printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}






