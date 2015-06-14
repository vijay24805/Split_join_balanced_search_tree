# Split_join_balanced_search_tree
Splitting and joining random balanced search trees

The function , tree node t *join(tree node t *tree1, tree node t *tree2,has trees tree1 and tree2 and the separating key as input. All keys in tree1 are < separating key, all keys in tree2 are ≥ separating key. The function returns the joined tree.

The function tree node t *split(tree node t *tree, key t splitting key), has one tree and the splitting key as input. It constructs two trees: the one containing all keys smaller than the splitting key is returned by the function, the one that contains the remaining keys has the root in the node tree which was the root of the input tree.
