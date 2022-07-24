# Operators `.*` (dot asterisk):

Ref guide at: [stack overflow][1]

1. The `.*` is a pointer-to-member operator for use with pointer-to-member types.
    - Example:
      ```
      struct A
      {
          int a;
          int b;
      };

      int main()
      {
          A obj;
          int A::* ptr_to_memb = &A::b;
          obj.*ptr_to_memb = 5;
          ptr_to_memb = &A::a;
          obj.*ptr_to_memb = 7;
          // Both members of obj are now assigned
      }
      ```
    - In the above example `A` is a struct and the ptr_to_memb is a pointer to  
      int member of `A`. The `.*` combines an `A` instance with a pointer to  
      member to form an lvalue expression referring to the appropriate member  
      of the given `A` instance obj.

[1]: <https://stackoverflow.com/questions/2548555/dot-asterisk-operator-in-c#:~:text=*%20combines%20an%20A%20instance%20with,thing'%20for%20virtual%20function%20members.> "stackoverflow"
