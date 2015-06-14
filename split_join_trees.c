#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef int object_t;
typedef int key_t;
typedef struct tr_n_t {
	key_t key;
	struct tr_n_t *left;
	struct tr_n_t *right;
	int height;
} tree_node_t;

tree_node_t *currentblock = NULL;
int size_left;
tree_node_t *free_list = NULL;

tree_node_t *get_node() {
	tree_node_t *tmp;
	if (free_list != NULL) {
		tmp = free_list;
		free_list = free_list->left;
	} else {
		if (currentblock == NULL || size_left == 0) {
			currentblock = (tree_node_t *) malloc(
					BLOCKSIZE * sizeof(tree_node_t));
			size_left = BLOCKSIZE;
		}
		tmp = currentblock++;
		size_left -= 1;
	}
	return (tmp);
}

void return_node(tree_node_t *node) {
	node->left = free_list;
	free_list = node;
}

tree_node_t *create_tree(void) {
	tree_node_t *tmp_node;
	tmp_node = get_node();
	tmp_node->left = NULL;
	return (tmp_node);
}

void left_rotation(tree_node_t *n)
{  tree_node_t *tmp_node;
   key_t        tmp_key;
   tmp_node = n->left;
   tmp_key  = n->key;
   n->left  = n->right;
   n->key   = n->right->key;
   n->right = n->left->right;
   n->left->right = n->left->left;
   n->left->left  = tmp_node;
   n->left->key   = tmp_key;
}

void right_rotation(tree_node_t *n)
{  tree_node_t *tmp_node;
   key_t        tmp_key;
   tmp_node = n->right;
   tmp_key  = n->key;
   n->right = n->left;
   n->key   = n->left->key;
   n->left  = n->right->left;
   n->right->left = n->right->right;
   n->right->right  = tmp_node;
   n->right->key   = tmp_key;
}

object_t *find(tree_node_t *tree, key_t query_key)
{  tree_node_t *tmp_node;
   if( tree->left == NULL )
     return(NULL);
   else
   {  tmp_node = tree;
      while( tmp_node->right != NULL )
      {   if( query_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->key == query_key )
         return( (object_t *) tmp_node->left );
      else
         return( NULL );
   }
}


int insert(tree_node_t *tree, key_t new_key, object_t *new_object)
{  tree_node_t *tmp_node;
   int finished;
   if( tree->left == NULL )
   {  tree->left = (tree_node_t *) new_object;
      tree->key  = new_key;
      tree->height = 0;
      tree->right  = NULL;
   }
   else
     {  tree_node_t * path_stack[100]; int  path_st_p = 0;
      tmp_node = tree;
      while( tmp_node->right != NULL )
      {   path_stack[path_st_p++] = tmp_node;
          if( new_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      /* found the candidate leaf. Test whether key distinct */
      if( tmp_node->key == new_key )
         return( -1 );
      /* key is distinct, now perform the insert */
      {  tree_node_t *old_leaf, *new_leaf;
         old_leaf = get_node();
         old_leaf->left = tmp_node->left;
         old_leaf->key = tmp_node->key;
         old_leaf->right  = NULL;
         old_leaf->height = 0;
         new_leaf = get_node();
         new_leaf->left = (tree_node_t *) new_object;
         new_leaf->key = new_key;
         new_leaf->right  = NULL;
         new_leaf->height = 0;
         if( tmp_node->key < new_key )
         {   tmp_node->left  = old_leaf;
             tmp_node->right = new_leaf;
             tmp_node->key = new_key;
         }
         else
         {   tmp_node->left  = new_leaf;
             tmp_node->right = old_leaf;
         }
         tmp_node->height = 1;
      }
      /* rebalance */
      finished = 0;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         old_height= tmp_node->height;
         if( tmp_node->left->height -
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height -
                                 tmp_node->right->height == 1 )
            {  right_rotation( tmp_node );
               tmp_node->right->height =
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation( tmp_node->left );
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height;
               tmp_node->left->height  = tmp_height + 1;
               tmp_node->right->height = tmp_height + 1;
               tmp_node->height = tmp_height + 2;
            }
         }
         else if ( tmp_node->left->height -
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height -
                                  tmp_node->left->height == 1 )
            {  left_rotation( tmp_node );
               tmp_node->left->height =
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height;
               tmp_node->left->height  = tmp_height + 1;
               tmp_node->right->height = tmp_height + 1;
               tmp_node->height = tmp_height + 2;
            }
         }
         else /* update height even if there was no rotation */
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished = 1;
      }

   }
   return( 0 );
}

