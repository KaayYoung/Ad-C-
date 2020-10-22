There are 6 test files totally.
The first test file is the basic test file which is given to us

In the second test file, because constructor is the base of the program, so I test constructor first
The method I test constructors is first initialize a graph with nodes and edges,
then create multiple sections in test file, use each of the function mentioned in 2.2 Constructors,
check whether they have the same nodes and edges from the initial graph after calling the function,
then do few basic operations to see whether the graph can work fine.

In the third test file, I test Accessors first because functions from Accessors are used often when test Modifers.
The logic is create one section for each of the function from Accessors, then insert nodes and edges,
test corresponding function, because this file is used to test Accessors, I won't put too many Modifiers'
functions in this test file.

Next the fourth test file is used to test Modifiers. For each function, I use three different test cases
to test them, one is the basic case, one is the case will occur exception or return false, and one
harder case. In this file, the test is usually combined with Constructors, Accessors, even iterator.
Hence Coverage is great.

Then the fifth test file is used to test range access and iterator. To test the operations' correctness,
I deference them first, for example, I use [a, b, c] to stand for the deference format of an iterator
and check whether a, b, c is equal to from, to and weight

Last file is for testing comparison and extractor, the method I test them is starting from a initial
graph, then each time I insert/erase/replace nodes/edges to see whether the comparison and extractor can also
work fine with these changes
