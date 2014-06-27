/* For copyright information, see olden_v1.0/COPYRIGHT */

/* make_graph.c - Create a graph to be solved for the electromagnetic
 *                problem in 3 dimensions.
 *
 * By:  Martin C. Carlisle
 * Date: Feb 23, 1994
 *
 */

#define SEED1 793
#define SEED2 39
#define SEED3 17

#include "em3d.h"
#include "util.h"

extern int NumNodes;
int NumMisses;

int n_nodes;
int d_nodes;
int local_p;

node_t **make_table(int size, int procname) {
  node_t **retval = (node_t **)malloc(size*sizeof(node_t *));
  assert(retval);
  return retval;
}

/* We expect node_table to be a local table of e or h nodes */
void fill_table(node_t **node_table, double *values, int size, int procname)
{
  node_t *cur_node, *prev_node;
  int i;
  
  prev_node = (node_t *)malloc(sizeof(node_t));
  node_table[0] = prev_node;
  *values = gen_uniform_double();
  prev_node->value = values++;
  prev_node->from_count = 0;
  
  /* Now we fill the node_table with allocated nodes */
  for (i=1; i<size; i++) {
    cur_node = (node_t *)malloc(sizeof(node_t));
    *values = gen_uniform_double();
    cur_node->value = values++;
    cur_node->from_count = 0;
    node_table[i] = cur_node;
    prev_node->next = cur_node;
    prev_node = cur_node;
  }
  cur_node->next = NULL;
}

void make_neighbors(node_t *nodelist, node_t **table[], int tablesz,
		    int degree, int percent_local, int id)
{
  node_t *cur_node;

  for(cur_node = nodelist; cur_node; cur_node=cur_node->next) {
    node_t *other_node;
    int j,k;
    int dest_proc;

    cur_node->to_nodes = (node_t **)malloc(degree*(sizeof(node_t *)));
    if (!cur_node->to_nodes) {
      chatting("Uncaught malloc error\n");
      exit(0);
    }

    for (j=0; j<degree; j++) {
      do {
        node_t **local_table;
        int number = gen_number(tablesz);

        if (check_percent(percent_local)) {
          dest_proc = id;
        } else {
          dest_proc = (id + PROCS + 4*gen_signed_number(1)) % PROCS;
        }

        /* We expect these accesses to be remote */
        local_table = table[dest_proc];
        other_node = local_table[number];   /* <------ 4% load miss penalty */
        if (!other_node) {
          chatting("Error! on dest %d @ %d\n",number,dest_proc);
          exit(1);
        }

        for (k=0; k<j; k++)
          if (other_node == cur_node->to_nodes[k]) break;

#if 0
        if ((((unsigned long long) other_node) >> 7) < 2048)
          chatting("pre other_node = 0x%x,number = %d,dest = %d\n",
                   other_node,number,dest_proc);
#endif
      }

      while (k<j);

      if (!cur_node || !cur_node->to_nodes) {
        chatting("Error! no to_nodes filed!\n");
        exit(1);
      }

      cur_node->to_nodes[j]=other_node;       /* <------ 6.5% store penalty */
#if 0
      if ((((unsigned long long) other_node) >> 7) < 2048)
        chatting("post other_node = 0x%x\n",other_node);
#endif
      ++other_node->from_count;            /* <----- 12% load miss penalty */
    }
  }
}

void update_from_coeffs(node_t *nodelist) {
  node_t *cur_node;
  
  /* Setup coefficient and from_nodes vectors for h nodes */  
  for (cur_node = nodelist; cur_node; cur_node=cur_node->next) {
    int from_count = cur_node->from_count;
    
    if (from_count < 1) {
      chatting("Help! no from count (from_count=%d) \n", from_count);
      cur_node->from_values = (double **)malloc(20 * sizeof(double *));
      cur_node->coeffs = (double *)malloc(20 * sizeof(double));
      cur_node->from_length = 0;
    } else {
      cur_node->from_values = (double **)malloc(from_count * sizeof(double *));
      cur_node->coeffs = (double *)malloc(from_count * sizeof(double));
      cur_node->from_length = 0;
    }
  }
}

void fill_from_fields(node_t *nodelist, int degree) {
  node_t *cur_node;
  for(cur_node = nodelist; cur_node; cur_node = cur_node->next) {
    int j;

    for (j=0; j<degree; j++) {
      int count,thecount;
      node_t *other_node = cur_node->to_nodes[j]; /* <-- 6% load miss penalty */
      double **otherlist;
      double *value = cur_node->value;

      if (!other_node) chatting("Help!!\n");
      count=(other_node->from_length)++;  /* <----- 30% load miss penalty */
      otherlist=other_node->from_values;  /* <----- 10% load miss penalty */
      thecount=other_node->from_count;
      if (!otherlist) {
        /*chatting("node 0x%p list 0x%p count %d\n",
                 other_node,otherlist,thecount);*/
        otherlist = other_node->from_values;
        /*chatting("No from list!! 0x%p\n",otherlist);*/
      }
      
      otherlist[count] = value;                 /* <------ 42% store penalty */

      /* <----- 42+6.5% store penalty */
      other_node->coeffs[count]=gen_uniform_double();
    }
  }
}

void localize_local(node_t *nodelist) {
  node_t *cur_node;

  for(cur_node = nodelist; cur_node; cur_node = cur_node->next) {
    cur_node->coeffs = cur_node->coeffs;
    cur_node->from_values =  cur_node->from_values;
    cur_node->value =  cur_node->value;
  }
}