void check_tree( tree_node_t *tr, int depth, int lower, int upper )
{  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   if( tr->key < lower || tr->key >= upper )
         printf("Wrong Key Order \n");
   if( tr->right == NULL )
   {  if( *( (int *) tr->left) == 10*tr->key + 2 )
         printf("%d(%d)  ", tr->key, depth );
      else
         printf("Wrong Object \n");
   }
   else
   {  check_tree(tr->left, depth+1, lower, tr->key );
      check_tree(tr->right, depth+1, tr->key, upper );
   }
}



tree_node_t *join_two(tree_node_t *tree1, tree_node_t *tree2, key_t separating_key) {

	tree_node_t *result_tree,*tmp_node,*tmp_node2,*swap,*test;
	tree_node_t *new_node;

	//printf(" tree1 , tree2 and key are %d , %d and %d \n",tree1->key,tree2->key,separating_key);
	new_node=get_node();
	new_node->key=separating_key;

	result_tree=tree1;
	tmp_node=tree1;
	tmp_node2=tree2;

	tree_node_t * path_stack[100],*uppernode;
	int finished;
	int path_st_p = 0;


	if(tmp_node->height==tmp_node2->height){


		if(separating_key<=tmp_node2->key){

			new_node->left = tmp_node;
			new_node->right = tmp_node2;
			new_node->height = tmp_node2->height + 1;
			result_tree=new_node;


		}
		else{

			new_node->left = tmp_node2;
			new_node->right = tmp_node;
			new_node->height = tmp_node2->height + 1;
			result_tree=new_node;


		}



	}
	else{
		int check=0;
		int dup=0;
		if(tmp_node->height<tmp_node2->height){
			//swap=tmp_node;
			//tmp_node=tmp_node2;
			//tmp_node2=swap;


		}

	while (tmp_node->right != NULL && tmp_node->height != tmp_node2->height) {
			path_stack[path_st_p++] = tmp_node;
			tmp_node = tmp_node->left;

	}



	uppernode=path_stack[path_st_p-1];
	//printf("found node in t1 with same height as t2 is %d\n",tmp_node->key);
	new_node->left=tmp_node2;
	new_node->right=tmp_node;
	new_node->height=tmp_node2->height+1;


	uppernode->left=new_node;


    /* rebalance */
    finished = 0;
    while( path_st_p > 0 && !finished )
    {  int tmp_height, old_height;
       tmp_node = path_stack[--path_st_p];
       old_height= tmp_node->height;
       if( tmp_node->left->height -
                               tmp_node->right->height == 2 )
       {  if( tmp_node->left->left->height -
                               tmp_node->right->height == 1 )
          {  right_rotation( tmp_node );
             tmp_node->right->height =
                          tmp_node->right->left->height + 1;
             tmp_node->height = tmp_node->right->height + 1;
          }
          else
          {  left_rotation( tmp_node->left );
             right_rotation( tmp_node );
             tmp_height = tmp_node->left->left->height;
             tmp_node->left->height  = tmp_height + 1;
             tmp_node->right->height = tmp_height + 1;
             tmp_node->height = tmp_height + 2;
          }
       }
       else if ( tmp_node->left->height -
                              tmp_node->right->height == -2 )
       {  if( tmp_node->right->right->height -
                                tmp_node->left->height == 1 )
          {  left_rotation( tmp_node );
             tmp_node->left->height =
                         tmp_node->left->right->height + 1;
             tmp_node->height = tmp_node->left->height + 1;
          }
          else
          {  right_rotation( tmp_node->right );
             left_rotation( tmp_node );
             tmp_height = tmp_node->right->right->height;
             tmp_node->left->height  = tmp_height + 1;
             tmp_node->right->height = tmp_height + 1;
             tmp_node->height = tmp_height + 2;
          }
       }
       else /* update height even if there was no rotation */
       {  if( tmp_node->left->height > tmp_node->right->height )
             tmp_node->height = tmp_node->left->height + 1;
          else
             tmp_node->height = tmp_node->right->height + 1;
       }
       if( tmp_node->height == old_height )
          finished = 1;
    }

	}

    return result_tree;

}

