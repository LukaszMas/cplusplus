# Can you write a C++ program without main() ?

Short answer: Yes.

We are thought in almost every book that `main()` is the required entry point  
of a program for correct execution. Just from the perspective of a programmer,  
at an abstract level this is true. But from the system perspective a program  
must only have the entry points that are required by the operating system on  
which it will run.

This little project is meant to give an insight on writing a C++ program  
without the entry point of a program in form of `main()` function but the  
`_start()` function that is defined by the standard as a public entry point.  
Keep in mind, that any global name starting with an underscore is a reserved  
keyword and is not really meant to be used by anybody but the developers of  
the C libraries or compilers.  

Also, this solution is completely implementation/compiler specific.  
In a different implementation you may find similar function names like:  
`go()`, `run()`, `fly()`, etc..

Happy exploring & learning something new!

# Inspired by discussion on Quora:
[Can a C program be written without main()?][1]

[1]: <https://www.quora.com/Can-a-C-program-be-written-without-main/answer/Mohd-Saquib-211#comments>