void make_tables(table_t *table,int groupname) {
  node_t **h_table,**e_table;
  double *h_values, *e_values;
  int procname = 0;

  init_random(SEED1*groupname);
  h_values = (double *)malloc(n_nodes/PROCS*sizeof(double));
  h_table = make_table(n_nodes/PROCS,procname);
  fill_table(h_table,h_values,n_nodes/PROCS,procname);
  e_values = (double *)malloc(n_nodes/PROCS*sizeof(double));
  e_table = make_table(n_nodes/PROCS,procname);
  fill_table(e_table,e_values,n_nodes/PROCS,procname);

  /* This is done on procname-- we expect table to be remote */
  /* We use remote writes */
  table->e_table[groupname] = e_table;
  table->h_table[groupname] = h_table;
}

void make_all_neighbors(table_t *table,int groupname) {
  node_t *first_node;
  node_t **local_table;
  node_t ***local_table_array;

  init_random(SEED2*groupname);
  /* We expect table to be remote */
  local_table = table->h_table[groupname];
  local_table_array = table->e_table;
  first_node = local_table[0];
  make_neighbors(first_node,
		 local_table_array,n_nodes/PROCS,
		 d_nodes,local_p,groupname);

  local_table = table->e_table[groupname];
  local_table_array = table->h_table;
  first_node = local_table[0];
  make_neighbors(first_node,
		 local_table_array,n_nodes/PROCS,
		 d_nodes,local_p,groupname);
}

void update_all_from_coeffs(table_t *table, int groupname)    
{
  node_t **local_table;
  node_t *first_node;

  /* Done by do_all, table not local */
  local_table = table->h_table[groupname];
  /* We expect this to be local */
  first_node = local_table[0];
  update_from_coeffs(first_node);

  local_table = table->e_table[groupname];
  first_node = local_table[0];
  update_from_coeffs(first_node);
}

void fill_all_from_fields(table_t *table, int groupname)
{
  node_t **local_table;
  node_t *first_node;

  init_random(SEED3*groupname);
  local_table = table->h_table[groupname];
  first_node = local_table[0];
  fill_from_fields(first_node,d_nodes);

  local_table = table->e_table[groupname];
  first_node = local_table[0];
  fill_from_fields(first_node,d_nodes);
}

void localize(table_t *table, int groupname)
{
  node_t **local_table;
  node_t *first_node;

  local_table = table->h_table[groupname];
  first_node = local_table[0];
  localize_local(first_node);

  local_table = table->e_table[groupname];
  first_node = local_table[0];
  localize_local(first_node);
}
  
void clear_nummiss(table_t *table, int groupname)
{
  NumMisses = 0;
}

void do_all(table_t *table, int groupname, int nproc,
	    void func(table_t *, int),int groupsize) {
  /*chatting("do all group %d with %d\n",groupname,nproc);*/
  if (nproc > 1) {
    do_all(table,groupname+nproc/2,nproc/2,func,groupsize);
    do_all(table,groupname,nproc/2,func,groupsize);
  } else {
    func(table,groupname);
  }
}

graph_t *initialize_graph() {
  table_t *table;
  graph_t *retval;
  int i,j,blocksize;
  int groupsize;

  table = (table_t *)malloc(sizeof(table_t));
  retval = (graph_t *)malloc(sizeof(graph_t));

  groupsize = PROCS/NumNodes;

  chatting("making tables \n");
  do_all(table,0,PROCS,make_tables,groupsize);

  /* At this point, for each h node, we give it the appropriate number
     of neighbors as defined by the degree */
  chatting("making neighbors\n");

  do_all(table,0,PROCS,make_all_neighbors,groupsize);

  /* We now create from count and initialize coefficients */
  chatting("updating from and coeffs\n");
  do_all(table,0,PROCS,update_all_from_coeffs,groupsize);

  /* Fill the from fields in the nodes */
  chatting("filling from fields\n");
  do_all(table,0,PROCS,fill_all_from_fields,groupsize);

  chatting("localizing coeffs, from_nodes\n");
  do_all(table,0,PROCS,localize,groupsize);

  blocksize = PROCS/NumNodes;

  chatting("cleanup for return now\n");
  for (i=0; i<NumNodes; i++) {
    node_t **local_table = table->e_table[i*blocksize];
    node_t *local_node_r = local_table[0];

    retval->e_nodes[i] = local_node_r;
      
    local_table = table->h_table[i*blocksize];
    local_node_r = local_table[0];
    retval->h_nodes[i] = local_node_r;
    for (j = 1; j < blocksize; j++) {
      node_t *local_node_l;

      local_table = table->e_table[i*blocksize+j-1];
      local_node_l = local_table[(n_nodes/PROCS)-1];
      local_table = table->e_table[i*blocksize+j];
      local_node_r = local_table[0];
      local_node_l->next = local_node_r;
      
      local_table = table->h_table[i*blocksize+j-1];
      local_node_l = local_table[(n_nodes/PROCS)-1];
      local_table = table->h_table[i*blocksize+j];
      local_node_r = local_table[0];
      local_node_l->next = local_node_r;
    }
  }
  
  chatting("Clearing NumMisses\n");
  do_all(table,0,PROCS,clear_nummiss,groupsize);
  chatting("Returning\n");

  return retval;
}