tree_node_t *join(tree_node_t *tree1, tree_node_t *tree2, key_t separating_key) {

	tree_node_t *result_tree, *tmp_node, *tmp_node2, *test;
	tree_node_t *new_node;

	//printf(" tree1 , tree2 and key are %d , %d and %d \n",tree1->key,tree2->key,separating_key);
	new_node = get_node();
	new_node->key = separating_key;

	result_tree = tree1;
	tmp_node = tree1;
	tmp_node2 = tree2;

	tree_node_t * path_stack[100], *uppernode;
	int finished;
	int path_st_p = 0;

	if (tmp_node->height == tmp_node2->height) {

		if (separating_key <= tmp_node2->key) {

			new_node->left = tmp_node;
			new_node->right = tmp_node2;
			new_node->height = tmp_node2->height + 1;
			result_tree = new_node;

		} else {

			new_node->left = tmp_node2;
			new_node->right = tmp_node;
			new_node->height = tmp_node2->height + 1;
			result_tree = new_node;

		}


		return result_tree;

	} else {


		tree_node_t *swap;
		if (tmp_node->height < tmp_node2->height && separating_key<=tmp_node2->key) {
			swap=tmp_node;
			tmp_node=tmp_node2;
			tmp_node2=swap;


			result_tree=join_two(tmp_node,tmp_node2,separating_key);
			return result_tree;


		} else {

			if(tmp_node->height < tmp_node2->height){

				swap=tmp_node;
				tmp_node=tmp_node2;
				tmp_node2=swap;


			}
			if(tree1->key>=separating_key && tree2->key<separating_key)
			{
				result_tree=join_two(tmp_node,tmp_node2,separating_key);
				return result_tree;
			}
			else{

			while (tmp_node->right != NULL
					&& tmp_node->height != tmp_node2->height) {
				path_stack[path_st_p++] = tmp_node;
				tmp_node = tmp_node->right;

			}

			int p=path_st_p-1;

			p=path_st_p-1;
			uppernode = path_stack[p];
			new_node->left = tmp_node;
			new_node->right = tmp_node2;
			new_node->height = tmp_node2->height + 1;




				uppernode->right = new_node;
				uppernode->height = new_node->height;




			/* rebalance */
			finished = 0;
			while (path_st_p > 0 && !finished) {
				int tmp_height, old_height;
				tmp_node = path_stack[--path_st_p];
				old_height = tmp_node->height;
				if (tmp_node->left->height - tmp_node->right->height == 2) {
					if (tmp_node->left->left->height - tmp_node->right->height
							== 1) {
						right_rotation(tmp_node);
						tmp_node->right->height = tmp_node->right->left->height
								+ 1;
						tmp_node->height = tmp_node->right->height + 1;
					} else {
						left_rotation(tmp_node->left);
						right_rotation(tmp_node);
						tmp_height = tmp_node->left->left->height;
						tmp_node->left->height = tmp_height + 1;
						tmp_node->right->height = tmp_height + 1;
						tmp_node->height = tmp_height + 2;
					}
				} else if (tmp_node->left->height - tmp_node->right->height
						== -2) {
					if (tmp_node->right->right->height - tmp_node->left->height
							== 1) {
						left_rotation(tmp_node);
						tmp_node->left->height = tmp_node->left->right->height
								+ 1;
						tmp_node->height = tmp_node->left->height + 1;
					} else {
						right_rotation(tmp_node->right);
						left_rotation(tmp_node);
						tmp_height = tmp_node->right->right->height;
						tmp_node->left->height = tmp_height + 1;
						tmp_node->right->height = tmp_height + 1;
						tmp_node->height = tmp_height + 2;
					}
				} else /* update height even if there was no rotation */
				{
					if (tmp_node->left->height > tmp_node->right->height){



						tmp_node->height = tmp_node->left->height + 1;
					}
					else{
						//printf("tmp_node height %d,left %d, right %d  \n",tmp_node->height,tmp_node->left->height,tmp_node->right->height);

						tmp_node->height = tmp_node->right->height + 1;


					}
				}
				if (tmp_node->height == old_height){
					finished = 1;
				}
			}
			return result_tree;

		 }

		}

	}

}


