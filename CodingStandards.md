# Introduction #

Not surprisingly, I am extremely picky about the format and presentation of code I write.  A paramount consideration for me is the readability of the code.  I tend to heavily favor readable code in a trade-off with highly optimized code.  My experience has shown that over time readable code gets refactored and improved (usually to within a hair of the optimized code) while difficult to understand code just grows stale.  In an effort to promote highly consistent and readable code within the MGALib project, below I outline the coding standards for this project.  This list is adapted from that found at [Possibility.com](http://www.possibility.com/Cpp/CppCodingStandard.html).  I additionally lean heavily upon [Effective C++ by Scott Meyers](http://www.aristeia.com/).

# Coding Standards #

## Classes ##

As MGALib is a C++ library striving to be as C++-ish as possible we shall start here.

### Class Definition Structure ###

Classes should be defined in a specific order to make it as easy as possible to understand what the class is trying to accomplish and how you will interact with the class.

### Class Names ###

Class names should reflect the intent of the class as much as possible.  Do not shy away from lengthy names, but don't go too crazy.  Use some common sense here.  Here are the primary rules:
  * Start class names with a single capital letter.  Each sub-word within a name also gets a capital letter.
  * Use only 'a'-'z', 'A'-'Z', and 0-9.  Do not use any dashes, hyphens or other crap.  Keep it simple.

### Class Attribute Names ###

Classes contain data members also called Attributes.  I have a deep preference for not making any attributes public, only protected and private.  Why let just anyone muck around with your data?
  * Start class attributes with an underscore and then a lower case letter.  Each sub-word within the name gets a capital letter.
  * Again, after the initial underscore, use only 'a'-'z', 'A'-'Z', and 0-9.  Do not use any dashes, hyphens or other crap.  Keep it simple.

### Class Method Names ###
> ...