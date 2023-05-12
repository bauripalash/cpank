# Pankti Internal Memo #2 

```
Title: Pankti Import and Module System
Author: Palash Bauri <me [at] palashbauri [dot] in>
		<palashbauri1 [at] gmail [dot] com>
Status: Draft
Created: April 23, 2023 16:25:04 IST
Last Modified: April 10, 2023 10:35:30 IST
```

## Abstract
This memo describes and mentions all the standard library and builtin functions
which must be present in all Implementation of Pankti Programming Language

## Standard Library
### 1. OS 
#### Description:
Utility functions for fetching information related to the current operating 
system.

#### Module Name: os / ওএস

#### Functions
##### <sub>os</sub>.**name()** -> String

Returns the name of operating system presently being used.  
On Windows returns 'windows'  
On Linux based distribution and on other Unix-like OS, returns 'unix'  
On MacOS returns 'macos'  
On Android returns 'android'  
On Others or if failed to detect, returns 'unknown'  


##### <sub>os</sub>.**arch()** -> String


On x86_64 / amd64 / 64-bit  systems, it returns '64'.  
On x86 / 32-bit systems, it returns '32'.   
On arm systems, it returns 'arm'.  
On other systems or if failed to detect, it returns 'unknown'. 

##### <sub>os</sub>.**user()** -> String

Returns the user name of current users  

##### <sub>os</sub>.**home()** -> String
Returns the home directory of the current user

##### <sub>os</sub>.**curdir()** -> String
Returns current working directory

### 2.  Array
#### Description:
Helper functions to modify and take advantage of arrays

#### Module Name: array / 

#### Functions

##### <sub>array</sub>.**pop(input : Array)** -> Nil
Removes the last element of the array `input`. Mutates the input array. Always returns nil.

##### <sub>array</sub>.**push(arr : Array , v : Value)** -> Nil
Add the `v` (any type) to the end of `arr` by mutating the `arr` Array. Always returns nil

##### <sub>array</sub>.**join(a : Array , b : Array)** -> Array

Returns a new Array by joining `a` and `b`. Doesn't mutate `a` or `b`.

##### <sub>array</sub>.**popat(arr : Array , index : Number)** -> Value

Removes the `N`th element of `arr` where `N = index` (in Pankti first element is 0th), returns the removed element Value.

`index` must be non non negative integer.

### 3.  Big
#### Description:
Big Numbers, creating and using functions.
#### Module Name: big / 
#### Functions

##### <sub>big</sub>.**new(v : Value)** -> Big Number

Creates a new big number from `v`, which can be a number or string. When manually creating big numbers it is recommended to use strings as `v`

##### <sub>big</sub>.**add(a : Big Number , b : Big Number)** -> Big Number

Returns `a+b`

#### <sub>big</sub>.**sub(a : Big Number , b : Big Number)** -> Big Number

Returns `a-b`

##### <sub>big</sub>.**gt(a : Big Number , b : Big Number)** -> Bool

Returns `true` if `a > b` else returns `false`

##### <sub>big</sub>.**lt(a : Big Number , b : Big Number)** -> Bool

Returns `true` if `a < b` else returns `false`


##### <sub>big</sub>.**eq(a : Big Number , b : Big Number)** -> Bool

Returns `true` if `a == b` else returns `false`


##### <sub>big</sub>.**noteq(a : Big Number , b : Big Number)** -> Bool

Returns `true` if `a != b` else returns `false`


##### <sub>big</sub>.**pi()** -> Big Number

Returns the value of `Pi` as Big Number

##### <sub>big</sub>.**e()** -> Big Number

Returns the value of  `e` as Big Number

### 4.  Common
#### Description:
Some common utility functions
#### Module Name: common / 
#### Functions
##### <sub>common</sub>.**readline()** -> String
Read line from Standard Input, and return the line as string.  
If failed to read line or failed to allocate memory for reading line, returns empty string

### 5.  File
#### Description:
Utility functions to interact with File system
#### Module Name: file / 
#### Functions

##### <sub>file</sub>.**exists(path : String)** -> Boolean
Check if `path` exists in file system.

##### <sub>file</sub>.**readfile(path : String)** -> String
Reads a file at `path` and returns the contents of the file as string.  
If File doesn't exist return an empty string

##### <sub>file</sub>.**isfile(path : String)** -> Boolean
Check if `path` points to a file.

##### <sub>file</sub>.**isdir(path : String)** -> Boolean
Check if `path` points to a directory.

##### <sub>file</sub>.**create_empty(path : String)** -> Boolean
Create an empty file at `path`. Returns `true` if successful else returns `false`

##### <sub>file</sub>.**rename(old : String, new : String)** -> Boolean
Renames `old` to `new`. Returns `true` if successful else returns `false`

##### <sub>file</sub>.**delete(path : String)** -> Boolean
Removes `path`. Returns `true` if successful else returns `false`

##### <sub>file</sub>.**write(path : String , data : String)** -> Boolean
Writes `data` to file at `path`. This function will overwrite any data already present in `path`, if `path` doesn't exist it will try to create a file there. Returns true if successful, otherwise make the program create an error and exit.

##### <sub>file</sub>.**append(path : String , data : String)** -> Boolean
Writes `data` at the end of the file at `path`. This function will not overwrite any data already present in `path. Returns true if successful, otherwise make the program create an error and exit.

### 6.  Map
#### Description:
Utility functions for Hashmaps/Hashtables
#### Module Name: map / 
#### Functions
##### <sub>map</sub>.**iskey(m : Map , v : Value)** -> Boolean
Returns true if `v` is an key of `m` hashtable, else returns `false`

##### <sub>map</sub>.**keys(m : Map)** -> Array
Returns all keys of `m` as an Array

##### <sub>map</sub>.**values(m : Map)** -> Array
Returns all values of `m` as an Array


### 7.  String
#### Description:
Utility functions for Strings
#### Module Name: str / 
#### Functions

##### <sub>str</sub>.**split(input : String , delim : String)** -> Array
Splits the string `input` with `delim` as delimiter and returns the parts as an array

##### <sub>str</sub>.**string(v : Value)** -> String
Returns the string form of `v`


### 8.  Math
#### Description:
Some common Mathematics functions
#### Module Name: math / 
#### Functions
#### <sub>math</sub>.**pow(a : Number , b : Number)** -> Number
Returns `a`<sup>b</sup>. $\a^b$\

#### <sub>math</sub>.**gcd(a : Number , b : Number)** -> Number
Returns greatest common divisor (GCD) of `a` and `b`

#### <sub>math</sub>.**lcm(a : Number , b : Number)** -> Number
Returns least common multiplier  (LCM) of `a` and `b`$\lcm(a, b)$\