tree_node_t *split(tree_node_t *tree, key_t splitting_key){

	tree_node_t *tmp_node,*test;


	tmp_node=tree;

	tree_node_t *tree1_stack[100],*tree2_stack[100],*temp;
	int tree1_keys[100],tree2_keys[100];
	int path1=0;int path2=0;
	int key_path1=0;int key_path2=0;
	while(tmp_node->right!=NULL)
	{

		if(splitting_key<=tmp_node->key){

			// takes the right tree out

			tree2_keys[key_path2++]=tmp_node->key;
			temp=tmp_node->right;
			tree2_stack[path2++]=temp;
			tmp_node=tmp_node->left;


		}
		else{


			// take the left tree out
						tree1_keys[key_path1++]=tmp_node->key;
						temp=tmp_node->left;
						tree1_stack[path1++]=temp;
						tmp_node=tmp_node->right;



		}
	}


	if(splitting_key <= tmp_node->key){

		tree2_keys[key_path2++]=splitting_key;
		tree2_stack[path2++]=tmp_node;

	}
	else{
		tree1_keys[key_path1++]=splitting_key;
		tree1_stack[path1++]=tmp_node;
	}




	/* join the right tree */

	tree_node_t *tree3, *tree4,*swap1;
	key_path2=key_path2-2;
	tree3= tree2_stack[--path2];
	tree4=tree2_stack[--path2];

	if(key_path2<0){

		tree->key = tree3->key;
		tree->right = tree3->right;
		tree->left = tree3->left;
		if(tree->left->height>tree->right->height){

			tree->height=tree->left->height+1;
		}
		else if(tree->left->height<tree->right->height){

			tree->height=tree->right->height+1;

		}else if(tree->left->height==tree->right->height)
		{
			tree->height=tree->right->height+1;

		}
		else{
			printf("exception caught\n");
		}


	}
	else{

		while (key_path2 >= 0) {

			if (tree3->height > tree4->height) {

			} else {

				swap1 = tree3;
				tree3 = tree4;
				tree4 = swap1;

			}

			tree3 = join_two(tree3, tree4, tree2_keys[key_path2]);
			key_path2--;
			tree4 = tree2_stack[--path2];
		}

		tree->key = tree3->key;
		tree->right = tree3->right;
		tree->left = tree3->left;



	}



	/* join the left tree1 */

	tree_node_t *tree1, *tree2;
	key_path1=key_path1-2;
	tree1= tree1_stack[--path1];
	tree2=tree1_stack[--path1];

	tree_node_t *swap;

	while(key_path1>=0){


if(tree1->height>tree2->height || tree1->height==tree2->height ){

}
else{
	swap=tree1;
	tree1=tree2;
	tree2=swap;

}

	tree1=join(tree1,tree2,tree1_keys[key_path1]);
	key_path1--;
	tree2=tree1_stack[--path1];

	}


	return tree1;

}






int main()
{  tree_node_t *st1, *st2, *trees[1000] ;
   int success;
   long i, j, k;
   int  *m;
   int o[3] = {0,2,4};
   for(i=0; i<1000; i++)
      trees[i] = create_tree();
   printf("Made one thousand Trees\n");
   for(i=0; i< 100000; i++)
   {  k = 3*i;
      success = insert( trees[0], k, &(o[1]) );
      if( success != 0 )
      {  printf("  insert %d failed in tree[0], return value %d\n",k, success);
         exit(-1);
      }
   }
   for(i=0; i< 100000; i++)
   {  k = 600000 - 3*i -3;
      success = insert( trees[1], k, &(o[1]) );
      if( success != 0 )
      {  printf("  insert %d failed in tree[1], return value %d\n",k, success);
         exit(-1);
      }
   }
   k = 600000;
   for(i=2; i<999; i++)
   {  for( j=0; j<200; j++)
      {  success = insert( trees[i], k, &(o[1]) );
         if( success != 0 )
         {  printf("  insert %d failed in tree[%d], return value %d\n",
               k, i, success);
            exit(-1);
         }
         k +=3;
      }
   }
   for( j=0; j<100000; j++)
   {  success = insert( trees[999], k, &(o[1]) );
      if( success != 0 )
      {  printf("  insert %d failed in tree[999], return value %d\n",
            k, success);
         exit(-1);
      }
      k +=3;
   }
   printf("Performed %d inserts\n", k/3); fflush(stdout);
   st1 = join(trees[0], trees[1], 300000);

   st2 = join( st1, trees[999], 600000);
   printf("Performed two large joins\n"); fflush(stdout);
   st1 = trees[2]; k = 600600;
   for(i=3; i<999; i++)
   {  st1 = join(st1, trees[i], k);
      k+=600;
   }
   printf("Performed many small joins\n"); fflush(stdout);

   // st2: the join of the 3 large trees
   // st1: the join of all the 997 small trees

   trees[0] = split( st2, 1000);
   trees[1] = split( st2, 600000);
   trees[0] = join( trees[0], trees[1], 1000);
   st2     = join( st1, st2, 600000 + 997*600);
   trees[0] = join( trees[0], st2, 600000);

   printf("Performed two splits and three more joins\n");
   for( i=0; i< 900000 + 997*600 -1; i++)
   {  m = find(trees[0],i);
      if( i%3 == 0  )
      {  if (m== NULL)
            printf(" find failed on st1 for %d, returned NULL\n", i);
         else if (*m != 2 )
            printf(" find failed on st1 for %d, returned %d instead of 2\n", i,*m );
      }
      else if (m != NULL)
            printf(" find failed on tree[0] for %d, returned non-NULL pointer\n", i);
   }
   printf("Passed %d finds. End of test.\n",900000 +997*600-2);
   fflush(stdout);
   return(0);

}
