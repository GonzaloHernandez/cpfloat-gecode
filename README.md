N-P problems, which are problems that cannot be solved with polynomial complexity, can be solved with different techniques such as: artificial intelligence, genetic algorithms, constraint scheduling, etc.

The constraint programming technique proposes a domain reduction method in order to speed up the search for solutions. This process is carried out systematically and by filtering values that quickly discard those values that are not part of a solution.

These problems can be modeled as constraints on variables of different types such as integers, booleans, sets, and reals. These last ones that manage continuous domains, have not been very worked by the search engines based on restrictions.

Gecode is one of the free-to-use engines that allow you to solve N-P problems with constraint programming. However, so far it does not have a module that allows modeling problems with real type variables.

With this research, a module is proposed that can be added to Gecode in such a way that it can be used in the solution of this type of problem.
