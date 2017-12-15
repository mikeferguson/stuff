# C++ Style Guide

In general our C++ style guide follows:

 * [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.html)
 * [ROS C++ Style Guide](http://wiki.ros.org/CppStyleGuide)

## Naming:

 * Package, topic, file names are __under_scored__. So are variable names.
 * Class names are __CamelCased__.
 * Functions are __camelCased__.
 * Constants are __ALL_CAPITALS__.
 * Private member variables have trailing underscore.

## Whitespace

By way of example:

    #ifndef PACKAGE_NAME_HEADER_NAME_H
    #define PACKAGE_NAME_HEADER_NAME_H

    namespace foo
    {  // Braces on own line (ROS style, not Google).

    // Do not indent code inside namespace
    class ReallyReallyLongNameShouldNotBeUsed
    {
    public:  // do not indent public/private/etc
      ReallyReallyLongNameShouldNotBeUsed(
        int indent_arguments_2_spaces,
        int yep_2_spaces) :
          indent_initializers_how_many_spaces(4)
      {
        // Standard indentation is 2 spaces.
      }
    };

    }  // namespace foo (note that comment is 2 spaces from code)

    #endif  // PACKAGE_NAME_HEADER_NAME_H

## Code Maintainability

 * [Constructors callable with one argument must be explicit.](http://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Explicit_Constructors)
 * [Use C++ style casts](http://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Casting)
 * Use ``TODO`` comments to note pieces of code that are not really finished.
 * Use of braces: put them on their own line, put them in always.
 * All packages should be compiled with ``-Wall``.

## Comments and Debugging Aids

 * Use ``/** */`` around doxygen comments. Prefer the ``@brief/@param`` style names.
   Put all doxygen in the header file.
 * Use ``//`` inside body of functions so that entire functions can be easily
   commented out.
 * Only spaces, no tabs (setting your editor this way prevents you making tabs in Python too)
 * When using ``ROS_DEBUG``, use the ``NAMED`` version with a relevant name:
   ```
   ROS_DEBUG_NAMED("package_name", "This is my message");
   ```

## Define Guards

All header files should have a guard. Guards are of the form
PACKAGE_NAME_FILE_PATH_H. Path will not include the INCLUDE_PACKAGE_NAME part.
Should have comment with #endif to tell us what file this:

    #ifndef PACKAGE_NAME_FILE_PATH_H
    #define PACKAGE_NAME_FILE_PATH_H

    // This matches ROS style guide,
    // Google style guide has trailing underscore.

    #endif  // PACKAGE_NAME_FILE_PATH_H

## Namespaces

``using namespace std;`` is terrible. ``using std::list`` is OK.

## Preferred 3rd Party Libraries

 * Eigen for matrix math.
 * gtest for testing.
